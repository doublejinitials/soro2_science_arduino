#ifndef GRAVITYCO2SENSOR_H
#define GRAVITYCO2SENSOR_H

class GravityCO2Sensor
{
public:
  void begin(uint8_t analogPin)
  {
    _analogPin = analogPin;
  }

  /* Returns 0 if the sensor is still preheating, or -1 on a fault. Otherwise
   *  returns the C02 concentration in PPM.
   */
  int getCO2Ppm()
  {
    int value = analogRead(_analogPin);  
  
    // The analog signal is converted to a voltage 
    float voltage = value*(5000/1024.0); 
    if(voltage == 0)
    {
      return -1;
    }
    else if(voltage < 400)
    {
      return 0;
    }
    else
    {
      int voltage_diference=voltage-400;
      int concentration=voltage_diference*50.0/16.0;
      return concentration;
    }
  }

private:
  uint8_t _analogPin;

};

#endif
