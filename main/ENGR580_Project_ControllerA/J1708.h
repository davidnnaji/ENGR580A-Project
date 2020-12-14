/*
 * J1708.h
 * by David Nnaji
 * 
 * Description:
 * A library with J1708 functions for ENGR580A Final Project.
 * Compatible for Teensy 4.0 only.
 * 
 */

//External Libraries
#include <EEPROM.h>

//Teensy 4.0 Pin Definitions
const int LEDPin1 = 13; //High-LEDON, Low-LEDOFF
const int LEDPin2 = 11; //High-LEDON, Low-LEDOFF
const int RDPin = 12; //Enables Tx or Rx on RS485 Tranceiver

//Flags
bool ShowRxData = true;
bool ShowTxData = false; //Shows the message that was sent (Debug only)
bool ShowTime = true;
bool ShowChecksum = true;
bool ShowLength = true;
bool RxListen = true;
bool LED1On = true;
bool LED2On = true;
bool TP_Rx_Flag=false;
bool TP_Tx_Flag=false;
bool Loop_flag = false;
uint8_t CryptographyFlags = 0b00000000; //X,X,X,X,X,X,pubK_Saved,prK_Saved

//Variables
elapsedMicros J1708Timer; //Set up a microsecond timer to run after each byte is received.
elapsedMillis J1708LoopTimer; //Set up a microsecond timer to run after each byte is received.
uint16_t idleTime = 3640; //Idle time is used to delimit each frame. 3640 micros minimum for T4.0 (experimental)
uint8_t J1708FrameLength = 0;
int J1708ByteCount;
uint8_t J1708Checksum = 0;
int TP_Rx_NBytes=0;
int TP_Rx_NSegments=0;
int TP_Tx_NBytes=0;
int TP_Tx_NSegments=0;
uint8_t TP_Default_Segment_Size=15;
uint8_t fx = 0;
uint32_t P = 1000;

//Component Information
uint8_t TractorMID = 88;
uint8_t TrailerMID = 89;
uint8_t MasterMID = 90;
uint8_t selfMID = 88;

//Buffers
uint8_t J1708RxBuffer[256]; //Buffer for unprinted Rx frames
uint8_t J1708TxBuffer[21]; //Buffer for unprinted Tx frames
char hexDisp[4]; //Character display buffer
uint8_t TP_Tx_Buffer[256] = {}; //Transport Protocol Buffer
uint8_t TP_Rx_Buffer[256] = {}; //Transport Protocol Buffer
uint8_t Loopbuffer[21];

//Functions
uint8_t J1708Rx(uint8_t (&J1708RxFrame)[256]) {
  if (Serial3.available()){
    J1708Timer = 0; //Reset the RX message timer for J1708 message framing
    J1708ByteCount++; // Increment the recieved byte counts
    
    if (J1708ByteCount < sizeof(J1708RxBuffer)){ //Ensure the RX buffer can handle the new messages
       J1708RxBuffer[J1708ByteCount] = Serial3.read();
       J1708Checksum += J1708RxBuffer[J1708ByteCount];
    }
    else{
      //This is what we do if we don't have room in the RX buffer.
      Serial.println("J1708 Buffer Overflow");
      Serial3.clear();
      J1708ByteCount = 0;
    }
  }

  //Check to see if a message has been framed?
  if (J1708Timer > idleTime && J1708ByteCount > 0){
    J1708FrameLength = J1708ByteCount;
    J1708ByteCount = 0; //reset the counter
    
    J1708Checksum -= J1708RxBuffer[J1708FrameLength]; //remove the received Checksum byte (last one)
    J1708Checksum = (~J1708Checksum + 1) & 0xFF; // finish calculating the checksum
    boolean J1708ChecksumOK = J1708Checksum == J1708RxBuffer[J1708FrameLength];
     
    if (J1708ChecksumOK) {
      return J1708FrameLength;
    }
    else {
      Serial.println ("Checksum failed, Message has errors.");
      return 0; //debug
      //return 0; //data would not be valid, so pretend it didn't come
    }
  }

  else
    return 0; //A message isn't ready yet.
}

