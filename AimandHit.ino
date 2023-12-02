#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C // 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int sda = 21;
const int scl = 22;
int UP_PIN = 19;
int DOWN_PIN = 5;
int MID_PIN = 16;
int btn_up_value = 0;
int btn_down_value = 0;
int btn_mid_value = 0;
// Variables will change:
int midButtonState;            // the current reading from the input pin
int midLastButtonState = LOW;  // the previous reading from the input pin
int upButtonState;            // the current reading from the input pin
int upLastButtonState = LOW;  // the previous reading from the input pin
int downButtonState;            // the current reading from the input pin
int downLastButtonState = LOW;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long midLastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long upLastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long downLastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
// time
int gameStartTiming = 0;
int gameOverTiming = 0;

enum Screen{
  HOME, SELECTLEVEL, GAMESTART, GAMEOVER, GAMEDURATION
};

enum Screen currentScreen = HOME;
int currentLevel = 1;

void setup() {
  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  // Joystick stuff
  Serial.begin(9600);
  pinMode(UP_PIN, INPUT);
  pinMode(DOWN_PIN, INPUT);
  pinMode(MID_PIN, INPUT);

}

void loop() {
  //To-do
  // press button to go to level selection screen
  // display "Level 1 (10)/ Level 2 (15)", user selects with joystick
  // display target pixels at random on OLED screen + the number of targets that are hit
  // arduino to read X-Y axis angle data from i2c 
  // show time taken
  readInputs();
  gameEngineLoop();  
  delay(10);
  // Serial.print("Value of the UP Pin: ");
  // Serial.print(btn_up_value);

  // Serial.print("Value of the DOWN Pin: ");
  // Serial.println(btn_down_value);

  // delay(10);
  // // OLED stuff
  // display.drawPixel(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, WHITE);
  // display.display();

  // delay(10);
}

void gameEngineLoop(){
  if(currentScreen == HOME){
    homeScreen();
  }
  else if(currentScreen == SELECTLEVEL){
    selectLevelScreen();
  }
  else if(currentScreen == GAMESTART){
    gameStartScreen();
  }
  else if(currentScreen == GAMEOVER){
    gameOverScreen();
  }
  else if(currentScreen == GAMEDURATION){
    gameDurationScreen();
  }
}

void readInputs() {
  //   MID PIN
  btn_mid_value = 0;
  int reading = digitalRead(MID_PIN);
  if (reading != midLastButtonState) {
    midLastDebounceTime = millis();
  }
   if ((millis() - midLastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != midButtonState) {
      midButtonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (midButtonState == HIGH) {
        Serial.println("MID!");
        btn_mid_value = 1;
      }
    }
   }
     // save the reading. Next time through the loop, it'll be the lastButtonState:
  midLastButtonState = reading;

  // UP PIN
  btn_up_value = 0;
  reading = digitalRead(UP_PIN);
  if (reading != upLastButtonState) {
    upLastDebounceTime = millis();
  }
   if ((millis() - upLastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != upButtonState) {
      upButtonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (upButtonState == HIGH) {
        Serial.println("UP!");
        btn_up_value = 1;
      }
    }
   }
     // save the reading. Next time through the loop, it'll be the lastButtonState:
  upLastButtonState = reading;

  // DOWN PIN
  btn_down_value = 0;
  reading = digitalRead(DOWN_PIN);
  if (reading != downLastButtonState) {
    downLastDebounceTime = millis();
  }
   if ((millis() - downLastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != downButtonState) {
      downButtonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (downButtonState == HIGH) {
        Serial.println("DOWN!");
        btn_down_value = 1;
      }
    }
   }
     // save the reading. Next time through the loop, it'll be the lastButtonState:
  downLastButtonState = reading;

}
void homeScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Let's Play");
  display.println("A Game.");
  display.println("<PRESS");
  display.println("START>");

  display.display();

  if (btn_mid_value == 1){
    currentScreen = SELECTLEVEL;
    delay(200);
  }
}

void selectLevelScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Select");
  display.println("Level:");
  display.println(String(currentLevel));
  display.display();

  if (btn_up_value == 1){
    currentLevel = 2;
  }

  else if (btn_down_value == 1){
    currentLevel = 1;
  }
  
  else if (btn_mid_value == 1){
    currentScreen = GAMESTART;
  }

}

void gameStartScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Game");
  display.println("Start!");
  display.display();
  gameStartTiming = millis();
  currentScreen = GAMEOVER;
  delay(2000);
}

void gameOverScreen(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Game");
  display.println("Over!");
  display.display();
  gameOverTiming = millis();
  currentScreen = GAMEDURATION;
  delay(2000);
}

void gameDurationScreen(){
  int elapsedTiming = (gameOverTiming - gameStartTiming) / 1000;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("You took");
  display.print(String(elapsedTiming));  
  display.print(" seconds");
  display.display();
}
