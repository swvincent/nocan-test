//nocan analogRead example
//Read value from analog input and publish on a channel
//Using Omzlo CanZero Node and NoCan system - http://www.omzlo.com
//
//Written by Scott W. Vincent - www.swvincent.com

#include <nocan.h>

const int SENSOR_PIN = 0;               //Analog pin that sensor is connected to
const long SENSOR_INTERVAL = 1000;      //Time in ms between sensor readings
const long STATUS_LED_TIME = 10;        //Time in ms to leave LED on indicating sensor reading

int sensorValue = 0;                    //Last value read from sensor
unsigned long sensorPrevMillis = 0;     //Last time reading was taken from sensor

NocanChannelId analogChannel;           //Nocan channel for "analogReading"

void setup()
{
  for (;;)
  {
    if (Nocan.open() < 0)
      delay(1000);
    else
      break;  
  }
  
  Nocan.registerChannel("analogReading", &analogChannel);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  unsigned long currentMillis = millis();

  //Read from sensor
  if (currentMillis - sensorPrevMillis >= SENSOR_INTERVAL)
  {
    //Take new reading and publish.
    sensorPrevMillis = currentMillis;
    int sensorValue = analogRead(SENSOR_PIN);
    
    if (publishInt(analogChannel, sensorValue) == 0)
    {
      //Success, turn on LED
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
  else
  {
    //Check if LED should be turned off.
    if (currentMillis - STATUS_LED_TIME >= sensorPrevMillis)
    {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}


int publishInt(NocanChannelId cid, int i)
{
  String f_s = String(i);
  return Nocan.publishMessage(cid, f_s.c_str());
}
