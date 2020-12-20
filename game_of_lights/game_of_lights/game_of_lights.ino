#define player1 2
#define player2 3


int input = 0; //user input
int canBeChosen[5] = {1, 2, 3, 4, 5}; //list for options
int pins[8]; //array for the led pins

volatile bool buttonState1 = LOW;  //state of the button now
volatile bool lastButtonState1 = LOW;  //state of the button before
volatile bool buttonState2 = LOW;  //state of the button now
volatile bool lastButtonState2 = LOW;  //state of the button before


unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceTime = 30;    // the debounce time; increase if the output flickers


void setup() {
  // put your setup code here, to run once:
  //  setup for serial and some prints
  Serial.begin(115200);
  Serial.setTimeout(2); // for reducing the waiting period 
  Serial.print("Enter:\n1 for the first game.\n2 for the second game.\n");

  // Setup for the pins and the array
  for (int i = 0; i < 8; i++) {
    pins[i] = i + 4; //leds are connected to the pins through 4-11
    pinMode(pins[i], OUTPUT);
  }
  pinMode(player1, INPUT_PULLUP);
  pinMode(player2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(player1), buttonToggeled1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(player2), buttonToggeled2, CHANGE);

}

void loop() {
  input = GetInput(1);
  if (input == 1) {
    Game1();
  } else if (input == 2){
    //Game2();
  }
}

void Game1() {
  int p1 = 0;
  int s = 7;
  while (1){
    if (buttonState2){
      deBounce(buttonState2);
      for (int i = 0; i < 8; i++) {
         digitalWrite(pins[i], HIGH);
         delay(300);
         if (buttonState1) {
            deBounce(buttonState1);
            p1 = 1;
         }
         if (p1) {
            digitalWrite(pins[s], HIGH);
            s--;
         }
         if (s == i) {
            OffLeds();
            break;
         }
      }
      s = 7;
      p1 = 0;
    }
  }  
}

void buttonToggeled1() {
  buttonState1 = !buttonState1;
}
void buttonToggeled2() {
  buttonState2 = !buttonState2;
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
/*
 * Debounce handler
 * Takes the new reading
 */
void deBounce (volatile bool buttonState)
{
  unsigned long now = millis ();
  do
  {
    // on bounce, reset time-out
    if (digitalRead (buttonState) == LOW)
      now = millis ();
  } 
  while (digitalRead (buttonState) == LOW ||
    (millis () - now) <= debounceTime);

}
