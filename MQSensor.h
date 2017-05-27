#ifndef MQSENSOR_H
#define MQSENSOR_H

class MQSensor
{
public:
  void begin(uint8_t analogPin)
  {
    _analogPin = analogPin;
  }

  int read()
  {
    return analogRead(_analogPin);
  }
  
private:
  uint8_t _analogPin;
};

#endif
