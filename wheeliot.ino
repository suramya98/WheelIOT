#include <TimerOne.h>
#include <SoftwareSerial.h>
#include <math.h>
double Thermistor(int RawADC)
{
  double Temp;
  Temp = log(10000.0 / (1024.0 / RawADC - 1)); //log(10000.0*((1024.0/RawADC-1)));
  //          // for pull-up configuration
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
  Temp = (Temp * 9.0) / 5.0 + 32; // Convert Celcius to Fahrenheit
  return Temp;
}
int input = A0;
int ledPin = 13;
int Bit = 0;
int count = 0;
int rate = 0;
boolean ledOn = false;
bool flag  = 0;
SoftwareSerial gsmSerial(11, 12);
bool newData = false, sndAlt[4] = {false, false, false, false}, upd = false;
unsigned long previousMillis = 0, pulse;
int tempC;
String number = "9036577382";
void setup()
{
  pinMode(input, INPUT);
  Serial.begin(9600);
  gsmSerial.begin(9600);
  delay(3000);
  gsmSerial.println("AT+CMGF=1");//text mode
  delay(300);
  gsmSerial.println("AT+CNMI=2,2,0,0,0");
  delay(300);
  pinMode(A4, INPUT_PULLUP);
  pinMode(input, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  Timer1.initialize(500000);
  Timer1.attachInterrupt(callback);
}
void loop()
{
  int x = analogRead(A2);
  int y = analogRead(A3);
  tempC = int(Thermistor(analogRead(A1)));
  if (y < 100)robMove("F");
  else if (y > 900)robMove("B");
  else if (x < 100)robMove("L");
  else if (x > 900)robMove("R");
  else {
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
  }
  while (gsmSerial.available())
  {
    String sms = gsmSerial.readStringUntil('\r');
    Serial.println(sms);
    sms.trim();
    sms.toLowerCase();
    if (sms.startsWith("+cmt:"))
    {
      sms = gsmSerial.readStringUntil('\r');
      sms.trim();
      sms.toLowerCase();
      if (sms.startsWith("track"))
      {
        sendSms(number, "Temperature:" + String(tempC) + "\n\rHeart-Beat:" + String(pulse));
      }
    }
  }
  if (tempC > 40)
  {
    if (sndAlt[0] == false)
    {
      sndAlt[0] = true;
      sendSms(number, "High temperature\n\rTemperature:" + String(tempC) + "\n\rHeart-Beat:" + String(pulse));
      Serial.println("htemp");
    }
  } else sndAlt[0] = false;
  if (tempC < 10)
  {
    if (sndAlt[3] == false)
    {
      sndAlt[3] = true;
      sendSms(number, "LOW temperature\n\rTemperature:" + String(tempC) + "\n\rHeart-Beat:" + String(pulse));
      Serial.println("ltemp");
    }
  } else sndAlt[3] = false;
  if (pulse > 100 || pulse < 20)
  {
    if (sndAlt[1] == false)
    {
      sndAlt[1] = true;
      sendSms(number, "Pulse abnormal\n\rTemperature:" + String(tempC) + "\n\rHeart-Beat:" + String(pulse));
      Serial.println("pulse");
    }
  } else sndAlt[1] = false;

  if (digitalRead(A4) == LOW)
  {
    if (sndAlt[2] == false)
    {
      sndAlt[2] = true;
      sendSms(number, "EMERGENCY\n\rTemperature:" + String(tempC) + "\n\rHeart-Beat:" + String(pulse));
      Serial.println("panic");
    }
  } else sndAlt[2] = false;

  if (digitalRead(input) == 0)
  {
    flag = 1;
    digitalWrite(ledPin, HIGH);
    delay(750);
    Bit = Bit + 1;
  }
  else {
    digitalWrite(ledPin, LOW);
  }
  rate = Bit * 7.5;
}
void sendSms(String num, String msg)
{
  gsmSerial.println("AT+CMGS=\"" + num + "\"");
  delay(150);
  gsmSerial.println(msg);
  delay(150);
  gsmSerial.println(char(26));
  delay(3000);
}
void callback()
{
  count = count + 1;
  if (count == 16) //after (16/2)=8sec,reset all
  {
    Serial.println(String(pulse) + "," + String(tempC));
    pulse = rate;
    count = 0;
    Bit = 0;
  }
}
void robMove(String Str)
{
  if (Str == "B")
  {
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
  }
  else if (Str == "F")
  {
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
  }
  else if (Str == "L")
  {
    digitalWrite(4, HIGH);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    delay(100);
  }
  else if (Str == "R")
  {
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);
    delay(100);
  }
  delay(100);
}
