//nocan analog relay example
//Read analog reading from channel and
//close/open relay based on that value.
//Using Omzlo CanZero Node http://www.omzlo.com
//
//Written by Scott W. Vincent - www.swvincent.com

#include <nocan.h>

const int RELAY_PIN = 2;                //Pin that relay is connected to
const int STATUS_LED = LED_BUILTIN;     //Program running status LED
NocanChannelId analogChannel;           //Nocan channel for "analogReading"
NocanChannelId relayChannel;            //Nocan channel for "relayStatus"

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
  Nocan.subscribeChannel(analogChannel);
  Nocan.registerChannel("relayStatus", &relayChannel);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(STATUS_LED, OUTPUT);

  //Relay pin, set low to start
  digitalWrite(RELAY_PIN, LOW);
}

void loop()
{  
  //Read msg
  NocanMessage msg;
  
  int status = Nocan.receiveMessage(&msg);

  if (status == 0)
  {
    //Convert Nocan message data to int.
    msg.data[msg.data_len]=0;     
    String analogReadingString = (const char *)msg.data;
    int analogReading = analogReadingString.toInt();

    //Close relay if analog reading is low (less than 500).
    //Open it once level is 800 or greater. 0 is ignored
    //since toInt() returns 0 on error.
    if (analogReading > 0 && analogReading < 500)
    {
      digitalWrite(RELAY_PIN, HIGH);
      Nocan.publishMessage(relayChannel, "255");
    }
    else if (analogReading >= 800)
    {
      digitalWrite(RELAY_PIN, LOW);
      Nocan.publishMessage(relayChannel, "0");
    }

   //Success, turn on LED
   digitalWrite(STATUS_LED, HIGH);
  }
  else
  {
    //No msg, turn off LED
    digitalWrite(STATUS_LED, LOW);
  }
}
