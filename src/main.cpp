
// #define BLYNK_PRINT Serial
#include "config.h"
#include <LiquidCrystal_I2C.h>
#include "pitches.h"
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <Wire.h>
// Initialize LCDs
LiquidCrystal_I2C LCD1(0x27, 16, 4);
LiquidCrystal_I2C LCD2(0x28, 16, 4);
LiquidCrystal_I2C LCD3(0x29, 16, 4);
LiquidCrystal_I2C LCD4(0x30, 16, 4);

// Define virtual pins
#define VIRTUAL_PIN_LED_1 V1
#define VIRTUAL_PIN_LED_2 V2
#define VIRTUAL_PIN_LED_3 V3
#define VIRTUAL_PIN_LED_4 V4

// I2C pins for LCDs
#define I2C_SDA 21
#define I2C_SCL 22

// Speaker pin
#define SPEAKER_PIN 25

// Ultrasonic sensor pins
#define echoPin1 15
#define trigPin1 2
#define echoPin2 18
#define trigPin2 19
#define echoPin3 13
#define trigPin3 23
#define echoPin4 32
#define trigPin4 33

// LED pins
#define LED_PIN_1 5  // yellow
#define LED_PIN_2 17 // green
#define LED_PIN_3 16 // red
#define LED_PIN_4 4  // blue

// Button pins (Bus)
#define BUTTON_BUS_1 5  // yellow
#define BUTTON_BUS_2 17 // green
#define BUTTON_BUS_3 16 // red
#define BUTTON_BUS_4 4  // blue

// Button pins (Station)
#define BUTTON_PIN_1 26 // yellow
#define BUTTON_PIN_2 27 // green
#define BUTTON_PIN_3 14 // red
#define BUTTON_PIN_4 12 // blue

// WiFi credentials
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// State variables
byte ledState1 = LOW;
byte ledState2 = LOW;
byte ledState3 = LOW;
byte ledState4 = LOW;

// State variables for debouncing
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;
unsigned long lastDebounceTime4 = 0;

const int speakerTones[] = {NOTE_G3, NOTE_C4, NOTE_E4, NOTE_G5};
unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;
unsigned long previousMillis_1 = 0; // store time for first event
const long interval_1 = 1000;       // interval for first event (ultrasonic sensor, optional)
long duration1;                     // Declare variable to store echo time duration
int distance1;                      // Declare variable to store the result (distance)
long duration2;
int distance2;
long duration3;
int distance3;
long duration4;
int distance4;
const int maxRange = 50;

// Blynk Auth Token

char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;

//****************************************************
void spinner()
{
  static int8_t counter = 0;
  const char *glyphs = "\xa1\xa5\xdb";
  LCD1.setCursor(2, 10);
  LCD1.print(glyphs[counter++]);
  if (counter == strlen(glyphs))
  {
    counter = 0;
  }
  LCD2.setCursor(2, 10);
  LCD2.print(glyphs[counter++]);
  if (counter == strlen(glyphs))
  {
    counter = 0;
  }
  LCD3.setCursor(2, 10);
  LCD3.print(glyphs[counter++]);
  if (counter == strlen(glyphs))
  {
    counter = 0;
  }
  LCD4.setCursor(2, 10);
  LCD4.print(glyphs[counter++]);
  if (counter == strlen(glyphs))
  {
    counter = 0;
  }
}
//****************************************************

