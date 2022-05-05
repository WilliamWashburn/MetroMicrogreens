#define BLYNK_TEMPLATE_ID "TMPLiCRc-E4J"
#define BLYNK_DEVICE_NAME "Misting System 10 Control Zones Intervals"
#define BLYNK_AUTH_TOKEN "3Z5Coxx_DMbGZfztXoHA7KAE_qHalgGU"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

#include <WidgetRTC.h> //used for updating the time
#include "customTimer.h"
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "FiOS-QQN0S";
//char pass[] = "zoo7678rip7105gift";
char ssid[] = "Washburn Wifi";
char pass[] = "wandacat";

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial3

// or Software Serial on Uno, Nano...
//#include <SoftwareSerial.h>
//SoftwareSerial EspSerial(2,3); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600 //I had to set this with a command to the ESP

ESP8266 wifi(&EspSerial);

WidgetRTC rtc;

//int solPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; //pins to toggle the solenoids
int solPins[] = {3, 3, 3, 3, 3, 3, 3, 3, 3, 3}; //pins to toggle the solenoids

//each solenoid will be given a letter: [A,B,C,D,E,F,G,H,I,J]
//the pin given to each solenoid is dependant on the pins position in the solPins array
//solPins[0] <--> A, solPins[1] <--> B, etc

const int nmbrOfSolenoids = 10;
long intervals[nmbrOfSolenoids];
long durations[nmbrOfSolenoids]; //holds duration of each event;

int lastConnectionAttempt = millis();
int connectionDelay = 5000; // try to reconnect every 5 seconds

int nmbrOfTimers = 10;
customTimer timers[10];

int lastprinted = 0;

//**setup**
//

//**loop**

BLYNK_CONNECTED() {
  rtc.begin(); // Synchronize time on connection

  //    Blynk.syncAll(); //calls all BLYNK_WRITE for every virtual pin (that has sync setting turned on). Causes buffer overflow with Mega+ESP8266 shield:(
  //  syncPins(); //for some reason, if i called this here, I would get one extra timer and Solenoid A would go off twice in a row
}

void syncPins() {
  //This doesnt cause a buffer overflow but Im not going to type out 202 pins
  int syncPins[] = {V0, V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11, V12, V13, V14, V15, V16, V17, V18, V19, V20, V21, V22, V23, V24, V25, V26, V27, V28, V29, V30};

  for (int i = 0; i < sizeof(syncPins) / sizeof(int); i++) {
    Blynk.syncVirtual(syncPins[i]);
    delay(1); //avoids buffer overflow
  }
}

void setup()
{
  // Debug console
  Serial.begin(9600);

//  pinMode(2, OUTPUT);
//  pinMode(3, OUTPUT);
//  pinMode(4, OUTPUT);
//  pinMode(5, OUTPUT);
//  pinMode(6, OUTPUT);
//  pinMode(7, OUTPUT);
//  pinMode(8, OUTPUT);
//  pinMode(9, OUTPUT);
//  pinMode(10, OUTPUT);
//  pinMode(11, OUTPUT);
//
//  digitalWrite(2, HIGH);
//  digitalWrite(3, HIGH);
//  digitalWrite(4, HIGH);
//  digitalWrite(5, HIGH);
//  digitalWrite(6, HIGH);
//  digitalWrite(7, HIGH);
//  digitalWrite(8, HIGH);
//  digitalWrite(9, HIGH);
//  digitalWrite(10, LOW); //The relay would have to be sent a 9v signal to be HIGH. So 5v and 0v are considered low to the solenoid. Instead, we will trigger on low. The 8 module relay only triggers on LOW
//  digitalWrite(11, LOW);

  delay(10);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  //connect to wifi
  Blynk.begin(auth, wifi, ssid, pass);
  delay(30);
  if (Blynk.connected() == false) {
    Serial.println("didint connect. reconnecting");
    Blynk.connect();
  }

  setSyncInterval(10 * 60); //sync interval for syncing clock

  syncPins();

  //set maintenance timers
  //  timer.setInterval(1000, printScheduleAndDurations); //set timer to print debug info
  //  timer.setInterval(1000, updateDisplayedTime); //set timer to update the displayed time on the app

  //set solenoid timers
  timers[0] = customTimer(intervals[0], durations[0]);
  timers[1] = customTimer(intervals[1], durations[1]);
  timers[2] = customTimer(intervals[2], durations[2]);
  timers[3] = customTimer(intervals[3], durations[3]);
  timers[4] = customTimer(intervals[4], durations[4]);
  timers[5] = customTimer(intervals[5], durations[5]);
  timers[6] = customTimer(intervals[6], durations[6]);
  timers[7] = customTimer(intervals[7], durations[7]);
  timers[8] = customTimer(intervals[8], durations[8]);
  timers[9] = customTimer(intervals[9], durations[9]);

  Blynk.logEvent("device_connected");
}

