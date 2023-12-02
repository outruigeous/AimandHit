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

void readInputs(){
    btn_up_value = digitalRead(UP_PIN);
    btn_down_value = digitalRead(DOWN_PIN);
    btn_mid_value = digitalRead(MID_PIN);
    Serial.print("pins: ");
    Serial.print(btn_up_value);
    Serial.print(" ");
    Serial.print(btn_down_value);
    Serial.print(" ");
    Serial.println(btn_mid_value);
    delay(20);
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
  display.print("seconds");
  display.display();
}
