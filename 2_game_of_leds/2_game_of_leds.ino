#define player1 2
#define player2 3
#define delay_micros 200000
#define delay_ledmicros 400000
#define delayNum 50 //delay for the first game

int input = 0; //user input
int canBeChosen[2] = {1, 2}; //list for options
int pins[8]; //array for the led pins

int scores[2] = {0, 0}; //scores of the players
int penalty[2] = {0, 0}; //indicators whether players have penalty

unsigned long interrupt_counter1 = 0; //last interrupt counts
unsigned long interrupt_counter2 = 0;

unsigned long time_target; //time that the player sets
unsigned long time_guess; //time that the opponent guesses

volatile bool buttons[2] = {LOW, LOW}; //buttons
const int players[2] = {2, 3}; //players


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
    buttons[1] = LOW; buttons[0] = LOW; //setting buttons to low
    scores[0] = 0; scores[1] = 0; //setting scores to 0
    Game1Starter(); //starting the game
  } else if (input == 2){
    Serial.println("Nice choice. Enjoy!");
    buttons[1] = LOW; buttons[0] = LOW; //setting buttons to low
    scores[0] = 0; scores[1] = 0; //setting scores to 0
    Game2Starter(); //starting the game
  }
}

void Game1Starter() {
  //attaching interrupts
  attachInterrupt(digitalPinToInterrupt(player1), buttonToggeled1, RISING);
  attachInterrupt(digitalPinToInterrupt(player2), buttonToggeled2, RISING);
  
  //game will go on till one of the players reach score of 5
  while(scores[0] != 5 && scores[1] != 5) { 
    //starting the game
    if (buttons[0] && !(penalty[0])) {
      Button1Pressed(); //called if player 1 presses the button
    }else if (buttons[1] && !penalty[1]) {
        Button2Pressed(); //called if player 2 presses the button
    }
    OffLeds(); //turning of the leds when the round finishes
  }
  //printing the winner
  int winner = scores[0] > scores[1] ? 1 : 2; //finds out who won
  Serial.print("Player "); Serial.print(winner); Serial.println(" WON THE GAME"); 
  Serial.print("Enter:\nSelect 1 for the first game.\nSelect 2 for the second game.\n");

}

//--------------Functions-for-the-first-game------------------
/*
 * ISR functions for the first game
 */
void buttonToggeled1() {
  if ((micros()-interrupt_counter1) > delay_micros) { //solving the bounce problem
    if (buttons[0]) { //if player polls
      GameOver();
    } else {
      buttons[0] = true;
      interrupt_counter1 = micros();
    }
  }
}

void buttonToggeled2() {
  if ((micros()-interrupt_counter2) > delay_micros) { //solving the bounce problem
    if (buttons[1]) { //if player polls
      GameOver();
    } else {
      buttons[1] = true;
      interrupt_counter2 = micros();
    }
  }
}

void GameOver() {
  buttons[0] = false; buttons[1] = false; //buttons are deactivated when game finishes
  OffLeds();
}
/*
 * Function for when the first player pushes the button first
 */
void Button1Pressed() {
      int s = 7;
      int p1 = 0; //variable to make our function know that other player has pushed the button
      buttons[0] = false; buttons[1] = false;
      for (int i = 0; i < 8; i++) {
         if (buttons[0]) { //if player 1 polls
           Serial.println("Player 1 cheated");
           penalty[0] = 1; 
           GameOver();
           break; //game finishes
         }
         digitalWrite(pins[i], HIGH);
         //if player 2 doesn't have a penalty
         //or if he has but it is after 5 leds are turned on by the other player
         if (!penalty[1] || (penalty[1] && i > 4)) {
           if (buttons[1]) {
             p1 = 1; //changing the variable to make our function know that other player has pushed the button
           }
         } else {
           buttons[1] = false;
         }
         if (p1) { //if the other player pushed the button
            if (s - i < 0) { //if leds cancelled each other
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
         if (i == 7) { //if the player 1 reached to the other end
          scores[0]++; //score of the player 1 is incremented
          Serial.print("Player 1: "); Serial.print(scores[0]); Serial.print(" Player 2: "); Serial.println(scores[1]); 
         }
      }
      p1 = 0;
      penalty[1] = 0; //if there was a penalty it is gone now
}
/*
 * Function for when the second player pushes the button first
 */
void Button2Pressed() {
      int s = 0;
      int p1 = 0; //variable to make our function know that other player has pushed the button
      buttons[1] = false; buttons[0] = false;
      for (int i = 7; i >= 0; i--) {
         if (buttons[1]) {
           Serial.println("Player 2 cheated");
           penalty[1] = 1;
           GameOver();
           break; //game finishes
         }
         digitalWrite(pins[i], HIGH);
         //if player 2 doesn't have a penalty
         //or if he has but it is after 5 leds are turned on by the other player
         if (!penalty[0] || (penalty[0] && i < 3)) { 
           if (buttons[0]) {
             p1 = 1; // changing the variable to make our function know that other player has pushed the button
           }
         } else {
           buttons[0] = LOW;
         }
         
         if (p1) { //if the other player pushed the button
            if (i - s < 0) { //if leds cancelled each other
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
         if (i == 0){ //if the player 2 reached to the other end
          scores[1]++; //score of the player 2 is incremented
          Serial.print("Player 1: "); Serial.print(scores[0]); Serial.print(" Player 2: "); Serial.println(scores[1]); 
         }
      }
      p1 = 0;
      penalty[0] = 0; //if there was a penalty it is gone now
}

//--------------Functions-for-the-second-game------------------

/*
 * ISR functions for the second game
 */
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
  attachInterrupt(digitalPinToInterrupt(player1), buttonToggeled1_2, CHANGE);//attaching interrupts
  attachInterrupt(digitalPinToInterrupt(player2), buttonToggeled2_2, CHANGE);
  int whowillplay = 1;
    while (scores[0] != 5 && scores[1] != 5) { //game continues till one of the players' score become 5
      whowillplay = !whowillplay; //changing the player when the round ends
      
 
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
  int first = 1; //for seeing if it is the first time the player pushed the button
  unsigned long start, finish;
  buttons[0] = false; buttons[1] = false;
  while (1) {
    if (buttons[p] && first) { //player starts so push the button
      OnLeds();
      start = millis(); //time when the button is became HIGH
      first = 0; //changing the variable because from now on it will not be the first time
    }
    if (!buttons[p] && !first) { //player lets go off the button
      finish = millis(); //time when the button is became LOW after becoming HIGH
      OffLeds();
      break;
    }
  }
  time_target = finish - start; //time interval between HIGH and LOW
  first = 1;
  Serial.println("Oppenent should start when s/he sees the lights turned on.");
  delay(2000);
  buttons[0] = false; buttons[1] = false;
  OnLeds();
  //doing the similar things for the second player
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

  //Declearing who won this round
  if ((time_guess < (time_target * 1.1)) && (time_guess > (time_target * 0.9))) {
    (scores[(p+1)%2])++;
  } else {
    (scores[p])++;
  }
  Serial.print("Target time: "); Serial.print(time_target); Serial.print(" Guessed time: "); Serial.println(time_guess);

}


//---------general-functions-----------
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
*Reads the input from the serial monitor
*Returns the input
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
