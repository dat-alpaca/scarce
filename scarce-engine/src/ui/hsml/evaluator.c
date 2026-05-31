#include "evaluator.h"
#include "dynamic_array.h"
#include "logging/logger.h"
#include "memory/tag.h"
#include "platform/platform.h"
#include "ui/hsml/defines.h"
#include "ui/hsml/placeholder.h"
#include "ui/hsml/token.h"

#include <ctype.h>

static hsml_operator_type hsml_parse_operators(file_descriptor descriptor, char current, char next)
{
    if (current == '*')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_OPERATOR_MUL;
    }
    if (current == '/')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_OPERATOR_DIV;
    }
    if (current == 'm')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_OPERATOR_MOD;
    }
    if (current == '+')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_OPERATOR_PLS;
    }
    if (current == '-')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_OPERATOR_PLS;
    }

    if (current == '(')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_OPERATOR_OPEN;
    }
    if (current == ')')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_OPERATOR_CLOSE;
    }
    if (current == '>' && next != '=')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_OPERATOR_GRT;
    }
    if (current == '<' && next != '=')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_OPERATOR_LSS;
    }

    if (current == '=' && next == '=')
        return HSML_OPERATOR_EQU;
    if (current == '!' && next == '=')
        return HSML_OPERATOR_NEQ;

    if (current == '>' && next == '=')
        return HSML_OPERATOR_GEQ;
    if (current == '<' && next == '=')
        return HSML_OPERATOR_LEQ;

    if (current == '&' && next == '&')
        return HSML_OPERATOR_AND;
    if (current == '|' && next == '|')
        return HSML_OPERATOR_OR;

    return HSML_OPERATOR_INVALID;    
}

static u8 hsml_get_precedence(hsml_operator_type type)
{
    switch(type)
    {
        case HSML_OPERATOR_MUL:
        case HSML_OPERATOR_DIV:
        case HSML_OPERATOR_MOD:
            return 4;

        case HSML_OPERATOR_MNS:
        case HSML_OPERATOR_PLS:
            return 3;

        case HSML_OPERATOR_NEQ:
        case HSML_OPERATOR_EQU:
        case HSML_OPERATOR_GRT:
        case HSML_OPERATOR_LSS:
        case HSML_OPERATOR_GEQ:
        case HSML_OPERATOR_LEQ:
            return 2;

        case HSML_OPERATOR_AND:
        case HSML_OPERATOR_OR:
            return 1;

        default:
            return 0;
    }

    return 0;
}

static dynamic_array hsml_get_expression_array(ui_state* state, file_descriptor descriptor)
{
    dynamic_array conditionalArray = { 0 };
    dynamic_array_init(&conditionalArray, 16, sizeof(hsml_evaluation), TAG_HSML);

    bool possiblyValid = false;
    while (true)
    {
        char current;
        if (!platform_read_file(descriptor, &current, 1) || current == '\n' || current == '\0')
            break;

        if (current == HSML_TOKEN_DELIMITER)
        {
            possiblyValid = true;
            break;
        }

        if (current == ' ')
            continue;

        if (isdigit(current))
        {
            platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
            u32 value = 0;
            while(true)
            {   
                if (!platform_read_file(descriptor, &current, 1) || !isdigit(current))
                {
                    platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
                    break;
                }

                value = (value * 10) + (current - '0');
            }

            hsml_evaluation conditional = { .value = value, .type = HSML_OPERATOR_NUMBER };
            dynamic_array_push(&conditionalArray, &conditional, 1);
            continue;
        }

        if (current == HSML_TOKEN_PLACEHOLDER)
        {
            hsml_token placeholder = hsml_fetch_placeholder_value(state, descriptor);
            if (placeholder.type == HSML_TOKEN_TEXT)
                log_critical_s("Invalid HSML: expected value", 29);

            u64 value = hsml_fetch_number_from_placeholder(&placeholder);
            dynamic_array_destroy(&placeholder.value);

            hsml_evaluation conditional = { .value = value, .type = HSML_OPERATOR_NUMBER };
            dynamic_array_push(&conditionalArray, &conditional, 1);
            continue;
        }
        
        char next;
        platform_read_file(descriptor, &next, 1);
        hsml_operator_type type = hsml_parse_operators(descriptor, current, next);
        if (type == HSML_OPERATOR_INVALID)
            log_critical_s("Invalid HSML: invalid conditional symbol", 41);

        hsml_evaluation conditional = { .type = type };
        dynamic_array_push(&conditionalArray, &conditional, 1);
    }

    if (!possiblyValid)
        log_critical_s("Invalid HSML: invalid conditional expression", 45);

    return conditionalArray;
}