void sendDataToBlynk()
{
  // Send distance data to Blynk
  Blynk.virtualWrite(VIRTUAL_PIN_LED_1, distance1);
  Blynk.virtualWrite(VIRTUAL_PIN_LED_2, distance2);
  Blynk.virtualWrite(VIRTUAL_PIN_LED_3, distance3);
  Blynk.virtualWrite(VIRTUAL_PIN_LED_4, distance4);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("sketch has started");
  delay(10);
  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("Wire begin");
  // Initialize LCDs
  LCD1.init();
  LCD1.backlight();
  LCD2.init();
  LCD2.backlight();
  LCD3.init();
  LCD3.backlight();
  LCD4.init();
  LCD4.backlight();

  // Initialize Blynk
  Serial.println("Connecting to Blynk");
  Blynk.begin(auth, ssid, pass);
  Serial.println("Blynk begin");
  while (Blynk.connected() != true)
  {
    Serial.println("Loading...");
    spinner();
    delay(200);
  }
  Serial.println("Blynk connected");

  LCD1.setCursor(0, 0);

  LCD1.clear();
  LCD1.setCursor(0, 0);
  LCD1.println("Online");
  LCD2.clear();
  LCD2.setCursor(0, 0);
  LCD2.println("Online");
  LCD3.clear();
  LCD3.setCursor(0, 0);
  LCD3.println("Online");
  LCD4.clear();
  LCD4.setCursor(0, 0);
  LCD4.println("Online");

  // Initialize pins
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);
  pinMode(BUTTON_PIN_3, INPUT_PULLUP);
  pinMode(BUTTON_PIN_4, INPUT_PULLUP);

  pinMode(BUTTON_BUS_1, INPUT_PULLUP);
  pinMode(BUTTON_BUS_2, INPUT_PULLUP);
  pinMode(BUTTON_BUS_3, INPUT_PULLUP);
  pinMode(BUTTON_BUS_4, INPUT_PULLUP);

  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);

  pinMode(SPEAKER_PIN, OUTPUT);

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT);

  // Set virtual pin modes
  Blynk.virtualWrite(VIRTUAL_PIN_LED_1, LOW); // Initialize LED state
  Blynk.virtualWrite(VIRTUAL_PIN_LED_2, LOW);
  Blynk.virtualWrite(VIRTUAL_PIN_LED_3, LOW);
  Blynk.virtualWrite(VIRTUAL_PIN_LED_4, LOW);
  // Attach Blynk functions
  // yellow
  Blynk.setProperty(VIRTUAL_PIN_LED_1, "color", "#F3FF00");
  // green
  Blynk.setProperty(VIRTUAL_PIN_LED_2, "color", "#3AFF00");
  // red
  Blynk.setProperty(VIRTUAL_PIN_LED_3, "color", "#ff0000");
  // blue
  Blynk.setProperty(VIRTUAL_PIN_LED_4, "color", "#0000FF");
  Blynk.setProperty(VIRTUAL_PIN_LED_1, "isDisabled", false);
  Blynk.setProperty(VIRTUAL_PIN_LED_2, "isDisabled", false);
  Blynk.setProperty(VIRTUAL_PIN_LED_3, "isDisabled", false);
  Blynk.setProperty(VIRTUAL_PIN_LED_4, "isDisabled", false);
  delay(300);

  timer.setInterval(1000L, sendDataToBlynk);
}

