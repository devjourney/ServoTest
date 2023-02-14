#include <Servo.h>
#include <EEPROM.h>

#define SMOOTHING_SIZE 12

int rightServoPin = 5; // ESP8266 D1
int leftServoPin = 4; // ESP8266 D2
Servo rightServo, leftServo;
int pot = 0;
int speed = 90;
unsigned long last_check = 0;
// maintain the average of the last few readings
int pot_vals[SMOOTHING_SIZE] = {0};
int smoothing_ndx = 0;

void setup()
{
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pot = analogRead(A0);
  for (int ndx = 0; ndx < SMOOTHING_SIZE; ndx++)
    pot_vals[ndx] = pot;
  rightServo.attach(rightServoPin);
  leftServo.attach(leftServoPin);
  speed = map(pot, 0, 1023, 0, 180);
  rightServo.write(speed);
  leftServo.write(speed);
}

int avgInts(const int *intArray, const int length)
{
  double accumulator = 0;
  for (int ndx = 0; ndx < length; ndx++)
    accumulator += intArray[ndx];
  return int(round(accumulator / length));
}

void loop()
{
  unsigned long current = millis();
  if (current - last_check < 200)
    return;
  last_check = current;
  pot_vals[smoothing_ndx] = analogRead(A0);
  ++smoothing_ndx %= SMOOTHING_SIZE;
  int avg = avgInts(pot_vals, SMOOTHING_SIZE);
  if (abs(avg - pot) > 0)
  {
    pot = avg;
    speed = map(pot, 0, 1023, 0, 180);
    Serial.print(speed);
    Serial.print(" ");
    rightServo.write(speed);
    leftServo.write(speed);
  }
}