#include <Arduino.h>
#include "LedControl.h"
#include "Keypad.h"

void changeField(int field_position, byte player);
int getMove(void);
void getWinnerFrame(byte winner);
bool checkWinner(int recentMove, int player);
void winnerAnimation(byte winner);
bool checkInput(char button);
byte *getRemainingFields();
int checkFullFieldMinimax(int recentMove);

struct moveStruct // Nur für minimax
{
  int move;
  int score; //zwischen -9 und 9: Je höher desto bessere Change für den COM
};
struct moveStruct findBestMoveMinimax(int recentMove, bool currentPlayerIsComIsCom);
struct moveStruct minOrMaxMinimax(bool currentPlayerIsCom, moveStruct scores[], int amountRemainingFields);

unsigned int delaytime = 1000;

const byte EMPTY_FIELD = 0;
const byte PLAYER = 1;
const byte COM = 2;
const byte NO_WINNER = 3;

//LED
LedControl lc = LedControl(12, 11, 10, 1);

byte frame[8];

//Tastatur
const byte KEYBOARD_ROWS = 4;
const byte KEYBOARD_COLS = 4;

//Kann bei Ausrichtung des Keypads angepasst werden
char keyboardKeys[KEYBOARD_ROWS][KEYBOARD_COLS] = {
  {'3' , '6' , '9' , NULL},
  {'2' , '5' , '8' , NULL},
  {'1' , '4' , '7' , NULL},
  {NULL, NULL, NULL, NULL},
};

byte rowPins[KEYBOARD_COLS] = {9, 8, 7, 6};
byte colPins[KEYBOARD_ROWS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(keyboardKeys), rowPins, colPins, KEYBOARD_ROWS, KEYBOARD_COLS);

byte tictactoeField[9];


void setup(void) 
{
  lc.shutdown(0, false);
  // Niedriege Helligkeit. Kann erhöht werden: 0-7
  lc.setIntensity(0, 0);
  lc.clearDisplay(0);
}

void loop() {
  for (int i = 0; i < 9; i++)
    tictactoeField[i] = EMPTY_FIELD;
  for (int i = 0; i < 8; i++)
  {
    frame[i] = B00000000;
    lc.setRow(0, i, frame[i]);
  }
  byte movePlayer, moveCom;
  byte winner;
  byte amountOfRounds = 0; //Minimax vor Runde 2 verzögert das Programm stark

  while (true) {
    movePlayer = getMove();
    changeField(movePlayer, PLAYER);
    tictactoeField[movePlayer] = PLAYER;
    if (checkWinner(movePlayer, PLAYER)) {
      winner = PLAYER;
      break;
    }

    byte *remainingFieldsPtr = getRemainingFields();
    const int amountRemainingFields = *remainingFieldsPtr;
    free(remainingFieldsPtr);
    if (amountRemainingFields == 0) { // Das Feld ist voll
      winner = NO_WINNER;
      break;
    }

    delay(500);
    if (amountOfRounds > 0)
    {
      moveStruct move = findBestMoveMinimax(movePlayer, true);
      moveCom = move.move;
    }
    else
    {
      (movePlayer == 4) ? (moveCom = 7) : (moveCom = 4); //Falls das mittlere Feld nicht genommen wurde, wirds besetzt
    }
    changeField(moveCom, COM);
    tictactoeField[moveCom] = COM;
    if (checkWinner(moveCom, COM)) {
      winner = COM;
      break;
    }
    ++amountOfRounds;
  }
  delay(500);
  winnerAnimation(winner);
}

/* Tic Tac Toe Feld:
    Reihe 0 und 1: 0 1 2
    Reihe 3 und 4: 3 4 5
    Reihe 6 und 7: 6 7 8
*/

void changeField(int field_position, byte player) 
{

  /* Rundet zu einem vielfachen von 3 ab z.B. 8 zu 6 oder 4 zu 3.
     Wie im Feld oben beschrieben ist jede Reihe ein vielfaches
     von Drei und die nachfolgende Reihe
  */
  int row = (field_position / 3) * 3;

  /* Der Rest von field_position % 3 ergibt die spalte von rechts
     nach links, da aber der bitshift auch von rechts nach links
     geht wird das ergebnis von 2 abgezogen um es zu invertieren
  */
  int collumn =  2 - (field_position % 3);

  if (player == PLAYER) {
    frame[row] |= B00000011 << collumn * 3; //Bewegt das Quadrat in richtige Position
    frame[row + 1] |= B00000011 << collumn * 3;
  }

  if (player == COM) {
    frame[row] |= B00000001 << collumn * 3; //Bewegt die Linie in richtige Position
    frame[row + 1] |= B00000010 << collumn * 3;
  }

  //Bild wird generiert
  for (byte i = 0; i < 8; ++i) {
    lc.setRow(0, i, frame[i]);
  }
}

int getMove(void)
{
  char button;
  while (true) {
    button = customKeypad.getKey();
    if (button == 0) { //Noch keine Taste wurde gedrückt
      continue;
    }
    if (checkInput(button)) {
      break;
    }
  }
  return button -= '1';
}

void getWinnerFrame(byte winner)
{
  if (winner == PLAYER) {
    byte winnerFrame[8] = {B11111111, B10000001, B10111101, B10111101, B10111101, B10111101, B10000001, B11111111};
    for (byte i = 0; i < 8; ++i) {
      frame[i] = winnerFrame[i];
    }
  }
  else if (winner == COM) {
    byte winnerFrame[8] = {B11111111, B10000001, B10100001, B10010001, B10001001, B10000101, B10000001, B11111111};
    for (byte i = 0; i < 8; ++i) {
      frame[i] = winnerFrame[i];
    }
  }
  else if (winner == NO_WINNER) {
    byte winnerFrame[8] = {B11111111, B10000001, B10000001, B10000001, B10000001, B10000001, B10000001, B11111111};
    for (byte i = 0; i < 8; ++i) {
      frame[i] = winnerFrame[i];
    }
  }
}

