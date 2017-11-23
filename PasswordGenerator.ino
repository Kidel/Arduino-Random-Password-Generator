#include <Keyboard.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Set a button to any pin
#define buttonPinLeft 7
#define buttonPinRight 8

#define SSD1306_LCDHEIGHT 32

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


int status_main = 1;
int status_randomize = 0;
int status_symbol_choice = 0;
int status_generated = 0;

unsigned long seed = 0;
int modifier = 0;

char* letters = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char* symbols = "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ!$%^@#*.:;,?-_<>=";

char* rndstr;


void setup()
{
  pinMode(buttonPinLeft, INPUT);  
  pinMode(buttonPinRight, INPUT);  
  digitalWrite(buttonPinLeft, HIGH);  // Pull the button high
  digitalWrite(buttonPinRight, HIGH);  // Pull the button high

  delay(2000);
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(100);

  // Clear the buffer.
  display.clearDisplay();
  display.display();
  
  
}

void loop()
{
  if(status_main){
    displayMenuItem("Password generator", "Generate password?", "", "Go");
    if(digitalRead(buttonPinRight) == 0) {
      status_main = 0;
      status_randomize = 1;
      delay(600);
    }
  }
  else if(status_randomize) {
    if(seed == 0) displayMenuItem("Seed missing", "Press X to generate", "X", "");
    else displayMenuItem("Seed generated", "Press X to improve", "X", "Generate");
    if(seed != 0 && digitalRead(buttonPinRight) == 0) {
      status_randomize = 0;
      status_symbol_choice = 1;
      randomSeed(seed); // initialize the RNG
      delay(600);
    }
    if(digitalRead(buttonPinLeft) == 0) {
      if(!modifier){
        seed++;
        modifier = 1;
      } 
      else {
        seed = seed * 3;
        modifier = 0;
      }
      Serial.println(seed);
    }
  }
  else if(status_symbol_choice) {
    displayMenuItem("Password generator", "Use symbols?", "Yes", "No");
    if(digitalRead(buttonPinRight) == 0 || digitalRead(buttonPinLeft) == 0){
      status_symbol_choice = 0;
      status_generated = 1;
    }
    if(digitalRead(buttonPinRight) == 0) {
      rndstr = randomString(0);
      Serial.println(rndstr);
      delay(600);
    }
    if(digitalRead(buttonPinLeft) == 0) {
      rndstr = randomString(1);
      Serial.println(rndstr);
      delay(600);
    }
  }
  else if(status_generated) {
    displayMenuItem("Password generated!", "Write as keybord?", "Copy (ENG)", "Reset");
    if(digitalRead(buttonPinRight) == 0) {
      status_main = 1;
      status_generated = 0;
      seed = 0;
      delay(600);
    }
    if(digitalRead(buttonPinLeft) == 0) {
      Serial.println("writing password...");
      Serial.println(rndstr);
      Keyboard.println(rndstr);
      delay(2000);
    }
  }
  delay(10);
}


void displayMenuItem(char* question, char* text, char* choice1, char* choice2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(question);

  display.setCursor(0,SSD1306_LCDHEIGHT - 20); 
  display.println(text);

  display.drawLine(0, SSD1306_LCDHEIGHT - 23, 128, SSD1306_LCDHEIGHT - 23, WHITE);
  display.drawLine(0, SSD1306_LCDHEIGHT - 11, 128, SSD1306_LCDHEIGHT - 11, WHITE);

  display.setCursor(0,SSD1306_LCDHEIGHT - 8); 
  display.println(choice1);

  display.setCursor(128-((strlen(choice2)*6)+1),SSD1306_LCDHEIGHT - 8); 
  display.println(choice2);
  display.display();
}

void displayTestMenu() {
  displayMenuItem("Hello World!", "The world is nice!", "yes", "no");
}

void displayMainMenu() {
  displayMenuItem("Password generator", "Generate password?", "", "Go");
}

char randomChar() {
   return letters[random(0, strlen(letters)-1)];
}

char randomSymbol() {
   return symbols[random(0, strlen(symbols)-1)];
}

char* randomString(int useSymbols) {
  int i = 0;
  char buf[257];
  while(i<256) {
    buf[i] = ( useSymbols ? randomSymbol() : randomChar() );
    i++;
  }
  buf[256]='\0';
  return buf;
}

