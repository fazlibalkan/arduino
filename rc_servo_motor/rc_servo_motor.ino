#include<Servo.h>

#define servoPin 8

int canBeChosen[4] = {1, 2, 3, 4};
int input;
String str = "";
char chr = 0;



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
  Serial.println("To control the motor please choose\n1 for controlling with an input (pwm)\n2 for controlling with an input (library)\n3 for controlling with POT\n4 for controlling with a complex pattern of the form spos:wait,pos:waite");
}

void loop()
{
  input = GetInput(1);
  if (input == 1) {
    ServoControlWithPWM();
  } else if (input == 2) {
    ServoControlWithLibrary();
  } else if (input == 3) {
    ServoControlWithPOT();
  } else if (input == 4) {
    CommunicationProtocolStarter();
  }
}

void ServoControlWithPWM() {
  Serial.println("Please enter the angle...");
  while (1){
    while (notyet) {
      inputAngle = GetInput(0);
    }
    while(1) {
      servo1(inputAngle);
    }
    notyet = 1;
  }
  Serial.println("To control the motor please choose\n1 for controlling with an input (pwm)\n2 for controlling with an input (library)\n3 for controlling with POT\n4 for controlling with a complex pattern of the form spos:wait,pos:waite");
}

void ServoControlWithLibrary() {
  Serial.println("Please enter the angle...");
  while (notyet) {
    inputAngle = GetInput(0);
  }
  while(1) {
    servo2(inputAngle);
    break;
  }
  notyet = 1; 
  Serial.println("To control the motor please choose\n1 for controlling with an input (pwm)\n2 for controlling with an input (library)\n3 for controlling with POT\n4 for controlling with a complex pattern of the form spos:wait,pos:waite");
}

/*
 *Control the servo with the potentiometer
 */
void ServoControlWithPOT() {
  Serial.println("You can adjust your POT and accordingly motor will respond.");
  while (1) {
    servoVal = analogRead(servoP);
    int newVal = map(servoVal , 0, 1023, 0, 180);
    servo.write(newVal);
    Serial.print("Angle = "); Serial.println(newVal-90);
  }
  Serial.println("To control the motor please choose\n1 for controlling with an input (pwm)\n2 for controlling with an input (library)\n3 for controlling with POT\n4 for controlling with a complex pattern of the form spos:wait,pos:waite");
}


void CommunicationProtocolStarter() {
    Serial.println("Please enter the pattern...");
    while (notyet) {
      GetInputAsString();
    }
    Serial.println(str + "e");
    CommunicationProtocol(str + "eE");
    notyet = 1;
    str = "";
    Serial.println("To control the motor please choose\n1 for controlling with an input (pwm)\n2 for controlling with an input (library)\n3 for controlling with POT\n4 for controlling with a complex pattern of the form spos:wait,pos:waite");

}

/*
 * Takes a string input of the form "s45:1000,-45:1000,0:2000e"
 * Converts it into pos:wait pairs. pos = angle, wait = delay
 * Controlls the servo motor according to the given pairs
 */
void CommunicationProtocol(String str) {
  String pos = "";
  String wait = "";
  int i = 1;
  int posorwait = -1;
  
  while (str[i] != 'E') {
    if (str[i] == ':') { 
      posorwait = 1; //After ":" start reading for the waiting period
    } else if (str[i] == ',' || str[i] == 'e') { 
      posorwait = -1; //after these characters start reading for the angle
      servo2(pos.toInt()); //do the given paired pos:wait
      delay(wait.toInt());
      pos = ""; wait = "";  //empty the strings for the other pair
    }else if (posorwait == -1) {
      pos += String(str[i]);
    }else if (posorwait == 1) {
      wait += String(str[i]);
    }
    i++;
  }
}

/*
 *Control the servo with the given value using pwm logic
 */
void servo1 (int angle)
{
  if (angle >= -90 && angle <= 90) {
    pwm = 500 + (((90 + angle)/180.0)*2000);
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(pwm); 
    digitalWrite(servoPin, LOW);
    delayMicroseconds(20000 - pwm);
  } else {
    Serial.println("You gave an invalid angle input. Please make sure that your input is in the range [-90, 90]");
  }
}


/*
 *Control the servo with the given value using servo library
 */
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

/*
 *Reads the input from the serial monitor as a string till it sees the character 'e' 
 */
void GetInputAsString() {
  if (Serial.available() > 0) {    // is a character available?
    chr = Serial.read();       // get the character
    if (chr != 'e') {
      // a character of the string was received
      str.concat(chr);
    }
    else {
      // end of string
      notyet = 0;
    }
  }
}
