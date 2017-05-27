#include <SPI.h>
#include <Ethernet.h>
#include <string.h>
#include <EthernetUdp.h> 

#define BROADCAST_PORT  5670
#define SEND_INTERVAL   50

// Define pins here
#define PIN_PWM_ACTUATOR_TL       2
#define PIN_PWM_ACTUATOR_BL       3
#define PIN_PWM_ACTUATOR_TR       4
#define PIN_PWM_ACTUATOR_BR       5
#define PIN_DIGITAL_DUST_LED      22
#define PIN_ANALOG_SENSOR_DUST    A0
#define PIN_ANALOG_SENSOR_O2      A1
#define PIN_ANALOG_SENSOR_C02     A2
#define PIN_ANALOG_SENSOR_MQ2     A3
#define PIN_ANALOG_SENSOR_MQ4     A4
#define PIN_ANALOG_SENSOR_MQ5     A5
#define PIN_ANALOG_SENSOR_MQ6     A6
#define PIN_ANALOG_SENSOR_MQ7     A7
#define PIN_ANALOG_SENSOR_MQ9     A8
#define PIN_ANALOG_SENSOR_MQ135   A9
#define PIN_ANALOG_SENSOR_VOC     A10
#define PIN_ANALOG_SPEC_VIDEO     A11
#define PIN_DIGITAL_SPEC_LED      23
#define PIN_DIGITAL_SPEC_TRG      24
#define PIN_DIGITAL_SPEC_ST       25
#define PIN_DIGITAL_SPEC_CLK      26
#define PIN_DIGITAL_SPEC_LASER    27

#define HEADER_SET_ACTUATOR_POSITION  'a'
#define HEADER_SET_ARM_POSITION       'b'
#define HEADER_SET_SPECTROMETER_ON    'c'
#define HEADER_SET_GAS_SENSORS_ON     'd'
#define HEADER_SET_DUST_SENSOR_ON     'e'
#define HEADER_SET_GEIGER_ON          'f'
#define HEADER_DATA_MQ_SENSORS        'g'
#define HEADER_DATA_GAS_SENSORS       'h'
#define HEADER_DATA_SPECTROMETER      'i'
#define HEADER_DATA_DUST_SENSOR       'j'
#define HEADER_DATA_GEIGER            'k'

#define GEIGER_SERIAL             Serial2

#include "Serialize.h"
#include "Actuator.h"
#include "GroveO2Sensor.h"
#include "WaveshareDustSensor.h"
#include "SerialGeigerCounter.h"
#include "GravityCO2Sensor.h"
#include "MQSensor.h"
#include "SpectrometerC12880MA.h"
#include "SparkFun_MAG3110.h"
#include "Adafruit_BMP280.h"
#include "MPU6050.h"
#include "VOCSensorMICS5524.h"

byte eth_mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetUDP _eth;

Adafruit_BMP280 _bmp;

Actuator _actuatorTopLeft;
Actuator _actuatorTopRight;
Actuator _actuatorBottomLeft;
Actuator _actuatorBottomRight;

VOCSensorMICS5524 _vocSensor;

MQSensor _mq2, _mq4, _mq5, _mq6, _mq7, _mq9, _mq135;

GravityCO2Sensor _co2Sensor;

MAG3110 _mag;

GroveO2Sensor _o2Sensor;

WaveshareDustSensor _dustSensor;

SerialGeigerCounter _geiger;

SpectrometerC12880MA _spec;

uint16_t _mq2Data = 0, _mq4Data = 0, _mq5Data = 0, _mq6Data = 0, _mq7Data = 0, _mq9Data = 0, _mq135Data = 0;
uint16_t _vocData = 0;
uint32_t _o2Concentration= 0;
uint32_t _co2Ppm = 0;
uint32_t _geigerCounts = 0;
float _dustConcentration = 0;
uint16_t _ledSpecData[SpectrometerC12880MA::SPEC_CHANNELS];
uint16_t _laserSpecData[SpectrometerC12880MA::SPEC_CHANNELS];

bool _specOn = false;
bool _gasSensorsOn = false;
bool _dustSensorOn = false;
bool _geigerOn = false;

char _buffer[UDP_TX_PACKET_MAX_SIZE];
int _bufferIndex;

