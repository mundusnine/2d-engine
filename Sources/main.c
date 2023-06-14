#include "main.h"

#include <stdlib.h>
#include <string.h>

#include <assert.h>

#include "nativeplugins.h"
#ifdef set_api_require
#undef set_api_require
#endif

#include <kinc/global.h>
#include <kinc/global.h>
#include <kinc/log.h>
#include <kinc/system.h>
#include <kinc/display.h>
#include <kinc/threads/atomic.h>

#include <krink/system.h>
#include <krink/graphics2/graphics.h>

#include "renderer.h"
#include "system.h"

size_t mem_size;
void* memblck;

int module_init(void){

  kinc_display_init();
  kinc_display_mode_t dm = kinc_display_current_mode(kinc_primary_display());

  kinc_init("middle-engine",dm.width * 0.8,dm.height * 0.8,NULL,NULL);

  mem_size = 1024 * 1024 * 1024;//1 GiB
  memblck = malloc(mem_size);
  memset(memblck,0,mem_size);
  kr_init(memblck,mem_size,NULL,0);
  kr_g2_init();
  kr_evt_init();
  kr_evt_add_observer(luaevent_handler);
}

int isShutdown = 0;
int module_quit(void){
  isShutdown = 1;
  kr_g2_destroy();
  free(memblck);
  kinc_stop();
}

#define P(FUNC) { "" #FUNC, (fptr)(FUNC) }
static void* engine_api_require(const char* symbol) {
  static const function_node nodes[] = {
    P(ren_begin_frame),P(ren_end_frame),P(ren_draw_rect),P(ren_draw_tri),
    P(module_set_update),P(poll_event),

  };
  for (size_t i = 0; i < sizeof(nodes) / sizeof(function_node); ++i) {
    if (strcmp(nodes[i].symbol, symbol) == 0)
      return *(void**)(&nodes[i].address);
  }
  return NULL;
}

int module_start(void (*set_api_require)(void* (*func)(const char* /*symbol*/))){
  set_api_require(engine_api_require);
  kinc_start();
}

#define MAX_FUNCS 32
size_t f_len = 0;
int (*f_update[MAX_FUNCS])(void* /*data*/) = {NULL};
static int last_id = -1;
int module_set_update(int (*func)(void* /*data*/),int id){
  assert(id > 0 || func != NULL );
  //Don't reset to null if it's already null.
  if(func == NULL && id >0 && f_update[id] == NULL)
    return -1;
  size_t p_id = id > 0 ? id : last_id > 0 ? last_id :  f_len++;
  f_update[p_id] = func;
  if(p_id == id){
    last_id = id;
  }
  else{
    last_id = -1;
  }
  return p_id;
}
double last_time = 0.0;
void native_main_loop(void* udata){
  loop_t* data = (loop_t*)udata;
  if(*(data->end_state) || isShutdown)
    return;
  *(data->dt) = kinc_time() - last_time;
  last_time = kinc_time();
  if(f_len >0){
    for(int i = 0; i < f_len;++i){
      if(isShutdown)
        break;
      if(f_update[i] != NULL)
        f_update[i](data);
    }
  }
}