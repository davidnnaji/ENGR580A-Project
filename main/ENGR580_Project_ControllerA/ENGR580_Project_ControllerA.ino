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
//uint32_t previousMillis2=0;
//uint32_t currentMillis2=0;

//Heartbeat Message
const int messageLength = 32;
uint8_t message[messageLength] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32}; //Debug

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

  //Softer startup before joining
  delay(3000);
  J1708Tx(message,messageLength,89);
  
}

void loop() {
  if (RxListen){
    J1708Listen();
    if (J1708LoopTimer>P){
      if (fx!=0){
        switch(fx){
          case 1:
          RTS_Handler(Loopbuffer);
            break;
          case 2:
          CTS_Handler(Loopbuffer);
            break;
          case 3:
          EOM_Handler(Loopbuffer);
            break;
          case 4:
          Abort_Handler(Loopbuffer);
            break;
          case 5:
          CDP_Handler(Loopbuffer);
            break;
          default:
            break;
        }
        fx=0;
        P =2000;
        Loop_flag = false;
      }
      else{
        P = 1000;
      }
      J1708LoopTimer = 0;
    }
    else{
      if (fx!=0){
        //Wait Until next scheduled send.
      }
      else{
        fx = parseJ1708(Loopbuffer);
        if (fx>0){
          Loop_flag = true;
        }
        else{
          Loop_flag=false;
        }
      }    
    }
  }

/*  const int messageLength2 = random(1,20);
  uint8_t message2[messageLength2] = {};
  for (int i=0; i<messageLength2-1; i++){
    message2[i] = random(0,255);
  }
  J1708Tx(message2,messageLength2,8);
  delay(2000);*/
}
