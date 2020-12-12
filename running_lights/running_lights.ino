#define buttonPin 3

int input = 0;
int canBeChosen[5] = {1, 2, 3, 4, 5};
int pins[8]; //array for the led pins

volatile bool buttonState = LOW;
volatile bool lastButtonState = HIGH;


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
  pinMode(buttonPin, INPUT_PULLUP);

  //attachInterrupt(digitalPinToInterrupt(buttonPin), debounceButton, CHANGE); //Interrupt for the button
  
  }

  
void loop() {
  input = GetInput(1);
  if (input == 1) {
    RunningLights();
  } else if (input == 2){
    RunningLightsWithPauseAndResume();
  } else if (input == 3){
    BinaryCountUpOnTimer();
  } else if (input == 4){
    BinaryCountUpOnButton();
  } else if (input == 5){
    BinaryDisplayOfNumbers();
  }
  
}



/*
 * 1st option
 * Runs the lights
*/
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
/*
 * 2nd option
 * Runs the lights
 * When button is pressed, running stops;
 * When released, continues
*/
void RunningLightsWithPauseAndResume() {
  while(1) {
    for (int i = 0; i < 7; i++) {
      while (debounceButton(buttonState)){}
      ledOnOff(pins[i]);
    }
    for (int i = 0; i < 7; i++) {
      while (debounceButton(buttonState)){}
      ledOnOff(pins[7-i]);
    }
  }  
}

/*
 * 3rd option
 * Counts the numbers 0-255 and displays them as binary with leds
*/
void BinaryCountUpOnTimer(){
  while (1) {
    
    for (int i = 0; i < 256; i++) {
      PrintCurrentCount(i);
      DisplayNumber(i);
      delay(200);
      OffLeds();
    }
    
  }
  
}

/*
 * 4th option
 * Counts the numbers when pressed button 0-255 and displays them as binary with leds
*/

void BinaryCountUpOnButton(){
  while (1) {
    for (int i = 0; i < 256; i++) {
      while (lastButtonState == LOW || debounceButton(buttonPin)) {};
      while (lastButtonState == HIGH && !debounceButton(buttonPin)) {}
      OffLeds();
      PrintCurrentCount(i);
      DisplayNumber(i);
    }
  }
  
}

/*
 * Takes the number as input from the user and displays it in binary
*/

void BinaryDisplayOfNumbers() {
  int oldinput = 0;
  while (1) {
    while (!(input = GetInput(0))) {}
    OffLeds();
    DisplayNumber(input); 
    PrintCurrentCount(input);
  }
}

/*
 * Prints the number that is currently displaying
*/
void PrintCurrentCount(int current) {
  Serial.print("Current count is ");
  Serial.println(current);
}

/*
 * Displays the given number in binary using leds
*/
void DisplayNumber(int num) {
  int bitChecker = 1;
  for (int j = 0; j < 8; j++) {
    if ((num & bitChecker) == bitChecker){
      digitalWrite(pins[7-j], HIGH);
    }
    bitChecker = bitChecker * 2;
  }
}

/*
 * Turns off all the leds
*/
void OffLeds() {
  for (int k = 0; k < 8; k++) {
        digitalWrite(pins[k], LOW);
  }
}

/*
 * Turns on and off the given led
*/
void ledOnOff(int ledNum){
      digitalWrite(ledNum, HIGH);
      delay(200);
      digitalWrite(ledNum, LOW);
      delay(200);
}

/*
 * Called when the button is toggled

void buttonToggled(){
  lastButtonState = buttonState;
  buttonState = !buttonState;
}
*/

/*
Reads the input from the serial monitor
Returns the input
*/
int GetInput(int firstInput) {
  int chosenNumber = 0;
  if (Serial.available() > 0) { // send data only when you receive data:
    chosenNumber = Serial.parseInt(); // reading the input

    bool wrongInput = 1;          // checking if the user gave a correct input
    for (int i = 0; i < 5; i++) {
      if (chosenNumber == canBeChosen[i])
        wrongInput = 0;
    }
    if (firstInput && wrongInput) {  //if Input is not one of the numbers we want
      Serial.println("You wrote something that i don't want :(");
      return 0;
    }
  }
  return chosenNumber;
}


boolean debounceButton(boolean state) 
{
  boolean stateNow = digitalRead(buttonPin);
  if (state != stateNow){
    delay(10);
    stateNow = digitalRead(buttonPin);
  }
  lastButtonState == !stateNow;
  return stateNow;
}
