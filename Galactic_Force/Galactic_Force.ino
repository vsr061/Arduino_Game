//Author : Viraj Rane
//Created on 04/07/2015 

#include <LiquidCrystal.h>
#include "wining_song_notes.h"

//Just to simplify code to understand :
#define BEGIN 1
#define LOSE 2
#define WON 3
#define EASY 400
#define NORMAL 200

#define lcd_rows 16
#define number_of_asteroids 20   //Choose no. of asteroids

// Here come our custom characters :
byte SPACESHIP[8] = {
  0b00100,
  0b00100,
  0b10110,
  0b11111,
  0b11111,
  0b10110,
  0b00100,
  0b00100
};

byte ASTEROID[8] = {
  0b00000,
  0b10101,
  0b01110,
  0b11111,
  0b11111,
  0b01110,
  0b10101,
  0b00000
};

byte CHAMPIONCUP[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b01110,
  0b00100,
  0b00100,
  0b01110,
  0b11111
};

byte SADFACE[8] = {
  0b00000,
  0b11011,
  0b11011,
  0b00000,
  0b00100,
  0b00000,
  0b01110,
  0b10001
};

//Some Variables to use later for different purposes :
int playDifficulty = 1; //This will help to show Difficulty select menu only once
int playWarning = 1;  //This will help me to show "AVOID ASTEROID" warning only once
int build;           //This will help me to accumulate asteroid at differnet positions
int asteroid_x[number_of_asteroids], asteroid_y[number_of_asteroids];
int state = BEGIN;
int spaceship_y = 3;  //This will help to move spaceship up or down
int score = 0;
int timer;           // Difficulty will alter timer to change speed of asteroids
int j = 0;
int note = 6;
boolean firstpart = true; // This also work similar like playDifficulty
boolean secondpart = false;

LiquidCrystal lcd(12, 11, 5, 4, 9, 8);  //Pin 2 and 3 are use as interrupt pins so, now 8 and 9 will replace those pins


void setup() {

  lcd.createChar(0, SPACESHIP);
  lcd.createChar(1, ASTEROID);
  lcd.createChar(3, CHAMPIONCUP);
  lcd.createChar(4, SADFACE);

  lcd.begin(16, 2);

  attachInterrupt(0 , change , RISING);
  attachInterrupt(1 , change1 , RISING);

  //Game Start :
  lcd.setCursor(2, 0);
  lcd.print("!!Welcome!!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("!!Welcome!!");
  delay(200);
  for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(150);
  }
  lcd.clear();
  lcd.print("VIRAJ's");
  lcd.setCursor(0, 1);
  lcd.print("*GALACTIC FORCE*");
  delay(3000);
  for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayRight();
    // wait a bit:
    delay(150);
  }

  //This Block will place asteroids randomly :
  build = lcd_rows;
  for (int i = 0; i <= number_of_asteroids; i++) {
    // randomly pick a number between the ship's length and 8
    build += random(3, 8);
    asteroid_x[i] = build;
    asteroid_y[i] = random(0, 2);
  }

}


