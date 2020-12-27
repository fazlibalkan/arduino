#include<Servo.h>

#define servoPin 8

int canBeChosen[3] = {1, 2, 3};
int input;

Servo servo;

const byte servoP = A0;


int servoVal;
int pwm;
int inputAngle = 0, notyet = 1;


void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2); // for reducing the waiting period 
  servo.attach(servoPin);
  pinMode(servoPin, OUTPUT);
  Serial.println("");
}

void loop()
{
  input = GetInput(1);
  /*
  if (input == 1) {
    while (1) {
      inputAngle = GetInput(0);
      servo2(inputAngle);
      
    }
    
  }
  */
  servo2(-45);
  //servo1(-90);
  //ServoControlWithPOT();
}

void ServoControlWithPOT() {
  servoVal = analogRead(servoP);
  int newVal = map(servoVal , 0, 1023, 0, 180);
  servo.write(newVal);
  Serial.print("Aci Degeri = ");
  Serial.println(newVal);
}

void servo1 (int angle)
{
  //0 - 180 lerdeki değerlerini bul
  if (angle >= -90 && angle <= 90) {
    pwm = 500 + (((90 + angle)/180.0)*2000);
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(pwm); 
    digitalWrite(servoPin, LOW);
    delayMicroseconds(20000 - pwm);
  } else {
    Serial.println("You wrote an invalid angle input. Please make sure that your input is in the range [-90, 90]");
  }
}

void servo2 (int angle)
{
  servo.write(angle + 90);
}



/*
*Reads the input from the serial monitor
*Returns the input
*/
int GetInput(int first) {
  int chosenNumber = 0;
  if (Serial.available() > 0) { // send data only when you receive data:
    chosenNumber = Serial.parseInt(); // reading the input

    bool wrongInput = 1;          // checking if the user gave a correct input
    for (int i = 0; i < 3; i++) {
      if (chosenNumber == canBeChosen[i])
        wrongInput = 0;
    }
    if (wrongInput && first) {  //if Input is not one of the numbers we want
      Serial.println("Invalid input, please try again.");
      return 0;
    } else if (!first) {
      notyet = 0;
    }
  }
  return chosenNumber;
}
