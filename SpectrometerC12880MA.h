#ifndef SPECTROMETERC12880MA_H
#define SPECTROMETERC12880MA_H

class SpectrometerC12880MA
{
public:
  static const int SPEC_CHANNELS = 288;
  
  void begin(uint8_t analogVideoPin, uint8_t digitalLedPin, uint8_t digitalClkPin, uint8_t digitalStPin, uint8_t digitalTrgPin, uint8_t digitalLaserPin)
  {
    _analogVideoPin = analogVideoPin;
    _digitalLedPin = digitalLedPin;
    _digitalClkPin = digitalClkPin;
    _digitalStPin = digitalStPin;
    _digitalTrgPin = digitalTrgPin;
    _digitalLaserPin = digitalLaserPin;

    //Set desired pins to OUTPUT
    pinMode(_digitalClkPin, OUTPUT);
    pinMode(_digitalStPin, OUTPUT);
    pinMode(_digitalLaserPin, OUTPUT);
    pinMode(_digitalLedPin, OUTPUT);
  
    digitalWrite(_digitalClkPin, HIGH);
    digitalWrite(_digitalStPin, LOW);
  }

  void setLedOn(bool on)
  {
    digitalWrite(_digitalLedPin, on ? HIGH : LOW);
    delayMicroseconds(500);
  }

  void setLaserOn(bool on)
  {
    digitalWrite(_digitalLaserPin, on ? HIGH : LOW);
    delayMicroseconds(500);
  }

  /*
   * This functions reads spectrometer data from SPEC_VIDEO
   * Look at the Timing Chart in the Datasheet for more info
   * 
   * arr must be an array of length SPEC_CHANNELS
   */
  void read(uint16_t* arr)
  {    
    digitalWrite(_digitalLedPin, HIGH);
    
    // Start clock cycle and set start pulse to signal start
    digitalWrite(_digitalClkPin, LOW);
    delayMicroseconds(DELAY_MICROSECONDS);
    digitalWrite(_digitalClkPin, HIGH);
    delayMicroseconds(DELAY_MICROSECONDS);
    digitalWrite(_digitalClkPin, LOW);
    digitalWrite(_digitalStPin, HIGH);
    delayMicroseconds(DELAY_MICROSECONDS);
  
    //Sample for a period of time
    for(int i = 0; i < 15; i++){
  
        digitalWrite(_digitalClkPin, HIGH);
        delayMicroseconds(DELAY_MICROSECONDS);
        digitalWrite(_digitalClkPin, LOW);
        delayMicroseconds(DELAY_MICROSECONDS); 
   
    }
  
    //Set SPEC_ST to low
    digitalWrite(_digitalStPin, LOW);
  
    //Sample for a period of time
    for(int i = 0; i < 85; i++){
  
        digitalWrite(_digitalClkPin, HIGH);
        delayMicroseconds(DELAY_MICROSECONDS);
        digitalWrite(_digitalClkPin, LOW);
        delayMicroseconds(DELAY_MICROSECONDS); 
        
    }
  
    //One more clock pulse before the actual read
    digitalWrite(_digitalClkPin, HIGH);
    delayMicroseconds(DELAY_MICROSECONDS);
    digitalWrite(_digitalClkPin, LOW);
    delayMicroseconds(DELAY_MICROSECONDS);
  
    //Read from SPEC_VIDEO
    for(int i = 0; i < SPEC_CHANNELS; i++){
  
        arr[i] = analogRead(_analogVideoPin);
        
        digitalWrite(_digitalClkPin, HIGH);
        delayMicroseconds(DELAY_MICROSECONDS);
        digitalWrite(_digitalClkPin, LOW);
        delayMicroseconds(DELAY_MICROSECONDS);
          
    }
  
    //Set SPEC_ST to high
    digitalWrite(_digitalStPin, HIGH);
  
    //Sample for a small amount of time
    for(int i = 0; i < 7; i++){
      
        digitalWrite(_digitalClkPin, HIGH);
        delayMicroseconds(DELAY_MICROSECONDS);
        digitalWrite(_digitalClkPin, LOW);
        delayMicroseconds(DELAY_MICROSECONDS);
      
    }
  
    digitalWrite(_digitalClkPin, HIGH);
    delayMicroseconds(DELAY_MICROSECONDS);

    digitalWrite(_digitalLedPin, LOW);
  }

private:
  uint8_t _analogVideoPin;
  uint8_t _digitalLedPin;
  uint8_t _digitalClkPin;
  uint8_t _digitalStPin;
  uint8_t _digitalTrgPin;
  uint8_t _digitalLaserPin;
  static const int DELAY_MICROSECONDS = 1;
};

#endif
