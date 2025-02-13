#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <ArduinoJson.h>

const int FireSensor = A0;
const int buzzer = 21;
const int MotionSensor = A1;
const int led = 18;
const int temperatureSensor = A2;
const int rs = 7, e = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, e, d4, d5, d6, d7);
const int led_g = 19;
const int led_r = 20;
const int relayTemp = 8;
const int relayPass = 9;
const int relayFlame = 10;
const int relayPir = 11;

char enteredPassword[4];
char password[5] = {'1', '5', '9', '7'};
int i = 0;
char key_pressed = 0;

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
byte rowPins[ROWS] = {A8, A9, A10, A11};
byte colPins[COLS] = {A5, A6, A7};
Keypad Keys = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int lastFireState = LOW;
int lastMotionState = LOW;
int highTemperatureFlag = 1;
bool isPasswordCorrect = false;

void setup()
{
  Serial.begin(9600);
  pinMode(FireSensor, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(MotionSensor, INPUT);
  pinMode(led, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_r, OUTPUT);
  pinMode(relayTemp, OUTPUT);
  pinMode(relayPass, OUTPUT);
  pinMode(relayFlame, OUTPUT);
  pinMode(relayPir, OUTPUT);
  lcd.begin(16, 2);
}

void resetPassword()
{
  memset(enteredPassword, 0, sizeof(enteredPassword));
  i = 0;
}

void loop()
{
  // FIRE SENSOR
  int currentFireState = digitalRead(FireSensor);
  if (currentFireState != lastFireState)
  {
    lastFireState = currentFireState;
    if (currentFireState == HIGH)
    {
      digitalWrite(buzzer, HIGH);
      digitalWrite(relayFlame, HIGH);
      Serial.println("FIRE DETECTED!");
    }
    else
    {
      digitalWrite(buzzer, LOW);
      digitalWrite(relayFlame, LOW);
    }
  }

  // MOTION SENSOR
  int currentMotionState = digitalRead(MotionSensor);
  if (isPasswordCorrect == false && currentMotionState != lastMotionState)
  {
    lastMotionState = currentMotionState;
    if (currentMotionState == HIGH)
    {
      digitalWrite(led, HIGH);
      digitalWrite(relayPir, HIGH);
      Serial.println("MOTION DETECTED!");
    }
    else
    {
      digitalWrite(led, LOW);
      digitalWrite(relayPir, LOW);
    }
  }

  // TEMPERATURE CALCULATION
  int temperature;
  float voltage;
  voltage = analogRead(temperatureSensor);
  voltage = (voltage / 1023) * 5000;
  voltage = voltage / 10.0;
  temperature = voltage + 0.3;
  lcd.setCursor(0, 0);
  lcd.print("TEMPERATURE: ");
  lcd.setCursor(12, 0);
  lcd.print(temperature);
  lcd.setCursor(14, 0);
  lcd.print("C");
  if (temperature < 20)
  {
    if (highTemperatureFlag != 1)
    {
    lcd.setCursor(0, 1);
    lcd.print("LOW TEMPERATURE");
    digitalWrite(relayTemp, LOW);
    Serial.println("LOW TEMPERATURE!");
    highTemperatureFlag = 1;
    }
  }
  else if (temperature > 30)
  { if (highTemperatureFlag != 2)
    {
    lcd.setCursor(0, 1);
    lcd.print("HIGH TEMPERATURE");
    digitalWrite(relayTemp, HIGH);
    Serial.println("HIGH TEMPERATURE!");
    highTemperatureFlag = 2; 
    }
  }
  else
  {
    lcd.setCursor(0, 1);
    lcd.print("NORM TEMPERATURE");
    digitalWrite(relayTemp, LOW);
    highTemperatureFlag = 3;
  }

  // PASSWORD
  key_pressed = Keys.getKey();
  if (key_pressed)
  {
    if (key_pressed == '#')
    {
      resetPassword();
      digitalWrite(relayPass, LOW);
      isPasswordCorrect = false;
    }
    else
    {
      enteredPassword[i++] = key_pressed;
      if (i == 4)
      {
        if (!(strncmp(enteredPassword, password, 4)))
        {
          digitalWrite(led_g, HIGH);
          digitalWrite(relayPass, HIGH);
          Serial.println("PASSWORD CORRECT!");
          isPasswordCorrect = true;
          delay(400);
          digitalWrite(led_g, LOW);
          resetPassword();
        }
        else
        {
          digitalWrite(led_r, HIGH);
          digitalWrite(relayPass, LOW);
          Serial.println("PASSWORD INCORRECT!");
          isPasswordCorrect = false;
          delay(400);
          digitalWrite(led_r, LOW);
          resetPassword();
        }
      }
    }
  }
}
