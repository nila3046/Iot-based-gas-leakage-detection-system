#include <LiquidCrystal.h>
#include <SoftwareSerial.h> //for SIM800L
#define ledPin 6
#define greenPin 7
#define sensorPin A0

// initialize the library by associating any needed LCD interface pin with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2

--------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  digitalWrite(ledPin, LOW);
  digitalWrite(greenPin, HIGH);

  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("LPG DETECTION");
  lcd.setCursor(0,1);
  lcd.print("Arduino Project");
  delay(3000);
  lcd.clear();
  lcd.print("Sensor Warm Up !");
  lcd.setCursor(0,1);
  for(int i=0;i<16;i++){
    lcd.print(".");
    delay(800);
  }
  delay(3000);
}

------------------------------------------------------------------------------------------------------------------------------------------------

void loop()
{

  lcd.clear();
  readSensor();
  lcd.clear();

}

------------------------------------------------------------------------------------------------------------------------------------------------

void readSensor()
{
  unsigned int sensorValue = analogRead(sensorPin);  // Read the analog value from sensor
  unsigned int outputValue = map(sensorValue, 0, 1023, 0, 255); // map the 10-bit data to 8-bit data
  lcd.print("GAS Conc. : ");
  lcd.print(outputValue);
  if (outputValue > 190)
  {
    digitalWrite(greenPin, LOW);
    analogWrite(ledPin, outputValue);
    Serial.print("SMOKE/ GAS DETECTED ! : "); // generate PWM signal
    Serial.println(outputValue);
    lcd.setCursor(0,1);
    lcd.print("LEAKAGE DETECTED");
    sendsms();
    delay(1000);
  }
  else
  {
    digitalWrite(greenPin, HIGH);
    digitalWrite(ledPin, LOW);
    delay(1000);
  }
}


------------------------------------------------------------------------------------------------------------------------------------------------

void updateSerial()
{

  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}

------------------------------------------------------------------------------------------------------------------------------------------------

void sendsms()
{
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CMGS=\"+ZZxxxxxxxxxx\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  mySerial.print("SMOKE DETECTED BY SYSTEM !"); //text content
  updateSerial();
  mySerial.write(26);
}