static dynamic_array shunt_expression(dynamic_array* expression)
{
    dynamic_array output = { 0 };
    dynamic_array operators = { 0 };
    dynamic_array_init(&output, 16, sizeof(hsml_evaluation), TAG_HSML);
    dynamic_array_init(&operators, 16, sizeof(hsml_evaluation), TAG_TRANSIENT);

    hsml_evaluation* data = (hsml_evaluation*)expression->buffer;
    hsml_evaluation* operatorData = (hsml_evaluation*)operators.buffer;

    for (u32 i = 0; i < dynamic_array_size(expression); ++i)
    {
        hsml_evaluation* current = &data[i];
        
        if (current->type == HSML_OPERATOR_NUMBER)
        {
            dynamic_array_push(&output, current, 1);
            continue;
        }
        if (current->type == HSML_OPERATOR_OPEN)
        {
            dynamic_array_push(&operators, current, 1);
            continue;
        }
        else if (current->type == HSML_OPERATOR_CLOSE)
        {
            while(true)
            {
                if (dynamic_array_empty(&operators))
                    log_critical_s("Invalid HSML: matching parenthesis not found", 45);

                current = &operatorData[dynamic_array_size(&operators) - 1];

                if (current->type == HSML_OPERATOR_OPEN)
                {
                    dynamic_array_pop(&operators, 1);
                    break;
                }

                dynamic_array_push(&output, current, 1);
                dynamic_array_pop(&operators, 1);
            }
            continue;
        }

        if (dynamic_array_empty(&operators) || operatorData[0].type == HSML_OPERATOR_OPEN)
        {
            dynamic_array_push(&operators, current, 1);
            continue;
        }
        
        u8 currentPrecedence = hsml_get_precedence(current->type);
        while (!dynamic_array_empty(&operators)) 
        {
            hsml_evaluation* top = &operatorData[dynamic_array_size(&operators) - 1];
            
            if (top->type == HSML_OPERATOR_OPEN)
                break;

            if (hsml_get_precedence(top->type) < currentPrecedence) 
                break;

            dynamic_array_push(&output, top, 1);
            dynamic_array_pop(&operators, 1);
        }

        dynamic_array_push(&operators, current, 1);
    }

    for (u32 i = 0; i < dynamic_array_size(&operators); ++i)
    {
        hsml_evaluation* top = &operatorData[dynamic_array_size(&operators) - 1 - i];
        dynamic_array_push(&output, top, 1);
    }

    dynamic_array_destroy(&operators);
    return output;
}

static i32 hsml_evaluate_postfix(dynamic_array* output)
{
    dynamic_array stack = { 0 };
    dynamic_array_init(&stack, 8, sizeof(i32), TAG_TRANSIENT);

    i32* stackData = (i32*)stack.buffer;
    hsml_evaluation* data = (hsml_evaluation*)output->buffer;
    for (u32 i = 0; i < dynamic_array_size(output); ++i)
    {
        hsml_evaluation* current = &data[i];

        if (current->type == HSML_OPERATOR_NUMBER)
        {
            dynamic_array_push(&stack, &current->value, 1);
            continue;
        }

        if (dynamic_array_size(&stack) < 2)
            break;

        i32 rhs = stackData[dynamic_array_size(&stack) - 1];
        dynamic_array_pop(&stack, 1);
        i32 lhs = stackData[dynamic_array_size(&stack) - 1];
        dynamic_array_pop(&stack, 1);

        i32 partial = 0;
        switch(current->type) 
        {
            case HSML_OPERATOR_MUL: partial = (lhs * rhs); break;
            case HSML_OPERATOR_DIV: partial = (lhs / rhs); break;
            case HSML_OPERATOR_MOD: partial = (lhs % rhs); break;
            case HSML_OPERATOR_PLS: partial = (lhs + rhs); break;
            case HSML_OPERATOR_MNS: partial = (lhs - rhs); break;

            case HSML_OPERATOR_EQU: partial = (lhs == rhs); break;
            case HSML_OPERATOR_NEQ: partial = (lhs != rhs); break;
            case HSML_OPERATOR_GRT: partial = (lhs >  rhs); break;
            case HSML_OPERATOR_LSS: partial = (lhs <  rhs); break;
            case HSML_OPERATOR_GEQ: partial = (lhs >= rhs); break;
            case HSML_OPERATOR_LEQ: partial = (lhs <= rhs); break;
            case HSML_OPERATOR_AND: partial = (lhs && rhs); break;
            case HSML_OPERATOR_OR:  partial = (lhs || rhs); break;
            default:
                break;
        }

        dynamic_array_push(&stack, &partial, 1);
    }

    i32 result = (!dynamic_array_empty(&stack)) ? stackData[0] : false;
    dynamic_array_destroy(&stack);

    return result;
}

i32 hsml_get_expression_evaluation(ui_state* state, file_descriptor descriptor)
{
    dynamic_array conditionalArray = hsml_get_expression_array(state, descriptor);
    dynamic_array output = shunt_expression(&conditionalArray);
    
    i32 result = hsml_evaluate_postfix(&output);

    dynamic_array_destroy(&conditionalArray);
    dynamic_array_destroy(&output);
    return result;
}