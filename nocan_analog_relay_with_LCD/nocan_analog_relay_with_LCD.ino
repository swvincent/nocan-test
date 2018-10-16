//nocan analog relay example (with LCD display)
//Read analog reading from channel and
//close/open relay based on that value.
//Also display values on LCD for debugging.
//Using Omzlo CanZero Node http://www.omzlo.com
//
//Written by Scott W. Vincent - www.swvincent.com

#include <nocan.h>
#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>

const int RELAY_PIN = 2;                //Pin that relay is connected to
NocanChannelId analogChannel;           //Nocan channel for "analogReading"
NocanChannelId relayChannel;            //Nocan channel for "relayStatus"
Adafruit_LiquidCrystal lcd(0);          //LCD, via i2c address #0 (default)

void setup()
{
  // set up LCD's # of rows, columns
  lcd.begin(16, 4);
  // Print a message to the LCD.
  // Turn on backlight and show initial msg
  lcd.setBacklight(HIGH);
  lcd.print("Connecting...");
  
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

  //Relay pin, set low to start
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  //Status LED
  pinMode(LED_BUILTIN, OUTPUT);

  //Setup LCD for loop
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("*OMZLO CANZERO DEMO*");
  lcd.setCursor(2, 1);
  lcd.print("STR:");
  lcd.setCursor(2, 2);
  lcd.print("INT:");
  lcd.setCursor(0, 3);
  lcd.print("RELAY:");
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
    lcd.setCursor(7, 1);
    lcd.print("             ");
    lcd.setCursor(7, 1);
    lcd.print(analogReadingString);
    
    int analogReading = analogReadingString.toInt();
    lcd.setCursor(7, 2);
    lcd.print("             ");
    lcd.setCursor(7, 2);
    lcd.print(String(analogReading));

    lcd.setCursor(7, 3);
  
    //Close relay if analog reading is low (less than 500).
    //Open it once level is 800 or greater. 0 is ignored
    //since toInt() returns 0 on error.
    if (analogReading > 0 && analogReading < 500)
    {
      digitalWrite(RELAY_PIN, HIGH);
      lcd.print("CLOSED");
      Nocan.publishMessage(relayChannel, "255");
    }
    else if (analogReading >= 800)
    {
      digitalWrite(RELAY_PIN, LOW);
      lcd.print("OPEN  ");
      Nocan.publishMessage(relayChannel, "0");
    }

   //Success, turn on LED
   digitalWrite(LED_BUILTIN, HIGH);

   //Update LCD
   
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}