void listen_push_buttons()
{
  // Check button 1
  if (ledState1 == LOW)
  {
    if (digitalRead(BUTTON_PIN_1) == LOW)
    {
      ledState1 = HIGH;
      digitalWrite(LED_PIN_1, ledState1);
      Serial.println("Button yellow was pressed by the passengers in bus stop");
      Blynk.virtualWrite(VIRTUAL_PIN_LED_1, ledState1); // update button state
      tone(SPEAKER_PIN, speakerTones[1]);
      delay(300); // Simple debounce delay
      noTone(SPEAKER_PIN);
    }

    else if (digitalRead(BUTTON_BUS_1) == HIGH)
    {
      Serial.println(digitalRead(BUTTON_BUS_1));
      ledState1 = HIGH;
      digitalWrite(LED_PIN_1, ledState1);
      Serial.println("Button yellow was pressed by the passengers in bus");
      Blynk.virtualWrite(VIRTUAL_PIN_LED_1, ledState1); // update button state
    }
  }

  if (ledState1 == HIGH && distance1 < maxRange)
  {
    ledState1 = LOW;
    digitalWrite(LED_PIN_1, ledState1);
    LCD1.clear();
    LCD1.setCursor(0, 1);
    LCD1.print("The bus is next to the bus stop");
    Blynk.virtualWrite(VIRTUAL_PIN_LED_1, ledState1); // update button state
    Blynk.setProperty(VIRTUAL_PIN_LED_1, "isdDisabled", true);
  }

  //****************************************************
  // Check button 2
  if (ledState2 == LOW)
  {
    if (digitalRead(BUTTON_PIN_2) == LOW)
    {
      Serial.println(digitalRead(BUTTON_PIN_2));
      ledState2 = HIGH;
      digitalWrite(LED_PIN_2, ledState2);
      Serial.println("Button green was pressed by the passengers in bus stop");
      Blynk.virtualWrite(VIRTUAL_PIN_LED_2, ledState2); // update button state
      tone(SPEAKER_PIN, speakerTones[2]);
      delay(300); // Simple debounce delay
      noTone(SPEAKER_PIN);
    }

    else if (digitalRead(BUTTON_BUS_2) == HIGH)
    {
      Serial.println(digitalRead(BUTTON_BUS_2));
      ledState2 = HIGH;
      digitalWrite(LED_PIN_2, ledState2);
      Serial.println("Button green was pressed by the passengers in bus");
      Blynk.virtualWrite(VIRTUAL_PIN_LED_2, ledState2); // update button state
    }
  }

  if (ledState2 == HIGH && distance2 < maxRange)
  {
    ledState2 = LOW;
    digitalWrite(LED_PIN_2, ledState2);
    LCD2.clear();
    LCD2.setCursor(0, 1);
    LCD2.print("The bus is next to the bus stop");
    Blynk.virtualWrite(VIRTUAL_PIN_LED_2, ledState2); // update button state
    Blynk.setProperty(VIRTUAL_PIN_LED_2, "isdDisabled", true);
    delay(300);
  }

  //****************************************************
  // Check button 3
  if (ledState3 == LOW)
  {
    if (digitalRead(BUTTON_PIN_3) == LOW)
    {
      Serial.println(digitalRead(BUTTON_PIN_3));
      ledState3 = HIGH;
      digitalWrite(LED_PIN_3, ledState3);
      Serial.println("Button red was pressed by the passengers in bus stop");
      Blynk.virtualWrite(VIRTUAL_PIN_LED_3, ledState3); // update button state
      tone(SPEAKER_PIN, speakerTones[3]);
      delay(300);
      noTone(SPEAKER_PIN);
    }

    else if (digitalRead(BUTTON_BUS_3) == HIGH)
    {
      Serial.println(digitalRead(BUTTON_BUS_3));
      ledState3 = HIGH;
      digitalWrite(LED_PIN_3, ledState3);
      Serial.println("Button red was pressed by the passengers in bus");
      Blynk.virtualWrite(VIRTUAL_PIN_LED_3, ledState3); // update button state
    }
  }

  if (ledState3 == HIGH && distance3 < maxRange)
  {
    ledState3 = LOW;
    digitalWrite(LED_PIN_3, ledState3);
    LCD3.clear();
    LCD3.setCursor(0, 1);
    LCD3.print("The bus is next to the bus stop");
    Blynk.virtualWrite(VIRTUAL_PIN_LED_3, ledState3); // update button state
    Blynk.setProperty(VIRTUAL_PIN_LED_3, "isdDisabled", true);
    delay(300); // Simple debounce delay
  }

  //****************************************************
  // Check button 4
  if (ledState4 == LOW)
  {
    if (digitalRead(BUTTON_PIN_4) == LOW)
    {
      Serial.println(digitalRead(BUTTON_PIN_4));
      ledState4 = HIGH;
      digitalWrite(LED_PIN_4, ledState4);
      Serial.println("Button blue was pressed by the passengers in bus stop");
      Blynk.virtualWrite(VIRTUAL_PIN_LED_4, ledState4); // update button state
      tone(SPEAKER_PIN, speakerTones[4]);
      delay(300);
      noTone(SPEAKER_PIN);
    }

    else if (digitalRead(BUTTON_BUS_4) == HIGH)
    {
      Serial.println(digitalRead(BUTTON_BUS_4));
      ledState4 = HIGH;
      digitalWrite(LED_PIN_4, ledState4);
      Serial.println("Button blue was pressed by the passengers in bus");
      Blynk.virtualWrite(VIRTUAL_PIN_LED_4, ledState4); // update button state
    }
  }

  if (ledState4 == HIGH && distance4 < maxRange)
  {
    ledState4 = LOW;
    digitalWrite(LED_PIN_4, ledState4);
    LCD4.clear();
    LCD4.setCursor(0, 1);
    LCD4.print("The bus is next to the bus stop");
    Blynk.virtualWrite(VIRTUAL_PIN_LED_4, ledState4); // update button state
    Blynk.setProperty(VIRTUAL_PIN_LED_3, "isdDisabled", true);
    delay(300); // Simple debounce delay
  }
}

