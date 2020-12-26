#define player1 2
#define player2 3
#define delay_micros 200000
#define delay_ledmicros 400000
#define delayNum 30

int input = 0; //user input
int canBeChosen[2] = {1, 2}; //list for options
int pins[8]; //array for the led pins

int scores[2] = {0, 0}; //scores of the players
int penalty[2] = {0, 0}; //indicators whether players have penalty

unsigned long interrupt_counter1 = 0;
unsigned long interrupt_counter2 = 0;

unsigned long time_target;
unsigned long time_guess;

volatile bool buttons[2] = {LOW, LOW};
const int players[2] = {2, 3};


//unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
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
  pinMode(player1, INPUT);
  pinMode(player2, INPUT);

}

void loop() {
  input = GetInput(0);
  if (input == 1) {
    Serial.println("Nice choice. Enjoy!");
    buttons[1] = LOW; buttons[0] = LOW;
    scores[0] = 0; scores[1] = 0;
    Game1Starter();
  } else if (input == 2){
    Serial.println("Nice choice. Enjoy!");
    buttons[1] = LOW; buttons[0] = LOW;
    scores[0] = 0; scores[1] = 0;
    Game2Starter();
  }
}

void Game1Starter() {
  attachInterrupt(digitalPinToInterrupt(player1), buttonToggeled1, RISING);
  attachInterrupt(digitalPinToInterrupt(player2), buttonToggeled2, RISING);
  
  while(scores[0] != 5 && scores[1] != 5) {
    //starting the game
    if (buttons[0] && !(penalty[0])) {
      Button1Pressed();
    }else if (buttons[1] && !penalty[1]) {
        Button2Pressed();
    }
    OffLeds(); //turning of the leds when the round finishes
  }
  //printing the winner
  int winner = scores[0] > scores[1] ? 1 : 2; //
  Serial.print("Player "); Serial.print(winner); Serial.println(" WON THE GAME"); 
  Serial.print("Enter:\nSelect 1 for the first game.\nSelect 2 for the second game.\n");

}

/*
 * ISR functions for the first game
 */
void buttonToggeled1() {
  if ((micros()-interrupt_counter1) > delay_micros) { //solving the bounce problem
    if (buttons[0]) {
      GameOver();
    } else {
      buttons[0] = true;
      interrupt_counter1 = micros();
    }
  }
}

void buttonToggeled2() {
  if ((micros()-interrupt_counter2) > delay_micros) { //solving the bounce problem
    if (buttons[1]) {
      GameOver();
    } else {
      buttons[1] = true;
      interrupt_counter2 = micros();
    }
  }
}

void GameOver() {
  buttons[0] = false; buttons[1] = false;
  OffLeds();
}
/*
 * Function for whe the first player pushes the button first
 */
void Button1Pressed() {
      int s = 7;
      int p1 = 0;
      buttons[0] = false; buttons[1] = false;
      for (int i = 0; i < 8; i++) {
         if (buttons[0]) {
           Serial.println("Player 1 cheated");
           penalty[0] = 1;
           GameOver();
           break;
         }
         digitalWrite(pins[i], HIGH);
         if (!penalty[1] || (penalty[1] && i > 4)) {
           if (buttons[1]) {
             p1 = 1;
           }
         } else {
           buttons[1] = false;
         }
         if (p1) {
            if (s - i < 0) {
              digitalWrite(pins[s], HIGH);
              OffLeds();
              GameOver();
              Serial.println("No one won");
              break;
            }
            digitalWrite(pins[s], HIGH);
            s--;
         }
         delay(delayNum);
         if (i == 7) {
          scores[0]++;
          Serial.print("Player 1: "); Serial.print(scores[0]); Serial.print(" Player 2: "); Serial.println(scores[1]); 
         }
      }
      p1 = 0;
      penalty[1] = 0;
}
/*
 * Function for whe the second player pushes the button first
 */