void loop()
{
  // check WiFi connection:
  if (Blynk.connected() == false) {
    // (optional) "offline" part of code
    Serial.println("Connection Lost");

    // check delay:
    if (millis() - lastConnectionAttempt >= connectionDelay) {
      lastConnectionAttempt = millis();
      Serial.println("reconnecting...");

      //try to reconnect
      Blynk.connect();
    }
  }
  else
  {
    Blynk.run();
    for (int i = 0; i < nmbrOfTimers; i++) {
      if (timers[i].check() == true) {
        triggerSolenoid(i);
      }
    }

    int nowTime = millis();
    if (nowTime - lastprinted > 1000) {
      lastprinted = nowTime;
      printScheduleAndDurations();
    }
  }
}

void updateDisplayedTime() {
  String timeString = String(hour()) + ":" + String(minute()) + ":" + String(second());
  Blynk.virtualWrite(V0, timeString);
}

void printScheduleAndDurations() {
  int maxVals = 10;
  int minVal = 0;

  Serial.print("intervals: ");
  for (int i = minVal; i < maxVals; i++) {
    Serial.print(intervals[i]); Serial.print(", ");
  }
  Serial.println();

  Serial.print("Durations: ");
  for (int i = minVal; i < maxVals; i++) {
    Serial.print(String(durations[i]) + ", ");
  }
  Serial.println();

  long currentTimeMilli = hour() * 3.6e6 + minute() * 60000 + second() * 1000;
  Serial.print("Current Time: "); Serial.println(currentTimeMilli);
  Serial.println();

}

BLYNK_WRITE(V0) {
  updateDisplayedTime();
}

void triggerA() {
  triggerSolenoid(0);
}

void triggerB() {
  triggerSolenoid(1);
}

void triggerC() {
  triggerSolenoid(2);
}

void triggerD() {
  triggerSolenoid(3);
}

void triggerE() {
  triggerSolenoid(4);
}

void triggerF() {
  triggerSolenoid(5);
}

void triggerG() {
  triggerSolenoid(6);
}

void triggerH() {
  triggerSolenoid(7);
}

void triggerI() {
  triggerSolenoid(8);
}

void triggerJ() {
  triggerSolenoid(9);
}


void triggerSolenoid(int solNmbr) {
  logSolenoidEvent(solNmbr);
  int pinNmbr = solPins[solNmbr];
  
  if (pinNmbr != 10 && pinNmbr != 11) {
    Serial.println("the solnmbr is: " + String(solNmbr) );
    digitalWrite(pinNmbr, LOW);
    delay(durations[solNmbr] * 1000);
    digitalWrite(pinNmbr, HIGH);
  }
  
  else {
    Serial.println("the solnmbr is: " + String(solNmbr) );
    digitalWrite(pinNmbr, HIGH);
    delay(durations[solNmbr] * 1000);
    digitalWrite(pinNmbr, LOW);
  }
}

void logSolenoidEvent(int solNmbr) {
  switch (solNmbr) {
    case 0:
      Blynk.logEvent("a_triggered", "Solenoid A was triggered for " + String(durations[0]) + " seconds");
      Serial.println("A_Solenoid() triggered");
      break;
    case 1:
      Blynk.logEvent("b_triggered", "Solenoid B was triggered for " + String(durations[1]) + " seconds");
      Serial.println("B_Solenoid() triggered");
      break;
    case 2:
      Blynk.logEvent("c_triggered", "Solenoid C was triggered for " + String(durations[2]) + " seconds");
      Serial.println("C_Solenoid() triggered");
      break;
    case 3:
      Blynk.logEvent("d_triggered", "Solenoid D was triggered for " + String(durations[3]) + " seconds");
      Serial.println("D_Solenoid() triggered");
      break;
    case 4:
      Blynk.logEvent("e_triggered", "Solenoid E was triggered for " + String(durations[4]) + " seconds");
      Serial.println("E_Solenoid() triggered");
      break;
    case 5:
      Blynk.logEvent("f_triggered", "Solenoid F was triggered for " + String(durations[5]) + " seconds");
      Serial.println("F_Solenoid() triggered");
      break;
    case 6:
      Blynk.logEvent("g_triggered", "Solenoid G was triggered for " + String(durations[6]) + " seconds");
      Serial.println("G_Solenoid() triggered");
      break;
    case 7:
      Blynk.logEvent("h_triggered", "Solenoid H was triggered for " + String(durations[7]) + " seconds");
      Serial.println("H_Solenoid() triggered");
      break;
    case 8:
      Blynk.logEvent("i_triggered", "Solenoid I was triggered for " + String(durations[8]) + " seconds");
      Serial.println("I_Solenoid() triggered");
      break;
    case 9:
      Blynk.logEvent("j_triggered", "Solenoid J was triggered for " + String(durations[9]) + " seconds");
      Serial.println("J_Solenoid() triggered");
      break;
  }
}

//A
void updateTimerA() {
  timers[0].changeInterval(intervals[0]);
}

