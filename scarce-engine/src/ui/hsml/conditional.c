#include "conditional.h"
#include "dynamic_array.h"
#include "logging/logger.h"
#include "memory/memory.h"
#include "platform/platform.h"
#include <ctype.h>

static hsml_conditional_type hsml_parse_conditional_symbol(file_descriptor descriptor, char current, char next)
{
    if (current == '(')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_COND_OPEN;
    }
    if (current == ')')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_COND_CLOSE;
    }
    if (current == '>' && next != '=')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_COND_GRT;
    }
    if (current == '<' && next != '=')
    {
        platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
        return HSML_COND_LSS;
    }

    if (current == '=' && next == '=')
        return HSML_COND_EQU;
    if (current == '!' && next == '=')
        return HSML_COND_NEQ;

    if (current == '>' && next == '=')
        return HSML_COND_GEQ;
    if (current == '<' && next == '=')
        return HSML_COND_LEQ;

    if (current == '&' && next == '&')
        return HSML_COND_AND;
    if (current == '|' && next == '|')
        return HSML_COND_OR;

    return HSML_COND_INVALID;    
}

static u8 hsml_get_precedence(hsml_conditional_type type)
{
    switch(type)
    {
        case HSML_COND_NEQ:
        case HSML_COND_EQU:
        case HSML_COND_GRT:
        case HSML_COND_LSS:
        case HSML_COND_GEQ:
        case HSML_COND_LEQ:
            return 2;

        case HSML_COND_AND:
        case HSML_COND_OR:
            return 1;

        default:
            return 0;
    }

    return 0;
}

static dynamic_array hsml_get_conditional_array(file_descriptor descriptor, memory_pool* pool)
{
    dynamic_array conditionalArray = { 0 };
    dynamic_array_init(&conditionalArray, 16, sizeof(hsml_conditional));

    u8 placeholderAmount = pool[SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - 1];

    bool possiblyValid = false;
    while (true)
    {
        char current;
        if (!platform_read_file(descriptor, &current, 1))
            break;

        if (current == ';')
        {
            possiblyValid = true;
            break;
        }

        if (current == ' ')
            continue;

        if (isdigit(current))
        {
            u32 value = (current == '%') ? 0 : current - '0';
            while(true)
            {   
                char next;
                if (!platform_read_file(descriptor, &next, 1) || !isdigit(next))
                {
                    platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
                    break;
                }

                value = (value * 10) + (next - '0');
            }

            hsml_conditional conditional = { .value = value, .type = HSML_COND_NUMBER };
            dynamic_array_push(&conditionalArray, &conditional, 1);
            continue;
        }

        if (current == '%')
        {
            bool foundValue = false;
            u32 index = (current == '%') ? 0 : current - '0';
            while(true)
            {   
                char next;
                if (!platform_read_file(descriptor, &next, 1) || !isdigit(next))
                {
                    platform_file_seek(descriptor, SEEK_MODE_CURRENT, -1);
                    break;
                }

                foundValue = true;
                index = (index * 10) + (next - '0');
            }

            if (!foundValue)
                log_critical_s("Invalid HSML: invalid index passed to conditional expression placeholder.", 74);

            u8 value = pool[SCA_STACK_ADDRESS + pool[SCA_STACK_SIZE_ADDRESS] - placeholderAmount + index - 1];

            hsml_conditional conditional = { .value = value, .type = HSML_COND_NUMBER };
            dynamic_array_push(&conditionalArray, &conditional, 1);
            continue;
        }
        
        char next;
        platform_read_file(descriptor, &next, 1);
        hsml_conditional_type type = hsml_parse_conditional_symbol(descriptor, current, next);
        if (type == HSML_COND_INVALID)
            log_critical_s("Invalid HSML: invalid conditional symbol", 41);

        hsml_conditional conditional = { .type = type };
        dynamic_array_push(&conditionalArray, &conditional, 1);
    }

    if (!possiblyValid)
        log_critical_s("Invalid HSML: invalid conditional expression", 45);

    return conditionalArray;
}

