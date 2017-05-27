#ifndef SERIALGEIGERCOUNTER_H
#define SERIALGEIGERCOUNTER_H


// If you wish to use a different serial port other than the default, 
// define GEIGER_SERIAL to be Serial, Serial2, or Serial3 before
// including this header file
#ifndef GEIGER_SERIAL
#define GEIGER_SERIAL Serial
#endif

class SerialGeigerCounter
{
public:
  void begin()
  {
    GEIGER_SERIAL.begin(9600);
  }
  
  int getCounts()
  {
    if (GEIGER_SERIAL.available() > 2)
    {
      _counts = GEIGER_SERIAL.parseInt();
    }
    return _counts;
  } 

private:
  uint8_t _serialNo;
  int _counts = 0;
  
};

#endif
