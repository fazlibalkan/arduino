#include<Servo.h>

#define servoPin 8

int canBeChosen[4] = {1, 2, 3, 4};
int input;
char rx_byte = 0;
String rx_str = "";


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
  if (input == 1) {
    while (notyet) {
      Serial.println("a");
      inputAngle = GetInput(0);
    }
    while(1) {
      Serial.println("b");
      servo1(inputAngle);
    }
    notyet = 1;
  } else if (input == 2) {
    while (notyet) {
      Serial.println(inputAngle);
      inputAngle = GetInput(0);
    }
    while(1) {
      Serial.println(inputAngle);
      servo2(inputAngle);
    }
    notyet = 1;
  } else if (input == 3) {
    while (1) {
      ServoControlWithPOT();
    }
  } else if (input == 4) {
    //CommunicationProtocol();
    while (1) {
      GetInputAsString();
    }
    Serial.println(rx_str);
  }
  
  
  
}

void CommunicationProtocol(String str) {
  String pos = "";
  String wait = "";
  int i = 1;
  int posorwait = -1;
  
  while (1) {
    if (str[i] == ':') {
      posorwait = 1;
    }
    if (str[i] == ',') {
      posorwait = -1;
      servo2(int(pos));
      delay(int(wait));
    }
    if (posorwait == -1) {
      pos += str[i];
    }
    if (posorwait == 1) {
      wait += str[i];
    }
    i++;
  }
}


/*
 *Control the servo with the potentiometer
 */
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
  if (angle >= -90 && angle <= 90) {
    servo.write(angle + 90);
  } else {
    Serial.println("You wrote an invalid angle input. Please make sure that your input is in the range [-90, 90]");
  }
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
    for (int i = 0; i < 4; i++) {
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

void GetInputAsString() {
  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();       // get the character
    Serial.print("Welcome ");
    if (rx_byte != '\n') {
      // a character of the string was received
      rx_str = rx_str + rx_byte;
    }
    else {
      // end of string
      Serial.println("Welcome ");
      Serial.println(rx_str);
      //rx_str = "";                // clear the string for reuse
      Serial.println("");
      Serial.println("Enter your name.");
    }
  }
}