BLYNK_WRITE(V1) { //update misting interval
  intervals[0] = param.asInt();
  updateTimerA();
}
BLYNK_WRITE(V11) { //update misting duration
  durations[0] = param.asInt();
}
BLYNK_WRITE(V21) { //trigger A
  if (param.asInt() == 1) {
    Serial.println("trigger A Now pressed");
    triggerSolenoid(0); //trigger solenoid
    Blynk.virtualWrite(V21, 0);
  }
}

//B
void updateTimerB() {
  timers[1].changeInterval(intervals[1]);
}
BLYNK_WRITE(V2) { //update misting interval
  intervals[1] = param.asInt();
  updateTimerB();
}
BLYNK_WRITE(V12) { //update misting duration
  durations[1] = param.asInt();
}
BLYNK_WRITE(V22) { //trigger
  if (param.asInt() == 1) {
    triggerSolenoid(1); //trigger solenoid
    Blynk.virtualWrite(V22, 0);
  }
}

//C
void updateTimerC() {
  timers[2].changeInterval(intervals[2]);
}
BLYNK_WRITE(V3) { //update misting interval
  intervals[2] = param.asInt();
  updateTimerC();
}
BLYNK_WRITE(V13) { //update misting duration
  durations[2] = param.asInt();
}
BLYNK_WRITE(V23) { //trigger
  if (param.asInt() == 1) {
    triggerSolenoid(2); //trigger solenoid
    Blynk.virtualWrite(V23, 0);
  }
}

//D
void updateTimerD() {
  timers[3].changeInterval(intervals[3]);
}
BLYNK_WRITE(V4) { //update misting interval
  intervals[3] = param.asInt();
  updateTimerD();
}
BLYNK_WRITE(V14) { //update misting duration
  durations[3] = param.asInt();
}
BLYNK_WRITE(V24) { //trigger
  if (param.asInt() == 1) {
    triggerSolenoid(3); //trigger solenoid
    Blynk.virtualWrite(V24, 0);
  }
}

//E
void updateTimerE() {
  timers[4].changeInterval(intervals[4]);
}
BLYNK_WRITE(V5) { //update misting interval
  intervals[4] = param.asInt();
  updateTimerE();
}
BLYNK_WRITE(V15) { //update misting duration
  durations[4] = param.asInt();
}
BLYNK_WRITE(V25) { //trigger
  if (param.asInt() == 1) {
    triggerSolenoid(4); //trigger solenoid
    Blynk.virtualWrite(V25, 0);
  }
}

//F
void updateTimerF() {
  timers[5].changeInterval(intervals[5]);
}
BLYNK_WRITE(V6) { //update misting interval
  intervals[5] = param.asInt();
  updateTimerF();
}
BLYNK_WRITE(V16) { //update misting duration
  durations[5] = param.asInt();
}
BLYNK_WRITE(V26) { //trigger
  if (param.asInt() == 1) {
    triggerSolenoid(5); //trigger solenoid
    Blynk.virtualWrite(V26, 0);
  }
}


//G
void updateTimerG() {
  timers[6].changeInterval(intervals[6]);
}
BLYNK_WRITE(V7) { //update misting interval
  intervals[6] = param.asInt();
  updateTimerG();
}
BLYNK_WRITE(V17) { //update misting duration
  durations[6] = param.asInt();
}
BLYNK_WRITE(V27) { //trigger
  if (param.asInt() == 1) {
    triggerSolenoid(6); //trigger solenoid
    Blynk.virtualWrite(V27, 0);
  }
}

//H
void updateTimerH() {
  timers[7].changeInterval(intervals[7]);
}
BLYNK_WRITE(V8) { //update misting interval
  intervals[7] = param.asInt();
  updateTimerH();
}
BLYNK_WRITE(V18) { //update misting duration
  durations[7] = param.asInt();
}
BLYNK_WRITE(V28) { //trigger
  if (param.asInt() == 1) {
    triggerSolenoid(7); //trigger solenoid
    Blynk.virtualWrite(V28, 0);
  }
}

//I
void updateTimerI() {
  timers[8].changeInterval(intervals[8]);
}
BLYNK_WRITE(V9) { //update misting interval
  intervals[8] = param.asInt();
  updateTimerI();
}
BLYNK_WRITE(V19) { //update misting duration
  durations[8] = param.asInt();
}
BLYNK_WRITE(V29) { //trigger
  if (param.asInt() == 1) {
    triggerSolenoid(8); //trigger solenoid
    Blynk.virtualWrite(V29, 0);
  }
}

//J
void updateTimerJ() {
  timers[9].changeInterval(intervals[9]);
}
BLYNK_WRITE(V10) { //update misting interval
  intervals[9] = param.asInt();
  updateTimerJ();
}
BLYNK_WRITE(V20) { //update misting duration
  durations[9] = param.asInt();
}
BLYNK_WRITE(V30) { //trigger
  if (param.asInt() == 1) {
    triggerSolenoid(9); //trigger solenoid
    Blynk.virtualWrite(V30, 0);
  }
}
