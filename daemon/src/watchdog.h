#ifndef WATCHDOG_H
#define WATCHDOG_H

class watchdog {
protected:
    watchdog();
    ~watchdog();

public:
    static int alive();
    static void *monitor(void *args);
  
};

#endif