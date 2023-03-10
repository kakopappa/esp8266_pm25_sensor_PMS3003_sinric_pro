#include "PMS3003.h"

void PMS3003::begin() {
  Serial.printf("[PMS3003::begin] : Setup PMS3003 sensor..\r\n");
 
  #if defined PMS3003_DYNAMIC
    pms_ = new Pmsx003(PMS3003_RX, PMS3003_TX);
  #endif 
  
  pms.begin();
  pms.waitForData(Pmsx003::wakeupTime);
  pms.write(Pmsx003::cmdModeActive);
}

pm_level_t PMS3003::read() {
  const auto n = Pmsx003::Reserved;
  Pmsx003::pmsData data[n];
  Pmsx003::PmsStatus status = pms.read(data, n);
  int pm2_5 = 0;
  int pm10 = 0;
  
  switch (status) {
    case Pmsx003::OK:
    {
      pm2_5 = data[Pmsx003::PM2dot5];   
      pm10 = data[Pmsx003::PM10dot0];      
      break;
    }
    case Pmsx003::noData:
      break;
    default:
      Serial.println(Pmsx003::errorMsg[status]);
  };
 
  return {pm2_5, pm10};
}

pm_level_t PMS3003::get_pm_level() {
  int max_wait_time = 5000;
  const auto t0 = millis();

  // because sometimes get bad readings. 
  for (; (millis() - t0) < max_wait_time; delay(1000)) {
      pm_level_t level = read();
      if (level.pm25 > 0) {
        return level;
      }
  }
  
  return {0, 0};
}
 
