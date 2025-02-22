/* High_Teperature_Thermostat en Hanna P 2021 - 2025 en
For Arduino Nano or other boards */
/********************************************/

/*-----( Import needed libraries )-----*/
#include <Wire.h>  // Comes with Arduino IDE

// Get the LCD I2C Library here:
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.
#include <LiquidCrystal_I2C.h>

/*************************************************** 
  This is a library for the Adafruit PT100/P1000 RTD Sensor w/MAX31865
  Designed specifically to work with the Adafruit RTD Sensor
  ----> https://www.adafruit.com/products/3328
  This sensor uses SPI to communicate, 4 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution */
#include <Adafruit_MAX31865.h>

/*-----( Declare Constants )-----*/
const int relayPin = 13;   // Pin for relay

/*------( Set levels )------*/
const int hys_set_value = 5; // Setting up hysteris value
const float temperature_set_value = 55.00; // SW value

/*-----( Declare objects )-----*/
// set the LCD address to 0x20 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
// Set the LCD I2C address 0x20, 0x3f or something else,
// depending on the I2C circuit
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 temperature_sensor = Adafruit_MAX31865(8, 9, 10, 11);

// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 max = Adafruit_MAX31865(10);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF_temperature_sensor  427.0

// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

/*-----( Declare Variables )-----*/
int temperature_hys = 0;
float temperature_value;
String state_message = "";


void setup() 
{
  Serial.begin(9600);  // initialize the hardware UART for speed 9600
  test_IIC(); //Debug I2C
  digitalWrite(relayPin, LOW); // Set relay pin to Low
  lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines
  lcd.backlight(); // set backlight on 
  temperature_sensor.begin(MAX31865_2WIRE);  // initialize Max31865 and Set to 2WIRE or 4WIRE as necessary
  lcd.setCursor(0,0); //Start at character 0 on line 1
  lcd.print("Booting up system...");
  delay(2000);
  lcd.setCursor(0,1); // Move to character 0 on line 2
  lcd.print("High temp thermostat");
  lcd.setCursor(0,2); // Move to character 0 on line 3
  lcd.print("Version.001");
  Serial.print("SW Temperature = "); Serial.println(temperature_set_value + temperature_hys);
  delay(5000);
  Serial.println("Setup done..."); //Debug
}


void loop() 
{
  temperature_value = temperature_sensor.temperature(RNOMINAL, RREF_temperature_sensor); //Reading temperature
  temperature_comp();
  display_lcd();
  Serial.print("Temperature = "); Serial.println(temperature_value);
  delay(1000);
}


void temperature_comp()
{
  if (temperature_value < (temperature_set_value + temperature_hys))
  {
    temperature_hys = hys_set_value;
    digitalWrite(relayPin, HIGH); // Set relay pin to High
    state_message = "On";
    Serial.println("Relay on"); //Debug
  }
  else
  {
    temperature_hys = 0;
    digitalWrite(relayPin, LOW); // Set relay pin to Low
    state_message = "Off";
    Serial.println("Relay off"); //Debug
  }
}


void display_lcd()
{
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 0 on line 1
  lcd.print("Temp = "); lcd.print(temperature_value);
  lcd.setCursor(0,1); //Start at character 0 on line 2
  lcd.print("SW Temp = "); lcd.print(temperature_set_value + temperature_hys);
  lcd.setCursor(0,2); //Start at character 0 on line 3
  lcd.print("SW state = "); lcd.print(state_message);
}


void test_IIC() //Debug I2C
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;
 
  Wire.begin();
  for (byte i = 1; i < 120; i++)
  {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);  // maybe unneeded?
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");
}
