#include <bsd/bsd.h>
#include <bsd/stdio.h>
#include <bsd/string.h>
#include <bsd/stdlib.h>
#include "watchdog.h"

int g_watchdog=0x796E654A; 

watchdog::watchdog() {}
watchdog::~watchdog() {}


int watchdog::alive() {
    g_watchdog++;
}

void *watchdog::monitor(void *args) {
  g_watchdog = 9000;
  
  do {
      __asm("nop");
  }while(g_watchdog--);
  
  abort();
  return(NULL);
}