
/**
 * Hot wheels race timer by Cory Roy - May 2020
 * 
 * First car to break IR beam in winner
 * 
 * Includes countdown timer
 * Record scoreboard
 * Lane LEDs
 * 
 * Uses Gikfun 5mm 940nm IR LED's emitters and receivers: EK8443
 * Uses OLED display : 
 *    0.91 inch OLED with White/Blue color,128x32 resolution. 
 *    The driver IC is SSD1306 with I2C interface. 
 * 
 * - inspired and bootstrapped by by Mike Freda - March 2020
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

boolean debug = false;

int analogOutPin1 = A1; // finish line beam lane 1
int analogOutPin2 = A2; // finish line beam lane 2

const int ledPin1 = 12; //lane 1 winning LED
const int ledPin2 = 11; //Lane 2 winning LED

int sensorValueFinish1 = 0;
int sensorValueFinish2 = 0;

int sensorThresh = 300; //Sets the trigger sensing threshold of the IR receivers. ~1000 = high

boolean racing = false;

unsigned long startTime = 0;
unsigned long finishTime = 0;
unsigned long elapsedTime = 0;
unsigned long recordTime = 11000;

Adafruit_SSD1306 display(5);

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(9600);
  pinMode(ledPin1, OUTPUT); //lane 1
  pinMode(ledPin2, OUTPUT); //lane 2
  digitalWrite(ledPin1, HIGH);
  digitalWrite(ledPin2, HIGH);
  delay(2000);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  letsRace();
}

void resetDisplay() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
}

void letsRace() {
  racing = false;
  resetDisplay();
  display.setCursor(0, 0);
  display.println("Let's Race!");
  display.display();
  startCountdown();
}

void startCountdown() {
  delay(2000);
  display.clearDisplay();
  display.println("Race starting in ...");
  display.display();

  for (int i = 5; i >= 1; i--) {
    alternateWinnerLEDs(i);
    display.print(i);
    if (i > 1) {
      display.print(", ");
    }
    display.display();
    delay(1000);
  }
  startRace();
}

void startRace() {
  racing = true;
  for (int i = 1; i < 50; i++) {
    alternateWinnerLEDs(i);
    delay(50);
  }
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  display.clearDisplay();
  display.println("Go!!!!!");
  startTime = millis();
  display.display();
}

void alternateWinnerLEDs(int i) {
  if (i % 2) {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, LOW);
  } else {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, HIGH);
  }
}

void loop()
{
  //reads the analog in value:
  sensorValueFinish1 = analogRead(analogOutPin1);
  sensorValueFinish2 = analogRead(analogOutPin2);

  finishTime = millis();
  // You may have to adjust sensorThresh value for better accuracy.
  // The sensors should read around 900-1000 then go "low" (less than 400) when a car passes through the beam.

  if (debug) {
        Serial.print("Sensor Finish1 = " );
        Serial.println(sensorValueFinish1);
        Serial.print("Sensor Finish2 = " );
        Serial.println(sensorValueFinish2);
    delay(50);
  }

  // wait/check for the finish sensors to be triggered

  finishTime = millis();
  elapsedTime = finishTime - startTime;

  if (debug) {
    Serial.print("Elapsed Time: ");
    Serial.println(elapsedTime);
  }


  if (elapsedTime > 10000) {
    letsRace();
  } else if (sensorValueFinish1 < sensorThresh && digitalRead(ledPin2) == LOW) //Lane 1 is winner
  {
    digitalWrite(ledPin1, HIGH); //turn on lane 1 winner LED
    resetDisplay();
    display.println("Lane 1 wins!");
    printResultTime();
    digitalWrite(ledPin1, LOW); //turn off lane 1 winner LED
    letsRace();
  } else if (sensorValueFinish2 < sensorThresh && digitalRead(ledPin1) == LOW) //Lane 2 is winner
  {
    digitalWrite(ledPin2, HIGH); //turn on lane 2 winner LED
    resetDisplay();
    display.println("Lane 2 wins!");
    printResultTime();
    digitalWrite(ledPin2, LOW); //turn off lane 2 winner LED
    letsRace();
  }
}

void printResultTime() {
  racing = false;
  display.print((elapsedTime / 1000));
  display.print(".");
  display.print(((elapsedTime % 1000) / 10));
  display.println(" seconds");
  if (elapsedTime < recordTime) {
    display.println("");
    display.println(" -- New Record! --");
    recordTime = elapsedTime;
  }
  display.display();
  delay(6000);
}