bool checkWinner(int recentMove, int player)
{
  byte collumn = recentMove / 3;
  byte row = recentMove % 3;
  byte checkTictactoe[3][3];

  for (byte i = 0; i < 3; i++) {
    for (byte j = 0; j < 3; j++) {
      checkTictactoe[i][j] = tictactoeField[3 * i + j];
    }
  }

  if (checkTictactoe[0][row] == player && checkTictactoe[1][row] == player && checkTictactoe[2][row] == player) { // reihen
    return true;
  }
  if (checkTictactoe[collumn][0] == player && checkTictactoe[collumn][1] == player && checkTictactoe[collumn][2] == player) { // spalten
    return true;
  }
  if (checkTictactoe[0][0] == player && checkTictactoe[1][1] == player && checkTictactoe[2][2] == player) { // 1. diagonale
    return true;
  }
  if (checkTictactoe[2][0] == player && checkTictactoe[1][1] == player && checkTictactoe[0][2] == player) { // 2. diagonale
    return true;
  }

  return false;
}

void winnerAnimation(byte winner)
{
  for (byte winningCycles = 0; winningCycles < 5; ++winningCycles) {
    getWinnerFrame(winner);
    for (byte i = 0; i < 8; ++i) {
      lc.setRow(0, i, frame[i]);
    }
    delay(500);
    getWinnerFrame(NO_WINNER);
    for (byte i = 0; i < 8; ++i) {
      lc.setRow(0, i, frame[i]);
    }
    delay(500);
  }
  return;
}

bool checkInput(char button)
{
  button -= '1'; // ASCII -> Dezimal -> Minus eins, da der Field Array mit 0 Anfängt

  byte *remainingFieldsPtr = getRemainingFields();
  const int amountRemainingFields = *remainingFieldsPtr;
  bool fieldIsTaken = true;

  for (int i = 1; i < amountRemainingFields + 1; i++) { // Testet ob Feld belegt ist
    if (button == remainingFieldsPtr[i]) {
      fieldIsTaken = false;
    }
  }
  if (fieldIsTaken) {
    free(remainingFieldsPtr);
    return false;
  }

  free(remainingFieldsPtr);
  return true; //Eingabe war erfolgreich
}

byte *getRemainingFields()
{
  byte countFreeFields = 0;

  for (byte i = 0; i < 9; i++) {
    if (tictactoeField[i] == EMPTY_FIELD) {
      countFreeFields++;
    }
  }

  byte *remainingFields = (byte*)malloc((countFreeFields + 1) * sizeof(byte)); // Wert des Pointers ist Anzahl verbleibender Felder z.B. 4,0,1,2,6,7
  *remainingFields = countFreeFields;

  for (byte i = 0, fillArrayCount = 1; i < 9; i++) {
    if (tictactoeField[i] == EMPTY_FIELD) {
      remainingFields[fillArrayCount] = i; // Jedes drauffolgendes Element ist ein freies Feld
      fillArrayCount++;
    }
  }
  return remainingFields;
}

struct moveStruct findBestMoveMinimax(int recentMove, bool currentPlayerIsComIsCom)
{
  byte *remainingFieldsPtr = getRemainingFields();
  const byte amountRemainingFields = *remainingFieldsPtr;
  moveStruct move;
  move.move = recentMove;

  // Testet ob Spiel vorbei ist
  const int result = checkFullFieldMinimax(recentMove);
  if (result != 0)
  {
    move.score = result * (amountRemainingFields + 1);
  }
  else if (amountRemainingFields == 0)
  {
    move.score = 0;
  }
  else // Wenn Spiel nicht vorbei ist
  {
    moveStruct scores[amountRemainingFields];
    for (int i = 0; i < amountRemainingFields; i++) // Scores von allen Feldern einsammeln
    {
      scores[i].move = remainingFieldsPtr[i + 1];
      int currentPlayerIsComField;
      (currentPlayerIsComIsCom) ? (currentPlayerIsComField = COM) : (currentPlayerIsComField = PLAYER);
      tictactoeField[scores[i].move] = currentPlayerIsComField; // Feld wird temporär belegt
      moveStruct tempMove = findBestMoveMinimax(scores[i].move, !currentPlayerIsComIsCom);
      scores[i].score = tempMove.score;
      tictactoeField[scores[i].move] = EMPTY_FIELD; // Feld wird zurückgesetzt
    }
    move = minOrMaxMinimax(currentPlayerIsComIsCom, scores, amountRemainingFields); // Move mit höchsten Score wird heraussortiert
  }

  free(remainingFieldsPtr);
  return move;
}

struct moveStruct minOrMaxMinimax(bool currentPlayerIsCom, moveStruct scores[], int amountRemainingFields)
{
  if (currentPlayerIsCom) // Höchster Score wird gesucht
  {
    moveStruct max = scores[0];
    for (int i = 1; i < amountRemainingFields; i++)
    {
      if (scores[i].score > max.score)
      {
        max = scores[i];
      }
    }
    return max;
  }
  else // Niedrigster Score wird gesucht
  {
    moveStruct min = scores[0];
    for (int i = 1; i < amountRemainingFields; i++)
    {
      if (scores[i].score < min.score)
      {
        min = scores[i];
      }
    }
    return min;
  }
}

int checkFullFieldMinimax(int recentMove)
{
  if (checkWinner(recentMove, COM))
  {
    return 1;
  }
  else if (checkWinner(recentMove, PLAYER))
  {
    return -1;
  }
  else
  {
    return 0;
  }
}
