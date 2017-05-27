#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <Servo.h>

class Actuator
{
public:
  void attach(uint8_t pin)
  {
    _servo.attach(pin);  
  }
  
  void setPosition(float percentExtended)
  {
    _servo.writeMicroseconds(percentExtended * 1000 + 1000);
  }
  
private:
  Servo _servo;
};

#endif