void loop() {

  if (playDifficulty == 1) {
    // First Part to seclect Difficulty :
    if (firstpart == true) {
      lcd.clear();
      lcd.print("DIFFICULTY:");
      lcd.setCursor(0, 1);
      lcd.print("U:Normal  D:Easy");
      if (spaceship_y != 3) {
        if (spaceship_y == 1) {
          timer = EASY ;
          spaceship_y = 3;
        }
        if (spaceship_y == 0) {
          timer = NORMAL ;
          spaceship_y = 3;
        }
        firstpart = false;
        secondpart = true;
      }
    }

    // Second Part to show simple press button message :
    if (secondpart == true) {
      lcd.clear();
      lcd.print("Press any key");
      lcd.setCursor(0, 1);
      lcd.print("to START GAME");
      if (spaceship_y != 3) {
        playDifficulty = 0;
      }
    }

    delay(200);  //This will remove flickering due to clear function
  }

  // Avoid Asteroid message :
  if (spaceship_y != 3 && playWarning == 1 && state == BEGIN && playDifficulty == 0 ) {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("AVOID");
    lcd.setCursor(2, 1);
    lcd.write((uint8_t)1);
    lcd.setCursor(3, 1);
    lcd.print("ASTEROIDS");
    lcd.setCursor(12, 1);
    lcd.write((uint8_t)1);
    delay(5000);
    playWarning = 0;
  }

  // MAIN GAME BEGIN HERE :
  if (spaceship_y != 3 && state == BEGIN) {
    lcd.clear();
    for (int i = 0; i < number_of_asteroids; i++) {
      lcd.setCursor(1, spaceship_y);
      lcd.write((uint8_t)0);
      // move them to the left 1 space
      asteroid_x[i] = asteroid_x[i] - 1;
      if (asteroid_x[i] == 0) {
        score++; //To compute score
      }
      // and draw them where they should appear
      if (asteroid_x[i] >= 0 && asteroid_x[i] <= lcd_rows) {
        lcd.setCursor(asteroid_x[i], asteroid_y[i]);
        lcd.write((uint8_t)1);
      }
      tone(6, 150, 500); //U can try another frequencies

      //This statement will decide whether spaceship and asteroid are on same postion or not :
      if ((asteroid_x[i] == 1) && (asteroid_y[i] == spaceship_y)) {
        state = LOSE;
      }
    }
  }

  //This statement will count whether all asteroids are gone or not :
  if (asteroid_x[(number_of_asteroids - 1)] < 0) {
    state = WON;
  }

  //To show message after losing the game :
  if (state == LOSE) {
    lcd.clear();
    tone(6, 1000);
    delay(500);
    tone(6, 900);
    delay(50);
    tone(6, 1200);
    delay(50);
    tone(6, 750);
    delay(400);
    noTone(6);
    lcd.setCursor(2, 0);
    lcd.print("GAME OVER ");
    lcd.write((uint8_t)4);
    lcd.setCursor(4, 1);
    lcd.print("SCORE:");
    lcd.print(score);
  }

  //To show message after wining the game :
  if (state == WON) {
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("!YOU WON!");
    lcd.setCursor(0, 1);
    lcd.write((uint8_t)3);
    lcd.setCursor(4, 1);
    lcd.print("SCORE:");
    lcd.print(score + 1);
    lcd.setCursor(15, 1);
    lcd.write((uint8_t)3);
    LevelClear();       //Play mario's "Level Clear song"
  }

  delay(timer);  //timer will change asteroid's speed
  noTone(6);

}


void change() {
  spaceship_y = 0;
  delay(100);
}

void change1() {
  spaceship_y = 1;
  delay(100);
}


void LevelClear()
{
  tone(note, G3);
  delay(166);
  tone(note, C4);
  delay(166);
  tone(note, E4);
  delay(166);
  tone(note, G4);
  delay(166);
  tone(note, C5);
  delay(166);
  tone(note, E5);
  delay(166);
  tone(note, G5);
  delay(500);
  tone(note, E5);
  delay(500);

  tone(note, E3);
  delay(166);
  tone(note, C4);
  delay(166);
  tone(note, Eb4);
  delay(166);
  tone(note, Ab4);
  delay(166);
  tone(note, C5);
  delay(166);
  tone(note, Eb5);
  delay(166);
  tone(note, Ab5);
  delay(500);
  tone(note, Eb5);
  delay(500);

  tone(note, Bb3);
  delay(166);
  tone(note, D4);
  delay(166);
  tone(note, F4);
  delay(166);
  tone(note, Bb4);
  delay(166);
  tone(note, D5);
  delay(166);
  tone(note, F5);
  delay(166);
  tone(note, Bb5);
  delay(500);
  noTone(note);
  delay(50);
  tone(note, Bb5);
  delay(166);
  noTone(note);
  delay(50);
  tone(note, Bb5);
  delay(166);
  noTone(note);
  delay(50);
  tone(note, Bb5);
  delay(166);
  tone(note, C6);
  delay(2000);
}
