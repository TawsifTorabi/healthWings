//ibrary from me-no-dev
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESP32Servo.h>
#include <ESPAsyncWebServer.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


const char *ssid = "ssid";
const char *password = "password";


//PIR
int pirLED = 15;
int pirSensor = 34;
bool pirActivated = false;

//DFplayer
int DFplayerRX = 4;
int DFplayerTX = 2;


//0.96 OLED
int oledSCK = 2;
int oledSDA = 4;

//Door 1
Servo servo1;
int servo1pin = 14;
int LEDred1 = 27;
int LEDgreen1 = 26;

//Door 2
Servo servo2;
int LEDred2 = 19;
int LEDgreen2 = 33;
int servo2pin = 12;

//Door 3
Servo servo3;
int LEDred3 = 32;
int LEDgreen3 = 5;
int servo3pin = 13;

int volume = 20;


#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/DFplayerTX, /*tx =*/DFplayerRX);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);



AsyncWebServer server(80);
AsyncWebSocket ws("/remote");

unsigned long lastMessageTime = 0;
const unsigned long messageTimeout = 500; // 2 seconds


void notifyClients() {
  ws.textAll(String("Notification"));
}


//Switch ON or OFF LED
void toggleLED(int ledPin) {
  if(digitalRead(ledPin) == HIGH){
    digitalWrite(ledPin, LOW);
  }else{
    digitalWrite(ledPin, HIGH);
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        const char *message = (char *)data;
        Serial.println(message);

       if (strncmp(message, "door:", 5) == 0) {
        char door[2];
        int state;

        if (sscanf(message + 5, "%1[^,],state:%d", door, &state) == 2) {
            if (strcmp(door, "1") == 0) {
                if (state == 1) {
                    toggleLED(LEDred1);
                    toggleLED(LEDgreen1);
                    myDFPlayer.play(2);
                    servo1.write(0); //open
                    ws.textAll("info: door, key:1, val:1;");
                    
                    display.clearDisplay();
                    display.setTextSize(1);
                    display.setTextColor(WHITE);
                    display.setCursor(0, 10);
                    // Display static text
                    display.println("Please Collect from");
                    display.setCursor(0, 30);
                    display.setTextSize(2);
                    display.println("Box 1");
                    display.display(); 

                } else if (state == 0) {
                    toggleLED(LEDred1);
                    toggleLED(LEDgreen1);
                    servo1.write(90); //close
                    ws.textAll("info: door, key:1, val:0;");

                    display.clearDisplay();
                    display.setTextSize(3);
                    display.setTextColor(WHITE);
                    display.setCursor(0, 10);
                    display.println("THANK");
                    display.setCursor(0, 35);
                    display.println("YOU!");
                    display.display(); 

                    pirActivated = false;

                }
            } else if (strcmp(door, "2") == 0) {
                if (state == 1) {
                    toggleLED(LEDred2);
                    toggleLED(LEDgreen2);
                    myDFPlayer.play(3);
                    servo2.write(0); //open
                    ws.textAll("info: door, key:2, val:1;");

                    display.clearDisplay();
                    display.setTextSize(1);
                    display.setTextColor(WHITE);
                    display.setCursor(0, 10);
                    // Display static text
                    display.println("Please Collect from");
                    display.setCursor(0, 30);
                    display.setTextSize(2);
                    display.println("Box 2");
                    display.display(); 

                } else if (state == 0) {
                    toggleLED(LEDred2);
                    toggleLED(LEDgreen2);
                    servo2.write(90); //close
                    ws.textAll("info: door, key:2, val:0;");

                    display.clearDisplay();
                    display.setTextSize(3);
                    display.setTextColor(WHITE);
                    display.setCursor(0, 10);
                    display.println("THANK");
                    display.setCursor(0, 35);
                    display.println("YOU!");
                    
                    display.display(); 

                    pirActivated = false;
                }
            } else if (strcmp(door, "3") == 0) {
                if (state == 1) {
                    toggleLED(LEDred3);
                    toggleLED(LEDgreen3);
                    myDFPlayer.play(4);
                    servo3.write(0); //open
                    ws.textAll("info: door, key:3, val:1;");

                    display.clearDisplay();
                    display.setTextSize(1);
                    display.setTextColor(WHITE);
                    display.setCursor(0, 10);
                    // Display static text
                    display.println("Please Collect from");
                    display.setCursor(0, 30);
                    display.setTextSize(2);
                    display.println("Box 3");
                    display.display(); 
                } else if (state == 0) {
                    toggleLED(LEDred3);
                    toggleLED(LEDgreen3);
                    servo3.write(90); //close
                    ws.textAll("info: door, key:3, val:0;");

                    display.clearDisplay();
                    display.setTextSize(3);
                    display.setCursor(0, 10);
                    display.println("THANK");
                    display.setCursor(0, 35);
                    display.println("YOU!");
                    display.display(); 

                    pirActivated = false;
                }
            }
        }
      } else if (strcmp(message, "closeAll") == 0) {
          closeAllDoors();
          pirActivated = false;
          ws.textAll(String("info: door, key:alldoor, val: 0;"));
      } else if (strcmp(message, "openAll") == 0) {
          openAllDoors();
          pirActivated = false;
          ws.textAll(String("info: door, key:alldoor, val: 1;"));
      } else if (strcmp(message, "reset") == 0) {
          closeAllDoors();
          pirActivated = false;
          ws.textAll(String("info: reset, key:state, val: 1;"));
      }
    }
}







