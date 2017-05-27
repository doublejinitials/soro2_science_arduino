#ifndef GROVEO2SENSOR_H
#define GROVEO2SENSOR_H

float VoutArray[] = { 0.30769, 20.00000, 40.00000, 60.00000, 120.61538, 186.76923 };
float O2ConArray[] = { 0.00018, 2.66129, 5.32258, 8.05300, 16.19851, 25.15367 };

class GroveO2Sensor
{
public:
  void begin(uint8_t analogPin)
  {
    _analogPin = analogPin;
  }

  /* Concentration 0.0 - 1.0
   */
  float readO2Concentration()
  {
    float MeasuredVout = readO2Vout();
    float Concentration = FmultiMap(MeasuredVout, VoutArray, O2ConArray, 6);
    return Concentration;
  }

private:
  uint8_t _analogPin;
  
  //This code uses MultiMap implementation from http://playground.arduino.cc/Main/MultiMap
  float FmultiMap(float val, float * _in, float * _out, uint8_t size)
  {
    // take care the value is within range
    // val = constrain(val, _in[0], _in[size‐1]);
    if (val <= _in[0]) return _out[0];
    if (val >= _in[size - 1]) return _out[size - 1];
    // search right interval
    uint8_t pos = 1; // _in[0] allready tested
    while(val > _in[pos]) pos++;
    // this will handle all exact "points" in the _in array
    if (val == _in[pos]) return _out[pos];
    
    // interpolate in the right segment for the rest
    return (val - _in[pos - 1]) * (_out[pos] - _out[pos - 1]) / (_in[pos] - _in[pos - 1]) + _out[pos - 1];
  }
  
  /* 0.0 - 3.3
   */
  inline float readO2Vout()
  {
    // Vout samples are with reference to 3.3V
    float MeasuredVout = analogRead(_analogPin) * 3.3f / 1023.0f;
    return MeasuredVout;
  }
  
};

#endif