void RxEnable(){
  //OUTPUT: (Rx-Enable) High-RxOFF, Low-RxON
  //INPUT:  (Tx-Enable) High-TxON, LOW-TxOFF
  pinMode(RDPin,OUTPUT);
  digitalWrite(RDPin,LOW);
}

void TxEnable(){
  //OUTPUT: (Rx-Enable) High-RxOFF, Low-RxON
  //INPUT:  (Tx-Enable) High-TxON, LOW-TxOFF
  pinMode(RDPin,INPUT);
  digitalWrite(RDPin,HIGH);
}

void J1708AppendChecksum(uint8_t J1708TxData[],const uint8_t &TxFrameLength){
  uint8_t chk = 0;
  for (int i=0; i<(TxFrameLength-1);i++){
    chk+=J1708TxData[i];
  }
  chk=((~chk<<24)>>24)+1;
  J1708TxData[TxFrameLength-1] = chk;
}

void J1708Tx(uint8_t J1708TxData[], const uint8_t &TxFrameLength, const uint8_t &TxFramePriority) { //Max bytes in a J1708 frame is 21.
  /*
  * TODO:
  * - Add collision detection
  * - Add proper message timing by checking if bus is available. David
  */

  TxEnable();
  //Append checksum
  J1708AppendChecksum(J1708TxData,TxFrameLength);
  J1708Timer=0;
  //Priority Delay Time
  while (J1708Timer<(104.16*TxFramePriority*2)){
  }
  Serial3.write(J1708TxData,TxFrameLength);
  //Show Full message sent
  if (ShowTxData){
    if (ShowTime){
      Serial.print("(");
      Serial.print(micros());
      Serial.print(")");
      Serial.print(" ");
    }
    if (ShowLength){
      Serial.print("[");
      Serial.print(TxFrameLength);
      Serial.print("]");
      Serial.print(" ");
    }
    for (int i=0; i<TxFrameLength; i++){
      Serial.printf("%02X",J1708TxData[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
  J1708Timer=0;
  //Post-Transmit Delay Time
  while (J1708Timer<(104.16*random(0,7))){
  }
}

bool RTS_Handler(uint8_t TP_Data[]){
  Serial.print("RTS Handler Started [");Serial.print(selfMID);Serial.println("]");
  if (!TP_Rx_Flag){
      uint8_t TP_MID=TP_Data[1];
      TP_Rx_NSegments=TP_Data[6];
      uint8_t TP_Rx_NBytes=TP_Data[7];
    if (TP_Rx_NBytes<=256){
      uint8_t CTS_message[8] = {selfMID,197,4,TP_MID,2,TP_Rx_NSegments,1,0};
      TP_Rx_Flag = true;
      J1708Tx(CTS_message,8,8);
    }
    Serial.print("RTS Handler Complete [");Serial.print(selfMID);Serial.println("]");
    return 1;
  }
  else{
    Serial.print("RTS is busy but received request. [");Serial.print(selfMID);Serial.println("]");
    //Busy. Do not Respond.
    return 0;
  }
}

bool CTS_Handler(uint8_t TP_Data[]){
  Serial.print("CTS Handler Started [");Serial.print(selfMID);Serial.println("]");
  if (TP_Tx_Flag){
    uint8_t TP_MID=TP_Data[1];
    uint8_t TP_NSegments=TP_Data[6];
    uint8_t TP_StartSegment=TP_Data[7];
    uint8_t TP_NBytes=TP_Tx_NBytes; 
    int N=0;
    int delay_time = 1;
    if (TP_NSegments>1){
      int delay_time = 777;
    }
    for (int i=TP_StartSegment; i<TP_NSegments+TP_StartSegment; i++){
      if (TP_NBytes>=TP_Default_Segment_Size){
        N = TP_Default_Segment_Size;
        uint8_t CDP_message1[N+5+1]={selfMID,198,N+2,TP_MID,i};
        for(int j=0; j<N;j++){
          CDP_message1[j+5] = TP_Tx_Buffer[j+(N*(i-1))];
          TP_NBytes--;
        }
        J1708Tx(CDP_message1,N+5+1,8);
        delay(delay_time);
      }
      else{
        N = TP_NBytes;
        uint8_t CDP_message2[N+5+1]={selfMID,198,N+2,TP_MID,i};
        for (int k=0; k<N; k++){
          CDP_message2[k+5] = TP_Tx_Buffer[k+(TP_Default_Segment_Size*(i-1))];
          TP_NBytes--;
        }
        J1708Tx(CDP_message2,N+5+1,8);
        delay(delay_time);
      }
    }
    return 1;
  }
  else{
    Serial.print("CTS handler received request but is busy or did not expext CTS. [");Serial.print(selfMID);Serial.println("]");
    //Busy. Do not Respond.
    return 0;
  }
  Serial.print("CTS Handler Complete [");Serial.print(selfMID);Serial.println("]");
}

bool CDP_Handler(uint8_t TP_Data[]){
  Serial.print("CDP Handler Started [");Serial.print(selfMID);Serial.println("]");
  uint8_t TP_MID=TP_Data[1];
  uint8_t TP_NBytes=TP_Data[3]; 
  uint8_t TP_SegmentNumber=TP_Data[5];
  uint8_t TP_Start=(TP_SegmentNumber-1)*TP_Default_Segment_Size;

  for (int i=0; i<(TP_NBytes-2); i++){
    TP_Rx_Buffer[TP_Start+i] = TP_Data[i+6];
  }
  if (TP_SegmentNumber==TP_Rx_NSegments){
    uint8_t EOM_message[6] = {selfMID,197,2,TP_MID,3};
    J1708Tx(EOM_message,6,8);

    //Reset all Variables and Flags
    TP_Rx_Flag = false;
    TP_Rx_NBytes=0;
    TP_Rx_NSegments=0;
    Serial.print("CDP Handler Complete [");Serial.print(selfMID);Serial.println("]");
    return 1;
  }
  else{
    Serial.print("CDP Handler is Waiting for Data Segments.");Serial.print(selfMID);Serial.println("]");
    //Request the next batch of data segments or wait. 
    //TODO
    return 0;
  }
}

void EOM_Handler(uint8_t TP_Data[]){
  //uint8_t TP_MID=TP_Data[1]; //The only real information here
  Serial.print("EOM Handler Started [");Serial.print(selfMID);Serial.println("]");
  //Reset all Variables and Flags
  TP_Rx_Flag = false;
  TP_Rx_NBytes=0;
  TP_Rx_NSegments=0;
  TP_Tx_Flag = false;
  TP_Tx_NBytes=0;
  TP_Tx_NSegments=0;
  uint8_t TP_Tx_Buffer[256] = {}; //Transport Protocol Buffer
  Serial.print("EOM Handler Complete [");Serial.print(selfMID);Serial.println("]");
}

void Abort_Handler(uint8_t TP_Data[]){
  //Reset all Variables and Flags
  Serial.print("Abort Handler Started [");Serial.print(selfMID);Serial.println("]");
  TP_Rx_Flag = false;
  TP_Rx_NBytes=0;
  TP_Rx_NSegments=0;
  TP_Tx_Flag = false;
  TP_Tx_NBytes=0;
  TP_Tx_NSegments=0;
  uint8_t TP_Tx_Buffer[256] = {}; //Transport Protocol Buffer
  uint8_t TP_Rx_Buffer[256] = {}; //Transport Protocol Buffer
  Serial.print("Abort Handler Complete [");Serial.print(selfMID);Serial.println("]");
}

bool J1708CheckChecksum(uint8_t J1708Message[],const uint8_t &FrameLength){
  uint8_t chk = 0;
  for (int i=0; i<(FrameLength-1);i++){
    chk+=J1708Message[i];
  }
  chk=((~chk<<24)>>24)+1;
  if (J1708Message[FrameLength]==chk){
    return 1;
  }
  else{
    return 0;
  }
}

int parseJ1708(){
  if (!Loop_flag){
    //Save the current message in the RxBuffer to a more stable 32 byte location.
    //This also frees the J1708RxBuffer to be used in loop() after parseJ1708() is called.
    //uint8_t Loopbuffer[32]={};
    memcpy(Loopbuffer, J1708RxBuffer, 21);
    switch(Loopbuffer[1]){ //MID
      case 89:
        //Serial.println("MID 89 Received!"); //debug
        switch(Loopbuffer[2]){ //PID
          case 128:
            //Serial.println("PID 128 Received!"); //debug
            if (selfMID==Loopbuffer[4]){
              //Component-Specific Request Parameter handler
            }
            return 0;
            break;
          case 197:
            //Serial.println("PID 197 Received!"); //debug
            if (Loopbuffer[4]==selfMID){
              switch(Loopbuffer[5]){
                case 1:
                  Serial.println("RTS Received!");
                  //Run RTS Handler
                  return 1;
                  break;
                case 2:
                  Serial.println("CTS Received!");
                  //Run CTS Handler
                  return 2;
                  break;
                case 3:
                  Serial.println("EOM Received!");
                  //Run EOM Handler
                  return 3;
                  break;
                case 255:
                  Serial.println("Abort Received!");
                  //Run Abort Handler
                  return 4;
                  break;
              }
            }
            return 0;
            break;
          case 198:
            //Serial.println("PID 198 Received!"); //debug
            if (Loopbuffer[4]==selfMID){
              Serial.println("CDP Received!"); //debug
              //CDPHandler
              return 5;
              break;
            }
            else{
            }
            return 0;
            break;

          case 234:
            //Serial.print("PID 234 Received!"); //debug
            return 0;
            break;
          case 237:
            //Serial.print("PID 237 Received!");
            return 0;
            break;
          case 243:
            //Serial.print("PID 243 Received!");
            return 0;
            break;
          case 246:
            //Serial.print("PID 246 Received!");
            return 0;
            break;
          case 251:
            //Serial.print("PID 251 Received!");
            return 0;
            break;
          case 252:
            //Serial.print("PID 252 Received!");
            return 0;
            break;
          case 253:
            //Serial.print("PID 253 Received!");
            return 0;
            break;

          case 255:
            switch(Loopbuffer[3]){
              case 255:
                switch(Loopbuffer[4]){
                  case 0:
                    //Serial.println("PID 512 Received!"); //debug
                    //Request Parameter Handler
                    return 0;
                    break;
                  case 1:
                    //Serial.println("PID 513 Received!"); //debug
                    //Temperature Sensor Handler
                    return 0;
                    break;
                  case 128:
                    //Serial.println("PID 640 Received!"); //debug
                    //Component-Specific Request Parameter Handler
                    return 0;
                    break;
                  case 192:
                    //Serial.println("PID 704 Received!"); //debug
                    //Network Security Request Handler
                    return 0;
                    break;
                  case 193:
                    //Serial.println("PID 705 Received!"); //debug
                    //Network Security Report Handler
                    return 0;
                    break;
                  case 194:
                    //Serial.println("PID 706 Received!"); //debug
                    //Generate python prK Handler
                    return 0;
                    break;
                  case 195:
                    //Serial.println("PID 707 Received!"); //debug
                    //Generate python pubK Handler
                    return 0;
                    break;
                  case 196:
                    //Serial.println("PID 708 Received!"); //debug
                    //Generate HSM prK Handler
                    return 0;
                    break;
                  case 197:
                    //Serial.println("PID 709 Received!"); //debug
                    //Generate HSM pubK Handler
                    return 0;
                    break;
                }
                return 0;
                break;
            }
        }
        return 0;
        break;
      default:
        //Serial.println("Unknown MID Received!"); //debug
        return 0;
        break;
    }
  }
  else{
    return fx;
  }
}

void J1708Listen() {
  if (J1708Rx(J1708RxBuffer)>0){ //Execute this if the number of recieved bytes is more than zero.
    if (LED1On){
      digitalWrite(LEDPin1,HIGH);
    }
    if (ShowTime){
      Serial.print("(");
      Serial.print(micros());
      Serial.print(")");
      Serial.print(" ");
    }
    if (ShowLength){
      Serial.print("[");
      Serial.print(J1708FrameLength);
      Serial.print("]");
      Serial.print(" ");
    }
    for (int i = 1; i < J1708FrameLength; i++){ //start at 1 to exclude 0x00 start value
      J1708Checksum += J1708RxBuffer[i];
      if (ShowRxData){
        sprintf(hexDisp,"%02X ",J1708RxBuffer[i]);
        Serial.print(hexDisp);
      }
    }
    if (!ShowTime && !ShowLength && !ShowRxData){
      //Nothing will be printed because all flags set false
    }
    else{
      Serial.println();
    }
    if (int(J1708LoopTimer)>P){
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
        fx = parseJ1708();
        if (fx>0){
          Loop_flag = true;
        }
        else{
          Loop_flag=false;
        }
      }
      J1708LoopTimer = 0;
    }
    else {
      if (fx>0){
        //Wait Until next scheduled send.
      }
      else{
        fx = parseJ1708();
        if (fx>0){
          Loop_flag = true;
        }
        else{
          Loop_flag=false;
        }
      }    
    }
  }
  else{
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
  }
  digitalWrite(LEDPin1,LOW);
}

//Cryptography Functions
void genECCKeys(uint8_t opMID, bool keyGen=false){
  //KeyGen True: Use HSM, False: Use Python Library
  int genType=0;
  if (keyGen == true){
    int genType = 2;
  }
  if ((CryptographyFlags&0b00000001)>0){
    //Retreive prkey from EEPROM
    //ECC prK is 32 bytes long
    uint8_t prKbuffer[32] = {}; 
    for (int i=0; i<32; i++){
      prKbuffer[i] = EEPROM.read(i);
    }
  }
  else {//Request private key from master
    uint8_t J1708RequestBuffer[6] = {opMID,255,255,0,194+genType}; //Reuasable Buffer for request messages
    J1708Tx(J1708RequestBuffer,6,8);
  }

  if ((CryptographyFlags&0b00000010)>0){
    //Retreive pubkey from EEPROM
    //ECC pubK is 32 bytes long
    uint8_t pubKbuffer[32] = {}; 
    for (int i=32; i<64; i++){
      pubKbuffer[i] = EEPROM.read(i);
    }
  }
  else {//Request public key from master
    uint8_t J1708RequestBuffer[6] = {opMID,255,255,0,195+genType}; //Reuasable Buffer for request messages
    J1708Tx(J1708RequestBuffer,6,8);
  }
}


//Message Handlers
//Transport Protocol Functions
bool J1708TransportTx(uint8_t TP_Data[], const uint16_t &nBytes, const uint8_t &TP_MID){
  if (!TP_Tx_Flag){
    if (nBytes>21){
      int whole_seg = nBytes/TP_Default_Segment_Size;
      int part_seg = nBytes%TP_Default_Segment_Size;
      int seg = whole_seg;
      if (part_seg>1){
        seg++;
      }
      TP_Tx_NBytes=nBytes;
      TP_Tx_NSegments=seg;
      uint8_t RTS_message[] = {selfMID,197,5,TP_MID,1,seg,((nBytes<<8)>>8),(nBytes>>8),0};
      J1708Tx(RTS_message,9,8);
      TP_Tx_Flag = true;
      //Load Data into buffer
      for (int i=0; i<nBytes; i++){
        TP_Tx_Buffer[i] = TP_Data[i];
      }
      return 1;
    }
    else{
      //Small enough message to send in normal frame.
      return 0;
    }
  }
  else{
    //Busy transmitting something else
    return 0;
  }
}

void storeKey(){}
void CCSR_Handler(){}
void CSRP_Handler(){}
void RP_Handler(){}
void TS_Handler(){}
void NSRp_Handler(){}
void NSPq_Handler(){}