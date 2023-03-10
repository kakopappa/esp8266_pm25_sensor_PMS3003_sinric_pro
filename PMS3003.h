#pragma once
#include <pms.h> //https://github.com/riverscn/pmsx003
#define PMS3003_DYNAMIC

typedef struct {
  int pm25;
  int pm10;
} pm_level_t;

#define PMS3003_RX 13
#define PMS3003_TX 12

class PMS3003 {    
  public:
    void begin();
    pm_level_t get_pm_level();
    
  private:
    pm_level_t read();
    
    #if defined PMS3003_DYNAMIC
      Pmsx003 *pms_ = nullptr;
      #define pms (*pms_)
    #else
      Pmsx003 pms(PMS3003_RX, PMS3003_TX);  //rx, tx
    #endif

};
