#ifndef WAVESHAREDUSTSENSOR_H
#define WAVESHAREDUSTSENSOR_H

class WaveshareDustSensor
{
public:
  void begin(uint8_t ledPin, uint8_t analogPin)
  {
    _ledPin = ledPin;
    _analogPin = analogPin;
    
    pinMode(_ledPin, INPUT);
    digitalWrite(_ledPin, LOW);
  }

  float getDustConcentration()
  {
    /*
    get adcvalue
    */
    digitalWrite(_ledPin, HIGH);
    delayMicroseconds(280);
    int adcvalue = analogRead(_analogPin);
    digitalWrite(_ledPin, LOW);
    
    adcvalue = dustFilter(adcvalue);
    
    /*
    covert voltage (mv) Change 5000 to the system voltage if needed.
    */
    float voltage = (5000 / 1024.0) * adcvalue * 11;
    float density;
    
    /*
    voltage to density
    */
    if(voltage >= 400)
    {
      voltage -= 400;
      
      density = voltage * 0.2;
    }
    else 
    {
      density = 0;
    }
  
    return density;
  }

private:
  int _ledPin;
  int _analogPin;

  int dustFilter(int m)
  {
    static int flag_first = 0, _buff[10], sum;
    const int _buff_max = 10;
    int i;
    
    if(flag_first == 0)
    {
      flag_first = 1;
  
      for(i = 0, sum = 0; i < _buff_max; i++)
      {
        _buff[i] = m;
        sum += _buff[i];
      }
      return m;
    }
    else
    {
      sum -= _buff[0];
      for(i = 0; i < (_buff_max - 1); i++)
      {
        _buff[i] = _buff[i + 1];
      }
      _buff[9] = m;
      sum += _buff[9];
      
      i = sum / 10.0;
      return i;
    }
  }
  
};

#endif
