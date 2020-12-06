#define buttonPin 3

int input = 0;
int canBeChosen[5] = {1, 2, 3, 4, 5};
int pins[8]; //array for the led pins

volatile bool buttonState = false;


void setup() {
  //  setup for serial and some prints
  Serial.begin(115200);
  Serial.setTimeout(2); // for reducing the waiting period 
  Serial.print("Enter:\n1 for running lights\n2 for running lights with pause and resume\n3 for binary count up - on timer\n4 for binary count up - on button\n5 for binary display of numbers [1-255]\n");

  // Setup for the pins and the array
  for (int i = 0; i < 8; i++) {
    pins[i] = i + 4; //leds are connected to the pins through 4-11
    pinMode(pins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonToggled, CHANGE);

  }

  
void loop() {
  input = GetInput();
  if (input == 1) {
    RunningLights();
  } else if (input == 2){
    RunningLightsWithPauseAndResume();
  }
  
}




void RunningLights() {
  while(1) {
    for (int i = 0; i < 7; i++) {
      ledOnOff(pins[i]);
    }
    for (int i = 0; i < 7; i++) {
      ledOnOff(pins[7-i]);
    }
  }
}

void RunningLightsWithPauseAndResume() {
  while(1) {
    for (int i = 0; i < 7; i++) {
      while (buttonState){}
      ledOnOff(pins[i]);
    }
    for (int i = 0; i < 7; i++) {
      while (buttonState){}
      ledOnOff(pins[7-i]);
    }
  }  
}

void ledOnOff(int ledNum){
      digitalWrite(ledNum, HIGH);
      delay(100);
      digitalWrite(ledNum, LOW);
      delay(100);
}

void buttonToggled(){
  buttonState = !buttonState;
  Serial.print("Interrupt caught new state: ");
  Serial.println(buttonState);

}

/*
Reads the input from the serial monitor
Returns the input
*/
int GetInput() {
  int chosenNumber = 0;
  if (Serial.available() > 0) { // send data only when you receive data:
    chosenNumber = Serial.parseInt(); // reading the input

    bool wrongInput = 1; // checking if the user gave a correct input
    for (int i = 0; i < 5; i++) {
      if (chosenNumber == canBeChosen[i])
        wrongInput = 0;
    }
    if (wrongInput) { //if Input is not one of the numbers we want
      Serial.println("You wrote something that i don't want :(");
      return 0;
    }
  }
  return chosenNumber;
}