static dynamic_array shunt_conditional_expression(dynamic_array* expression)
{
    dynamic_array output = { 0 };
    dynamic_array operators = { 0 };
    dynamic_array_init(&output, 16, sizeof(hsml_conditional));
    dynamic_array_init(&operators, 16, sizeof(hsml_conditional));

    hsml_conditional* data = (hsml_conditional*)expression->buffer;
    hsml_conditional* operatorData = (hsml_conditional*)operators.buffer;

    for (u32 i = 0; i < dynamic_array_size(expression); ++i)
    {
        hsml_conditional* current = &data[i];
        
        if (current->type == HSML_COND_NUMBER)
        {
            dynamic_array_push(&output, current, 1);
            continue;
        }
        if (current->type == HSML_COND_OPEN)
        {
            dynamic_array_push(&operators, current, 1);
            continue;
        }
        else if (current->type == HSML_COND_CLOSE)
        {
            while(true)
            {
                if (dynamic_array_empty(&operators))
                    log_critical_s("Invalid HSML: matching parenthesis not found", 45);

                current = &operatorData[dynamic_array_size(&operators) - 1];

                if (current->type == HSML_COND_OPEN)
                {
                    dynamic_array_pop(&operators, 1);
                    break;
                }

                dynamic_array_push(&output, current, 1);
                dynamic_array_pop(&operators, 1);
            }
            continue;
        }

        if (dynamic_array_empty(&operators) || operatorData[0].type == HSML_COND_OPEN)
        {
            dynamic_array_push(&operators, current, 1);
            continue;
        }
        
        u8 currentPrecedence = hsml_get_precedence(current->type);
        while (!dynamic_array_empty(&operators)) 
        {
            hsml_conditional* top = &operatorData[dynamic_array_size(&operators) - 1];
            
            if (top->type == HSML_COND_OPEN)
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
        hsml_conditional* top = &operatorData[dynamic_array_size(&operators) - 1 - i];
        dynamic_array_push(&output, top, 1);
    }

    dynamic_array_destroy(&operators);
    return output;
}

static bool hsml_evaluate_postfix(dynamic_array* output)
{
    dynamic_array stack = { 0 };
    dynamic_array_init(&stack, 8, sizeof(u32));

    u32* stackData = (u32*)stack.buffer;
    hsml_conditional* data = (hsml_conditional*)output->buffer;
    for (u32 i = 0; i < dynamic_array_size(output); ++i)
    {
        hsml_conditional* current = &data[i];

        if (current->type == HSML_COND_NUMBER)
        {
            dynamic_array_push(&stack, &current->value, 1);
            continue;
        }

        if (dynamic_array_size(&stack) < 2)
            break;

        u32 rhs = stackData[dynamic_array_size(&stack) - 1];
        dynamic_array_pop(&stack, 1);
        u32 lhs = stackData[dynamic_array_size(&stack) - 1];
        dynamic_array_pop(&stack, 1);

        u32 partial = 0;
        switch(current->type) 
        {
            case HSML_COND_EQU: partial = (lhs == rhs); break;
            case HSML_COND_NEQ: partial = (lhs != rhs); break;
            case HSML_COND_GRT: partial = (lhs >  rhs); break;
            case HSML_COND_LSS: partial = (lhs <  rhs); break;
            case HSML_COND_GEQ: partial = (lhs >= rhs); break;
            case HSML_COND_LEQ: partial = (lhs <= rhs); break;
            case HSML_COND_AND: partial = (lhs && rhs); break;
            case HSML_COND_OR:  partial = (lhs || rhs); break;
            default:
                break;
        }

        dynamic_array_push(&stack, &partial, 1);
    }

    bool result = (!dynamic_array_empty(&stack)) ? stackData[0] : false;
    dynamic_array_destroy(&stack);

    return result;
}

bool hsml_get_conditional_result(file_descriptor descriptor, memory_pool* pool)
{
    dynamic_array conditionalArray = hsml_get_conditional_array(descriptor, pool);
    dynamic_array output = shunt_conditional_expression(&conditionalArray);
    
    bool result = hsml_evaluate_postfix(&output);

    dynamic_array_destroy(&conditionalArray);
    dynamic_array_destroy(&output);
    return result;
}