void Button2Pressed() {
      int s = 0;
      int p1 = 0;
      buttons[1] = false; buttons[0] = false;
      for (int i = 7; i >= 0; i--) {
         if (buttons[1]) {
           Serial.println("Player 2 cheated");
           penalty[1] = 1;
           GameOver();
           break;
         }
         digitalWrite(pins[i], HIGH);
         if (!penalty[0] || (penalty[0] && i < 3)) {
           if (buttons[0]) {
             p1 = 1;
           }
         } else {
           buttons[0] = LOW;
         }
         
         if (p1) {
            if (i - s < 0) {
              digitalWrite(pins[s], HIGH);
              OffLeds();
              GameOver();
              Serial.println("No one won");
              break;
            }
            digitalWrite(pins[s], HIGH);
            s++;
         }
         delay(delayNum);
         if (i == 0){
          scores[1]++;
          Serial.print("Player 1: "); Serial.print(scores[0]); Serial.print(" Player 2: "); Serial.println(scores[1]); 
         }
      }
      p1 = 0;
      penalty[0] = 0;
}

















































//-------------------------------------------------------------
void buttonToggeled1_2 () {
  if ((micros()-interrupt_counter1) > delay_micros) { //solving the bounce problem
    buttons[0] = !buttons[0];
    interrupt_counter1 = micros();
  }
}

void buttonToggeled2_2 () {
  if ((micros()-interrupt_counter2) > delay_micros) { //solving the bounce problem
    buttons[1] = !buttons[1];
    interrupt_counter2 = micros();
  }
}

void Game2Starter() {
  int whowillplay = 1;
    while (scores[0] != 5 && scores[1] != 5) { //game continues till one of the players' score become 5
      whowillplay = !whowillplay; //changing the player when the round ends
      
      attachInterrupt(digitalPinToInterrupt(player1), buttonToggeled1_2, CHANGE);//attaching interrupts
      attachInterrupt(digitalPinToInterrupt(player2), buttonToggeled2_2, CHANGE);
 
      Serial.print("Player "); Serial.print(whowillplay+1); Serial.println(" please press the button and set the time"); //printing a command for informing the player
      
      Game2(whowillplay); //Starting the game
      
      Serial.print("Scores are: Player1 = "); Serial.print(scores[0]); Serial.print(" Player2 = "); Serial.println(scores[1]); //printing the scores
    }
    //printing the winner
    int winner = scores[0] < scores[1]? 2 : 1;
    Serial.print("Player "); Serial.print(winner); Serial.println(" WON THE GAME!!");
    Serial.print("Enter:\nSelect 1 for the first game.\nSelect 2 for the second game.\n");
}

void Game2(int p) {
  int first = 1;
  unsigned long start, finish;
  buttons[0] = false; buttons[1] = false;
  while (1) {
    if (buttons[p] && first) {
      OnLeds();
      start = millis();
      first = 0;
    }
    if (!buttons[p] && !first) {
      finish = millis();
      OffLeds();
      break;
    }
  }
  time_target = finish - start;
  first = 1;
  Serial.println("Oppenent should start when s/he sees the lights turned on.");
  delay(3000);
  buttons[0] = false; buttons[1] = false;
  OnLeds();
  while (1) {
    if (buttons[(p+1)%2] && first) {
      start = millis();
      first = 0;
    }
    if (!buttons[(p+1)%2] && !first) {
      OffLeds();
      finish = millis();
      break;
    }
  }
  time_guess = finish - start;

  if ((time_guess < (time_target * 1.1)) && (time_guess > (time_target * 0.9))) {
    (scores[(p+1)%2])++;
  } else {
    (scores[p])++;
  }
  Serial.print("Target time: "); Serial.print(time_target); Serial.print(" Guessed time: "); Serial.println(time_guess);

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
 * Turns on all the leds
*/
void OnLeds() {
  for (int k = 0; k < 8; k++) {
        digitalWrite(pins[k], HIGH);
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
    for (int i = 0; i < 2; i++) {
      if (chosenNumber == canBeChosen[i])
        wrongInput = 0;
    }
    if (wrongInput) {  //if Input is not one of the numbers we want
      //firstInput is for figuring out 
      Serial.println("Invalid input, please try again.");
      return 0;
    }
  }
  return chosenNumber;
}
