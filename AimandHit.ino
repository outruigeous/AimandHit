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
int address = 0x68;
int UP_PIN = 19;
int DOWN_PIN = 5;
int MID_PIN = 16;
int btn_up_value = 0;
int btn_down_value = 0;
int btn_mid_value = 0;

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
  HOME, SELECTLEVEL, GAMESTART, GAME, GAMEOVER, GAMEDURATION
};

enum Screen currentScreen = HOME;
int currentLevel = 1;

// game related memory

int targetCoordinates[20][2];

int playerCoordinates[2];

// int for Gyrometer
byte GYRO_XOUT_H = 0;
byte GYRO_XOUT_L = 0;
byte GYRO_YOUT_H = 0;
byte GYRO_YOUT_L = 0;
int16_t GYRO_X_RAW = 0;
int16_t GYRO_Y_RAW = 0;
int16_t gyroX_Per_S = 0;
int16_t gyroY_Per_S = 0;
float elapsedTime = 0.0;
float currentTime = 0.0;
float previousTime = 0.0;
float oldGyroAngleX = 0.0;
float newGyroAngleX = 0.0;
float oldGyroAngleY = 0.0;
float newGyroAngleY = 0.0;
// gyro calibration
float calibrationOffsetX = 0.0;
float calibrationOffsetY = 0.0;

void setup() {

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();

  // Joystick stuff
  Serial.begin(115200);
  pinMode(UP_PIN, INPUT);
  pinMode(DOWN_PIN, INPUT);
  pinMode(MID_PIN, INPUT);

  // random number generator
  randomSeed(analogRead(0));

  // gyrometer stuff
   Wire.begin(sda,scl); //to call the SDA and SCL
    Serial.println("Calibration Start!");
    for (int i = 0; i < 100; i ++) { //calibrate
        readGyrometer();
        calibrationOffsetX += gyroX_Per_S;
        calibrationOffsetY += gyroY_Per_S;
        
        delay(50);
    }
    // Calculate the average calibration offsets
    calibrationOffsetX /= 100.0;
    calibrationOffsetY /= 100.0;

    Serial.println("Calibration End");
    Serial.println(calibrationOffsetX);
    Serial.println(calibrationOffsetY);
    
    oldGyroAngleX = 0.0;
    oldGyroAngleY = 0.0;
}

void loop() {
  //To-do
  // press button to go to level selection screen
  // display "Level 1 (10)/ Level 2 (15)", user selects with joystick
  // display target pixels at random on OLED screen + the number of targets that are hit
  // arduino to read X-Y axis angle data from i2c 
  // show time taken
  readInputs();
  readGyrometer();
  printAngle();
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
  else if(currentScreen == GAME){
    gameScreen();
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
  
  playerCoordinates[0] = 64;
  playerCoordinates[1] = 32;
  
  for (int i = 0; i < 20; i++) {
    if (currentLevel == 1){
      if (i < 10){
        generateTarget(i);
      }
      else {
        deleteTarget(i);
      }
    }
    else {
      generateTarget(i);
    }

  }
  
  gameStartTiming = millis();
  currentScreen = GAME;
  delay(2000);
}

void generateTarget(int i){
  targetCoordinates[i][0] = random(128);
  targetCoordinates[i][1] = random(64);
  Serial.print("Print Target Coordinates: ");
  Serial.print(targetCoordinates[i][0]);
  Serial.print(",");
  Serial.println(targetCoordinates[i][1]);
}

void deleteTarget(int i){
  targetCoordinates[i][0] = -1;
  targetCoordinates[i][1] = -1;
}

void gameScreen(){
  display.clearDisplay();
  display.fillCircle(playerCoordinates[0], playerCoordinates[1], 2, WHITE);
   for (int i = 0; i < 20; i++) {
    display.drawPixel(targetCoordinates[i][0], targetCoordinates[i][1], WHITE);
   }
  display.display();
  if (newGyroAngleX > 10){
    playerCoordinates[0] = max(0, playerCoordinates[0]-1);
  }
  if (newGyroAngleX < -10){
    playerCoordinates[0] = min(127, playerCoordinates[0]+1);
  }
  if (newGyroAngleY > 10){
    playerCoordinates[1] = min(63, playerCoordinates[1]+1);
  }
  if (newGyroAngleY < -10){
    playerCoordinates[1] = max(0, playerCoordinates[1]-1);
  }
  int deletedTargets = 0; // counter for deleted targets

  for (int i = 0; i < 20; i++) {
    if (collidingCoordinates(i)){
      deleteTarget(i);
    }
    if (targetIsDeleted(i)){
      deletedTargets++;
    }
  }
  if (deletedTargets == 20){ // if targets are all gone, move to the next screen
    currentScreen = GAMEOVER;
  }
}

bool collidingCoordinates(int i){
  return abs(playerCoordinates[0] - targetCoordinates[i][0]) <= 2 && abs(playerCoordinates[1] - targetCoordinates[i][1]) <= 2; // check if playerCoordinates - targetCoordinates is within the range of +/- 1
}

bool targetIsDeleted(int i){ // i represents the "i" target that is deleted
  return targetCoordinates[i][0] == -1 && targetCoordinates[i][1] == -1; // check if targetCoordinate is -1
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
  display.println(" seconds");
  display.println(" ");
  display.println("<BACK>");
  display.display();

  if (btn_mid_value == 1){
    currentScreen = HOME;
    delay(200);
  }
}

void readGyrometer(){
  Wire.beginTransmission(address);
  Wire.write(0x43);
  Wire.endTransmission();
  // read answer to request from 43
  Wire.requestFrom(address, 6);
  GYRO_XOUT_H = Wire.read();
  GYRO_XOUT_L = Wire.read();
  GYRO_YOUT_H = Wire.read();
  GYRO_YOUT_L = Wire.read();

  GYRO_X_RAW = GYRO_XOUT_H << 8 | GYRO_XOUT_L;
  GYRO_Y_RAW = GYRO_YOUT_H << 8 | GYRO_YOUT_L;

  gyroX_Per_S = GYRO_X_RAW/131.0; 
  gyroY_Per_S = GYRO_Y_RAW/131.0;

  currentTime = millis();
  elapsedTime = (currentTime - previousTime) / 1000;
  previousTime = currentTime;

  newGyroAngleX = oldGyroAngleX + (gyroX_Per_S - calibrationOffsetX) * elapsedTime;
  newGyroAngleY = oldGyroAngleY + (gyroY_Per_S - calibrationOffsetY) * elapsedTime;

  oldGyroAngleX = newGyroAngleX;
  oldGyroAngleY = newGyroAngleY;
}

void printAngle(){
  Serial.print("Angle X-axis:");
  Serial.print(newGyroAngleX);
  Serial.print(",");
  Serial.print ("Angle Y-axis:");
  Serial.println(newGyroAngleY);

}
