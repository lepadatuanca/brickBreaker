#include "LedControl.h"
#include <LiquidCrystal.h>

LedControl lc = LedControl (11, 13, 10, 1);
LiquidCrystal lcd (9, 8, 5, 4, 3, 2);

int buttonPin = 6;

int UD = 0, LR = 0; //up-down, left-right movement variables
int buttonState = 1, defaultButtonState = 1;
int defaultUD = -1, defaultLR = -1;
int reseter = 0;

class brickBall {
    short x, y;
    short ballDirection; //takes values: 1-up\, 2-up|, 3-up/, -1-down/, -2-down|, -3-down\, 0-standing
  public:
    brickBall() {
      x = 3;
      y = 6;
      ballDirection = 0;
    }
    short getBallDirection() {
      return ballDirection;
    }
    void setBallDirection (short setValue) {
      ballDirection = setValue;
    }
    short getBallX() {
      return x;
    }
    void setBallX (short setValue) {
      x = setValue;
    }
    short getBallY() {
      return y;
    }
    void setBallY (short setValue) {
      y = setValue;
    }
    void nextXY (short* nextX, short* nextY) {
      //returns the coordinates for the next position of the ball, taking into account just direction
      if (ballDirection == 1) {
        *nextX = x - 1;
        *nextY = y - 1;
      }
      else if (ballDirection == 2) {
        *nextX = x;
        *nextY = y - 1;
      }
      else if (ballDirection == 3) {
        *nextX = x + 1;
        *nextY = y - 1;
      }
      else if (ballDirection == -1) {
        *nextX = x - 1;
        *nextY = y + 1;
      }
      else if (ballDirection == -2) {
        *nextX = x;
        *nextY = y + 1;
      }
      else if (ballDirection == -3) {
        *nextX = x + 1;
        *nextY = y + 1;
      }
    }
};

class brickBoard {
    short middle;
  public:
    brickBoard() {
      middle = 3;
    }
    short getBoardMiddle() {
      return middle;
    }
    void setBoardMiddle (short setValue) {
      middle = setValue;
    }
    void moveBoard (short paramDirection) {
      if (paramDirection == -1 && middle > 1) {
        //move board left
        lc.setLed (0, 7, middle + 1, false);
        middle--;
        lc.setLed (0, 7, middle - 1, true);
      }
      else if (paramDirection == 1 && middle < 6) {
        //move board right
        lc.setLed (0, 7, middle - 1, false);
        middle++;
        lc.setLed (0, 7, middle + 1, true);
      }
    }
};

class brickBreaker {
    short bricks[8][8];
    short countBricks;
    short option; //menu option
    short difficulty;
    short level;
    brickBoard board;
    brickBall ball;
    bool inGame;
    String menuOptions[5];
    int score;
    short lives;
    int defaultScore;
    short defaultLives;
    bool start;

  public:
    brickBreaker() {
      countBricks = 18;
      option = 0;
      level = 1;
      difficulty = 1;
      inGame = false;
      menuOptions[0] = "Brick Breaker";
      menuOptions[1] = "New game ";
      menuOptions[2] = "Difficulty ";
      menuOptions[3] = "Score: ";
      menuOptions[4] = "Lives: ";
      score = 0;
      lives = 3;
      defaultScore = -1;
      defaultLives = -1;
      start = true;
    }

