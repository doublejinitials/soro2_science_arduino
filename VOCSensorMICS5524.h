#ifndef VOCSENSORMICS5524_h
#define VOCSENSORMICS5524_h

class VOCSensorMICS5524
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
