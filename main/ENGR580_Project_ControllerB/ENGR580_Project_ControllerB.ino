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
// const int messageLength2 = 5;
// uint8_t message2[messageLength2] = {0xDE,0xAD,0xBE,0xEF};
// const int messageLength3 = 4;
// uint8_t message3[messageLength3] = {0x5a,0xFE,0x51,0xDe};

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
    if (J1708LoopTimer>P){
      J1708LoopTimer = 0;
      if (fx>0){
        switch(fx){
          case 1:
            Serial.print("Loopbuffer: ");
            for (int i=0; i<9; i++){
            Serial.print(Loopbuffer[i],HEX);Serial.print(" ");
            }
            Serial.println();
            RTS_Handler(J1708RxBuffer);
            break;
          case 2:
            CTS_Handler(J1708RxBuffer);
            break;
          case 3:
            EOM_Handler(J1708RxBuffer);
            break;
          case 4:
            Abort_Handler(J1708RxBuffer);
            break;
          case 5:
            CDP_Handler(J1708RxBuffer);
            break;
          default:
            break;
        }
        fx=0;
        P = 2000;
        Loop_flag = false;
      }
      else{
        P = 1000;
      }
    }
    else{
      if (fx>0){
        //Wait Until next scheduled send.
      }
      else{
        fx = parseJ1708(J1708RxBuffer);
        if (fx>0){
          Loop_flag = true;
        }
        else{
          Loop_flag=false;
        }
      }    
    }
  }
}