unsigned long _lastSendTime = 0;

void setup()
{
  // Set the default voltage of the reference voltage
  analogReference(DEFAULT); 
  
  _geiger.begin();
  
  _actuatorTopLeft.attach(PIN_PWM_ACTUATOR_TL);
  _actuatorTopRight.attach(PIN_PWM_ACTUATOR_TR);
  _actuatorBottomLeft.attach(PIN_PWM_ACTUATOR_BL);
  _actuatorBottomRight.attach(PIN_PWM_ACTUATOR_BR);

  _o2Sensor.begin(PIN_ANALOG_SENSOR_O2);
  _co2Sensor.begin(PIN_ANALOG_SENSOR_C02);
  _dustSensor.begin(PIN_DIGITAL_DUST_LED, PIN_ANALOG_SENSOR_DUST);

  _spec.begin(PIN_ANALOG_SPEC_VIDEO, PIN_DIGITAL_SPEC_LED, PIN_DIGITAL_SPEC_CLK, PIN_DIGITAL_SPEC_ST, PIN_DIGITAL_SPEC_TRG, PIN_DIGITAL_SPEC_LASER);

  _mag.initialize();
  _mag.start();

  _vocSensor.begin(PIN_ANALOG_SENSOR_VOC);

  _mq2.begin(PIN_ANALOG_SENSOR_MQ2);
  _mq4.begin(PIN_ANALOG_SENSOR_MQ4);
  _mq5.begin(PIN_ANALOG_SENSOR_MQ5);
  _mq6.begin(PIN_ANALOG_SENSOR_MQ6);
  _mq7.begin(PIN_ANALOG_SENSOR_MQ7);
  _mq9.begin(PIN_ANALOG_SENSOR_MQ9);
  _mq135.begin(PIN_ANALOG_SENSOR_MQ135);
  
  // Begin ethernet with specified MAC address
  //Ethernet.begin(eth_mac);
  //_eth.begin();
}