    void mainMenu() {
      if (inGame == false) {
        //main menu options are not displayed during the game
        UD = analogRead (A1);
        //converts the analog value to -1, 0 or 1 to navigate through the menu
        if (UD > 800) {
          UD = -1;
        }
        else if (UD < 200) {
          UD = 1;
        }
        else {
          UD = 0;
        }

        buttonState = digitalRead (buttonPin);

        if (UD != defaultUD || buttonState != defaultButtonState) {
          //displays main menu at the start of the game/when the button is pressed
          lcd.clear();
          lcd.setCursor (0, 0);
          lcd.print (menuOptions[option]);
          lcd.setCursor (0, 1);
          lcd.print (menuOptions[option + 1]);
          if (option == 1)
            lcd.print (difficulty);
          lcd.print (" <");
          defaultUD = UD;
          if (UD == 1)
            option--;
          else if (UD == -1)
            option++;
          //the user can't go outside the menu displayed
          if (option > 1)
            option = 1;
          if (option < 0)
            option = 0;
          if (buttonState != defaultButtonState) {
            defaultButtonState = buttonState;
            if (!buttonState) {
              switch (option) {
                case 0:
                  //prepares the start of the game
                  inGame = true;
                  start = true;
                  lc.clearDisplay (0);
                  //displays board and ball, always at the same coordinates at the start
                  lc.setLed (0, 7, 2, true);
                  lc.setLed (0, 7, 3, true);
                  lc.setLed (0, 7, 4, true);
                  lc.setLed (0, 6, 3, true);
                  //displays/sets brick matrix
                  for (int i = 0; i < 18; i++)
                  {
                    lc.setLed(0, i / 6, i % 6 + 1, true);
                    bricks[i / 6 ][i % 6 + 1] = 1;
                  }
                  break;
                case 1:
                  //difficulty picker
                  difficulty++;
                  if (difficulty > 3)
                    difficulty = 1;
                  break;
              }
            }
          }
        }
      }
      else {
        //when inside the game
        if (score != defaultScore || lives != defaultLives) {
          //alters defaults so the if is accessed only once
          defaultScore = score;
          defaultLives = lives;
          lcd.clear();
          lcd.setCursor (0, 0);
          lcd.print (menuOptions[4]);
          for (int i = 0; i < lives; i++)
            lcd.print ("o ");
          lcd.setCursor (0, 1);
          lcd.print (menuOptions[3]);
          lcd.print (score);
        }
        while (start == true) {
          //when the joystick is moved to the left or to the right, the game starts with the ball going up, diagonally left or right
          LR = analogRead (A0);
          if (LR > 800)
            LR = -1;
          else if (LR < 200)
            LR = 1;
          else
            LR = 0;

          if (LR == 1 || LR == -1) {
            startMoveBall();
            start = false;
          }
        }
        LR = analogRead (A0);
        if (LR > 800)
          LR = -1;
        else if (LR < 200)
          LR = 1;
        else
          LR = 0;
        game(); //sets the actual direction of the ball, considering collisions
        board.moveBoard (LR);
      }
    }
    void startMoveBall() {
      if (ball.getBallDirection() == 0)
        ball.setBallDirection (LR + 2); //ball's directions range from 1 to 3 upwards
    }
    void game() {
      //checks for collisions between the ball and walls/bricks/board/lower void
      short nextX = 0, nextY = 0, nextDirection = ball.getBallDirection();
      ball.nextXY (&nextX, &nextY);
      switch (nextDirection) {
        case 1:
          //ball can't go outside the matrix boundaries, direction is switched accordingly
          if (nextX == -1)
            ball.setBallDirection (3);
          else if (nextY == -1)
            ball.setBallDirection (-1);
          //if the ball hits a brick, direction is switched accordingly, brick is turned off and score increases
          else if (bricks[nextY + 1][nextX] == 1) {
            ball.setBallDirection (3);
            bricks[nextY + 1][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY + 1, nextX, false);
            countBricks--;
          }
          else if (bricks[nextY][nextX + 1] == 1) {
            ball.setBallDirection (-1);
            bricks[nextY][nextX + 1] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX + 1, false);
            countBricks--;
          }
          else if (bricks[nextY][nextX] == 1) {
            ball.setBallDirection (-3);
            bricks[nextY][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX, false);
            countBricks--;
          }
          //ball continues its movement unharmed
          else {
            lc.setLed (0, ball.getBallY(), ball.getBallX(), false);
            ball.setBallX (nextX);
            ball.setBallY (nextY);
            lc.setLed (0, ball.getBallY(), ball.getBallX(), true);
            delay (100 * (5 - difficulty));
          }
          break;
        case 2:
          if (nextY == -1)
            ball.setBallDirection (-2);
          else if (bricks[nextY][nextX] == 1) {
            ball.setBallDirection (-2);
            bricks[nextY][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX, false);
            countBricks--;
          }
          else {
            lc.setLed (0, ball.getBallY(), ball.getBallX(), false);
            ball.setBallX (nextX);
            ball.setBallY (nextY);
            lc.setLed (0, ball.getBallY(), ball.getBallX(), true);
            delay (100 * (5 - difficulty));
          }
          break;
        case 3:
          if (nextX == 8)
            ball.setBallDirection (1);
          else if (nextY == -1)
            ball.setBallDirection (-3);
          else if (bricks[nextY + 1][nextX] == 1) {
            ball.setBallDirection (1);
            bricks[nextY + 1][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY + 1, nextX, false);
            countBricks--;
          }
          else if (bricks[nextY][nextX - 1] == 1) {
            ball.setBallDirection (-3);
            bricks[nextY][nextX - 1] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX - 1, false);
            countBricks--;
          }
          else if (bricks[nextY][nextX] == 1) {
            ball.setBallDirection (-1);
            bricks[nextY][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX,  false);
            countBricks--;
          }
          else {
            lc.setLed (0, ball.getBallY(), ball.getBallX(), false);
            ball.setBallX (nextX);
            ball.setBallY (nextY);
            lc.setLed (0, ball.getBallY(), ball.getBallX(), true);
            delay (100 * (5 - difficulty));
          }
          break;
        case -1:
          if (nextX == -1)
            ball.setBallDirection (-3);
          //for the negative directions (going down) we must check if the ball hits the board or misses
          else if (nextY == 7)
            boardRicochet();
          else if (bricks[nextY - 1][nextX] == 1) {
            ball.setBallDirection (-3);
            bricks[nextY - 1][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY - 1, nextX,  false);
            countBricks--;
          }
          else if (bricks[nextY][nextX + 1] == 1) {
            ball.setBallDirection (1);
            bricks[nextY][nextX + 1] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX + 1, false);
            countBricks--;
          }
          else if (bricks[nextY][nextX] == 1) {
            ball.setBallDirection (3);
            bricks[nextY][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX, false);
            countBricks--;
          }
          else {
            lc.setLed (0, ball.getBallY(), ball.getBallX(), false);
            ball.setBallX(nextX);
            ball.setBallY(nextY);
            lc.setLed (0, ball.getBallY(), ball.getBallX(),  true);
            delay (100 * (5 - difficulty));
          }
          break;
        case -2:
          if (nextY == 7)
            boardRicochet();
          else if (bricks[nextY][nextX] == 1) {
            ball.setBallDirection (2);
            bricks[nextY][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX,  false);
            countBricks--;
          }
          else {
            lc.setLed (0, ball.getBallY(), ball.getBallX(), false);
            ball.setBallX (nextX);
            ball.setBallY (nextY);
            lc.setLed (0, ball.getBallY(), ball.getBallX(),  true);
            delay (100 * (5 - difficulty));
          }
          break;
        case -3:
          if (nextX == 8)
            ball.setBallDirection (-1);
          else if (nextY == 7)
            boardRicochet();
          else if (bricks[nextY - 1][nextX] == 1) {
            ball.setBallDirection (-1);
            bricks[nextY - 1][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY - 1, nextX, false);
            countBricks--;
          }
          else if (bricks[nextY][nextX - 1] == 1) {
            ball.setBallDirection (3);
            bricks[nextY][nextX - 1] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX - 1, false);
            countBricks--;
          }
          else if (bricks[nextY][nextX] == 1) {
            ball.setBallDirection (1);
            bricks[nextY][nextX] = 0;
            score += difficulty * 10;
            lc.setLed (0, nextY, nextX, false);
            countBricks--;
          }
          else {
            lc.setLed (0, ball.getBallY(), ball.getBallX(), false);
            ball.setBallX(nextX);
            ball.setBallY(nextY);
            lc.setLed (0, ball.getBallY(), ball.getBallX(), true);
            delay (100 * (5 - difficulty));
          }
          break;
      }
    }
    void boardRicochet() {
      short mid = board.getBoardMiddle();
      short x = ball.getBallX();
      if (countBricks == 0) {
        if (level == 3) {
          //if the last level is finished, a message is displayed along with the score
          lcd.clear();
          lcd.setCursor (0, 0);
          lcd.print ("You Win! :) ");
          lcd.setCursor (0, 1);
          lcd.print (menuOptions[3]);
          lcd.print (score);
          lc.clearDisplay (0);
          buttonState = 1;
          defaultButtonState = 1;
          while (buttonState == defaultButtonState)
            buttonState = digitalRead (buttonPin);
          buttonState = 1;
          defaultButtonState = 0;
          reseter = 1;
        }
        else if (level < 3) {
          //if a lower level is finished, the next one is generated and the game continues
          levelGenerate (level);
          level++;
          if (difficulty > 1) {
            //for difficulties 2 and 3, for each finished level, the player gets an extra life
            lives++;
            lcd.clear ();
            lcd.setCursor (0, 0);
            lcd.print (menuOptions[4]);
            for (int i = 0; i < lives; i++)
              lcd.print ("o ");
            lcd.setCursor (0, 1);
            lcd.print (menuOptions[3]);
            lcd.print (score);
          }
        }
      }
      //if the ball hits the board, it changes direction accordingly
      if (x == mid - 2 && ball.getBallDirection() == -3) 
        ball.setBallDirection (1);
      else if (x == mid - 1) 
        ball.setBallDirection (1);
      else if (x == mid + 1) 
        ball.setBallDirection (3);
      else if (x == mid)
        ball.setBallDirection (2);
      else if (x == mid + 2 && ball.getBallDirection() == -1) 
        ball.setBallDirection (3);
      //else a life is lost
      else {
        lives--;
        //if there are lives left, game continues
        if (lives > 0) {
          lc.setLed (0, 7, mid - 1, false);
          lc.setLed (0, 7, mid, false);
          lc.setLed (0, 7, mid + 1, false);
          board.setBoardMiddle (3);
          start = true;
          lc.setLed (0, 6, x, false);
          lc.setLed (0, 6, 3, true);
          lc.setLed (0, 7, 2, true);
          lc.setLed (0, 7, 3, true);
          lc.setLed (0, 7, 4, true);
          ball.setBallX (3);
          ball.setBallY (6);
          ball.setBallDirection (0);
        }
        //else, the game is lost
        else {
          lcd.clear();
          lcd.setCursor (0, 0);
          lcd.print (menuOptions[4]);
          lcd.setCursor (0, 1);
          lcd.print (menuOptions[3]);
          lcd.print (score);
          lc.clearDisplay (0);
          buttonState = 1;
          defaultButtonState = 1;
          while (buttonState == defaultButtonState)
            buttonState = digitalRead(buttonPin);
          buttonState = 1;
          defaultButtonState = 0;
          reseter = 1;
        }
      }
    }
    void levelGenerate (short level) {
      //sets the matrix for a new level
      switch (level) {
        case 1:
          for (int i = 1; i < 7; i++) {
            bricks[0][i] = 1;
            lc.setLed (0, 0, i, true);
            bricks[1][i] = 1;
            lc.setLed (0, 1, i, true);
          }
          bricks[2][0] = 1;
          bricks[2][1] = 1;
          bricks[2][3] = 1;
          bricks[2][4] = 1;
          bricks[2][6] = 1;
          bricks[2][7] = 1;
          bricks[3][0] = 1;
          bricks[3][7] = 1;
          lc.setLed (0, 2, 0, true);
          lc.setLed (0, 2, 1, true);
          lc.setLed (0, 2, 3, true);
          lc.setLed (0, 2, 4, true);
          lc.setLed (0, 2, 6, true);
          lc.setLed (0, 2, 7, true);
          lc.setLed (0, 3, 0, true);
          lc.setLed (0, 3, 7, true);
          countBricks = 20;
          break;

        case 2:
          bricks[0][3] = 1;
          bricks[0][4] = 1;
          bricks[1][1] = 1;
          bricks[1][6] = 1;
          for (int i = 2; i <= 5; i++) {
            bricks[2][i] = 1;
            bricks[3][i] = 1;
            lc.setLed (0, 2, i, true);
            lc.setLed (0, 3, i, true);
          }
          bricks[3][0] = 1;
          bricks[3][7] = 1;
          bricks[4][0] = 1;
          bricks[4][3] = 1;
          bricks[4][4] = 1;
          bricks[4][7] = 1;

          lc.setLed (0, 0, 3, true);
          lc.setLed (0, 0, 4, true);
          lc.setLed (0, 1, 1, true);
          lc.setLed (0, 1, 6, true);
          lc.setLed (0, 3, 0, true);
          lc.setLed (0, 3, 7, true);
          lc.setLed (0, 4, 0, true);
          lc.setLed (0, 4, 3, true);
          lc.setLed (0, 4, 4, true);
          lc.setLed (0, 4, 7, true);
          countBricks = 18;
          break;
      }
    }
};

brickBreaker* game = new brickBreaker;

void setup() {
  Serial.begin (9600);
  pinMode (buttonPin, INPUT_PULLUP);
  lc.shutdown (0, false); // turn off power saving, enables display
  lc.setIntensity (0, 6); // sets brightness (0~15 possible values)
  lc.clearDisplay (0);// clear screen
  lcd.begin (16, 2);
}

void loop() {
  if (reseter == 1) {
    delete game;
    game = new brickBreaker;
    reseter = 0;
  }
  game->mainMenu();
}