void display_distance()
{
  if (distance1 >= maxRange)
  {
    LCD1.clear();
    LCD1.setCursor(0, 1);
    LCD1.print("Out of Range");
  }
  else
  {
    LCD1.clear();
    LCD1.setCursor(0, 0);
    LCD1.print("Distance:");
    LCD1.setCursor(0, 1);
    LCD1.print(distance1);
    LCD1.setCursor(4, 1);
    LCD1.print("m");
  }
  if (distance2 >= maxRange)
  {
    LCD2.clear();
    LCD2.setCursor(0, 1);
    LCD2.print("Out of Range");
  }
  else
  {
    LCD2.clear();
    LCD2.setCursor(0, 0);
    LCD2.print("Distance:");
    LCD2.setCursor(0, 1);
    LCD2.print(distance2);
    LCD2.setCursor(4, 1);
    LCD2.print("m");
  }
  if (distance3 >= maxRange)
  {
    LCD3.clear();
    LCD3.setCursor(0, 1);
    LCD3.print("Out of Range");
  }
  else
  {
    LCD3.clear();
    LCD3.setCursor(0, 0);
    LCD3.print("Distance:");
    LCD3.setCursor(0, 1);
    LCD3.print(distance3);
    LCD3.setCursor(4, 1);
    LCD3.print("m");
  }
  if (distance4 >= maxRange)
  {
    LCD4.clear();
    LCD4.setCursor(0, 1);
    LCD4.print("Out of Range");
  }
  else
  {
    LCD4.clear();
    LCD4.setCursor(0, 0);
    LCD4.print("Distance:");
    LCD4.setCursor(0, 1);
    LCD4.print(distance4);
    LCD4.setCursor(4, 1);
    LCD4.print("m");
  }
}

void measure_distance()
{
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration1 = pulseIn(echoPin1, HIGH);
  distance1 = duration1 * 0.0344 / 2;

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  duration2 = pulseIn(echoPin2, HIGH);
  distance2 = duration2 * 0.0344 / 2;

  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  duration3 = pulseIn(echoPin3, HIGH);
  distance3 = duration3 * 0.0344 / 2;

  digitalWrite(trigPin4, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin4, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin4, LOW);
  duration4 = pulseIn(echoPin4, HIGH);
  distance4 = duration4 * 0.0344 / 2;

  Blynk.virtualWrite(VIRTUAL_PIN_LED_1, distance1);
  Blynk.virtualWrite(VIRTUAL_PIN_LED_2, distance2);
  Blynk.virtualWrite(VIRTUAL_PIN_LED_3, distance3);
  Blynk.virtualWrite(VIRTUAL_PIN_LED_4, distance4);
}

void loop()
{
  Blynk.run();
  timer.run();
  //****************************************************
  listen_push_buttons();
  //****************************************************
  measure_distance();
  display_distance();
  //****************************************************
}

// BLYNK_WRITE functions
BLYNK_WRITE(VIRTUAL_PIN_LED_1)
{
  int ledStateV1 = param.asInt();
  digitalWrite(LED_PIN_1, ledStateV1);
}

BLYNK_WRITE(VIRTUAL_PIN_LED_2)
{
  int ledStateV2 = param.asInt();
  digitalWrite(LED_PIN_2, ledStateV2);
}

BLYNK_WRITE(VIRTUAL_PIN_LED_3)
{
  int ledStateV3 = param.asInt();
  digitalWrite(LED_PIN_3, ledStateV3);
}

BLYNK_WRITE(VIRTUAL_PIN_LED_4)
{
  int ledStateV4 = param.asInt();
  digitalWrite(LED_PIN_4, ledStateV4);
}
