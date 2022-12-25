#pragma once
#include <pms.h> //https://github.com/riverscn/pmsx003

class PMS3003 {    
  public:
    void begin();
    int get_pm25();
    
  private:
    int get_current_pm25_level();
    Pmsx003* _pms;
};
