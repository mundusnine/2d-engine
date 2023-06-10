#pragma once

int module_init(void);
int module_quit(void);

int module_start(void (*set_api_require)(void* (*func)(const char* /*symbol*/)));

void native_main_loop(void* udata);