void loop()
{
  _bufferIndex = 0;
  
  // Check for incoming packet
  int packetSize = _eth.parsePacket();
  if (packetSize)
  {
    _eth.read(_buffer, UDP_TX_PACKET_MAX_SIZE);
    char header = _buffer[_bufferIndex++];
    if (header == HEADER_SET_ACTUATOR_POSITION && packetSize >= 9)
    {
      uint16_t topLeft = deserialize<uint16_t>(_buffer + _bufferIndex);
      _bufferIndex += 2;
      uint16_t topRight = deserialize<uint16_t>(_buffer + _bufferIndex);
      _bufferIndex += 2;
      uint16_t bottomLeft = deserialize<uint16_t>(_buffer + _bufferIndex);
      _bufferIndex += 2;
      uint16_t bottomRight = deserialize<uint16_t>(_buffer + _bufferIndex);
      _bufferIndex += 2;

      _actuatorTopLeft.setPosition(topLeft / 65535.0f);
      _actuatorTopRight.setPosition(topRight / 65535.0f);
      _actuatorBottomLeft.setPosition(bottomLeft / 65535.0f);
      _actuatorBottomRight.setPosition(bottomRight / 65535.0f);
    }
    else if (header == HEADER_SET_ARM_POSITION && packetSize >= 9)
    {
      // TODO
    }
    else if (header == HEADER_SET_SPECTROMETER_ON && packetSize >= 2)
    {
      _specOn = reinterpret_cast<const bool&>(_buffer[_bufferIndex]);
    }
    else if (header == HEADER_SET_GAS_SENSORS_ON && packetSize >= 2)
    {
      _gasSensorsOn = reinterpret_cast<const bool&>(_buffer[_bufferIndex]);
    }
    else if (header == HEADER_SET_DUST_SENSOR_ON && packetSize >= 2)
    {
      _dustSensorOn = reinterpret_cast<const bool&>(_buffer[_bufferIndex]);
    }
    else if (header == HEADER_SET_GEIGER_ON && packetSize >= 2)
    {
      _geigerOn = reinterpret_cast<const bool&>(_buffer[_bufferIndex]);
    }
  }

  // See if a data packet should be sent
  unsigned long now = millis();
  if (now < _lastSendTime || now - _lastSendTime < SEND_INTERVAL)
  {
    _lastSendTime = now;
    return;
  }

  _bufferIndex = 0;

  if (_specOn)
  {
    // Read spectrometer
    _spec.setLedOn(true);
    _spec.read(_ledSpecData);
    _spec.setLedOn(false);
    _spec.setLaserOn(true);
    _spec.read(_laserSpecData);
    _spec.setLaserOn(false);

    _buffer[_bufferIndex++] = HEADER_DATA_SPECTROMETER;
    for (int i = 0; i < SpectrometerC12880MA::SPEC_CHANNELS; ++i)
    {
      serialize<uint16_t>(_buffer + _bufferIndex, _ledSpecData[i]);
      _bufferIndex += sizeof(uint16_t);
    }
    for (int i = 0; i < SpectrometerC12880MA::SPEC_CHANNELS; ++i)
    {
      serialize<uint16_t>(_buffer + _bufferIndex, _laserSpecData[i]);
      _bufferIndex += sizeof(uint16_t);
    }
  }

  if (_dustSensorOn)
  {
    // Read dust sensor
    _dustConcentration = _dustSensor.getDustConcentration();
  }

  if (_gasSensorsOn)
  {
    // Read MQ sensors
    _mq2Data = _mq2.read();
    _mq4Data = _mq4.read();
    _mq5Data = _mq5.read();
    _mq6Data = _mq6.read();
    _mq7Data = _mq7.read();
    _mq9Data = _mq9.read();
    _mq135Data = _mq135.read();
    // Read VOC gas data
    _vocData = _vocSensor.read();
    // Read O2 concentration
    _o2Concentration = _o2Sensor.readO2Concentration() * 100;
    // Read CO2 concentration
    _co2Ppm = _co2Sensor.getCO2Ppm();

    _buffer[_bufferIndex++] = HEADER_DATA_GAS_SENSORS;
    serialize<uint16_t>(_buffer + _bufferIndex, _mq2Data);
    _bufferIndex += sizeof(uint16_t);
    serialize<uint16_t>(_buffer + _bufferIndex, _mq4Data);
    _bufferIndex += sizeof(uint16_t);
    serialize<uint16_t>(_buffer + _bufferIndex, _mq5Data);
    _bufferIndex += sizeof(uint16_t);
    serialize<uint16_t>(_buffer + _bufferIndex, _mq6Data);
    _bufferIndex += sizeof(uint16_t);
    serialize<uint16_t>(_buffer + _bufferIndex, _mq7Data);
    _bufferIndex += sizeof(uint16_t);
    serialize<uint16_t>(_buffer + _bufferIndex, _mq9Data);
    _bufferIndex += sizeof(uint16_t);
    serialize<uint16_t>(_buffer + _bufferIndex, _mq135Data);
    _bufferIndex += sizeof(uint16_t);
    serialize<uint16_t>(_buffer + _bufferIndex, _vocData);
    _bufferIndex += sizeof(uint16_t);
    serialize<uint32_t>(_buffer + _bufferIndex, _o2Concentration);
    _bufferIndex += sizeof(uint32_t);
    serialize<uint32_t>(_buffer + _bufferIndex, _co2Ppm);
    _bufferIndex += sizeof(uint32_t);
  }

  if (_geigerOn)
  {
    // Read geiger counter
    _geigerCounts = _geiger.getCounts();
    
    _buffer[_bufferIndex++] = HEADER_DATA_GEIGER;
    serialize<uint16_t>(_buffer + _bufferIndex, _geigerCounts);
    _bufferIndex += sizeof(uint16_t);
  }

  float pot = analogRead(A15) / 1023.0f;
  _actuatorTopLeft.setPosition(pot);
  _actuatorTopRight.setPosition(pot);
  Serial.println(pot);
  
  /*/*int x, y, z;
  //Only read data when it's ready
  if(_mag.dataReady()) {
    //Read the data
    _mag.readMag(&x, &y, &z);
  
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.print(z);
    Serial.println();
  }*/

  _eth.beginPacket(_eth.remoteIP(), BROADCAST_PORT);
  _eth.write(_buffer, _bufferIndex);
  _eth.endPacket();
}
