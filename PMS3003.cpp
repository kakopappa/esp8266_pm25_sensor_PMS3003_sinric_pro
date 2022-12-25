#include "PMS3003.h"

void PMS3003::begin() {
  Serial.printf("[PMS3003::begin] : Setup PMS3003 sensor..\r\n");
  
  _pms = new Pmsx003(13, 12);
  _pms->begin();
  _pms->waitForData(Pmsx003::wakeupTime);
  _pms->write(Pmsx003::cmdModeActive);
}

int PMS3003::get_current_pm25_level() {
  const auto n = Pmsx003::Reserved;
  Pmsx003::pmsData data[n];
  Pmsx003::PmsStatus status = _pms->read(data, n);
  int pm2_5 = 0;
  
  switch (status) {
    case Pmsx003::OK:
    {
      pm2_5 = data[4];   
      break;
    }
    case Pmsx003::noData:
      break;
    default:
      Serial.println(Pmsx003::errorMsg[status]);
  };

  return pm2_5;
}

int PMS3003::get_pm25() {
  int max_wait_time = 5000;
  const auto t0 = millis();

  // because sometimes get bad readings. 
  for (; (millis() - t0) < max_wait_time; delay(1000)) {
      int level = get_current_pm25_level();
      if (level > 0) {
        return level;
      }
  }
  
  return 0;
}
 
