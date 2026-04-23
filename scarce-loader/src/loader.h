#pragma once
#include "scarce.h"

void* get_application_space(const char* filepath);
void  get_exported_functions(void* applicationSpace, load_func* onLoad, update_func* onUpdate, unload_func* onUnload);