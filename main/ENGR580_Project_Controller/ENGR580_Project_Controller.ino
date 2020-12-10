/*
 * ENGR580_Project_Controller.ino
 * by David Nnaji
 * 
 * Description:
 * Main program for ENGR580A Final Project.
 * Compatible for Teensy 4.0 only.
 * 
 */

 //Libraries
#include "J1708.h"

uint32_t previousMillis=0;
uint32_t currentMillis=0;

//Heartbeat Message
//const int messageLength = 5;
//uint8_t message[messageLength] = {TractorMID,255,255,1}; //Debug

void setup() {
  //LED1 Pin Configuration
  pinMode(LEDPin1,OUTPUT);
  digitalWrite(LEDPin1,LOW);

  //LED2 Pin Configuration
  pinMode(LEDPin2,OUTPUT);
  digitalWrite(LEDPin2,LOW);

  //Rx Enable Pin Configurations
  RxEnable();
  
  //The J1708 tranceiver is connected to UART3
  Serial.begin(9600);
  Serial3.begin(9600);
}

void loop() {
  if (RxListen){
    J1708Listen();
  }

  currentMillis=millis();
  if ((currentMillis-previousMillis)>1500){
    //J1708Tx(message,messageLength,8);
    genECCKeys(selfMID);
    previousMillis=currentMillis;
  }

}