void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


void closeAllDoors(){
  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  digitalWrite(LEDred1, HIGH);
  digitalWrite(LEDred2, HIGH);
  digitalWrite(LEDred3, HIGH);
  digitalWrite(LEDgreen1, LOW);
  digitalWrite(LEDgreen2, LOW);
  digitalWrite(LEDgreen3, LOW);
}

void openAllDoors(){
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  digitalWrite(LEDred1, LOW);
  digitalWrite(LEDred2, LOW);
  digitalWrite(LEDred3, LOW);
  digitalWrite(LEDgreen1, HIGH);
  digitalWrite(LEDgreen2, HIGH);
  digitalWrite(LEDgreen3, HIGH);
}



void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
  }
  delay(2000);


  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("System Started!");
  display.display(); 


//DF Player Initials
  #if (defined ESP32)
    FPSerial.begin(9600, SERIAL_8N1, /*rx =*/DFplayerTX, /*tx =*/DFplayerRX);
  #else
    FPSerial.begin(9600);
  #endif

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  
  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online.")); 
  myDFPlayer.volume(volume);  //Set volume value. From 0 to 30
//DF Player End


  pinMode(LEDred1, OUTPUT);
  pinMode(LEDgreen1, OUTPUT);
  pinMode(LEDred2, OUTPUT);
  pinMode(LEDgreen2, OUTPUT);
  pinMode(LEDred3, OUTPUT);
  pinMode(LEDgreen3, OUTPUT);
  pinMode(LEDgreen3, OUTPUT);
  pinMode(pirLED, OUTPUT);
  pinMode(pirSensor, INPUT);

  servo1.attach(servo1pin);
  servo2.attach(servo2pin);
  servo3.attach(servo3pin);
  
  closeAllDoors();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(">> Connected to WiFi");
  display.setCursor(0, 20);
  display.println(WiFi.localIP());
  display.display(); 

  initWebSocket();
  server.begin();

}

void loop() {

  ws.cleanupClients();

  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }

  if(digitalRead(pirSensor) == HIGH){
    if(pirActivated == false){
      pirActivated = true;
      digitalWrite(pirLED, HIGH);
      myDFPlayer.play(1);
      ws.textAll(String("info: pir, key:proximity, val: 1;"));
       display.setTextSize(1);
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Hello!");
      display.setCursor(0, 20);
      display.println("Scan The QR Code.");
      display.display(); 
    }
  }
}


void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}
