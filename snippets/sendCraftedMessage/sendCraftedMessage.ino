//const int msgLength = 9;
//uint8_t msg[msgLength] = {89,197,5,88,1,1,1,0,0};
//const int msgLength = 8;
//uint8_t msg[msgLength] = {89,197,4,88,2,1,3};
const int msgLength = 6;
uint8_t msg[msgLength] = {89,197,2,88,3};
//uint8_t msg[msgLength] = {0b00000000,0b00000000,0b01010110};
const int RxEnablePin = 12;

void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  pinMode(RxEnablePin, INPUT); //OUTPUT: (Rx-Enable) High-RxOFF, Low-RxON, INPUT: (Tx-Enable) High-TxON, LOW-TxOFF
  digitalWrite(RxEnablePin, HIGH);
  Serial.println(sizeof(msg));

  delay(3000);
  J1708CalculateChecksum(msg,msgLength);
  tx1();
}

void loop() {
  //Do nothing.
}

void tx1(){
  Serial3.write(msg,msgLength);
  /*
  Serial.print("(");
  Serial.print(micros());
  Serial.print(") ");
  */
  for (int i=0; i<msgLength; i++){
    Serial.printf("%02X",msg[i]);
    Serial.print(" ");
  }
  Serial.println();
  delay(1000);
}

void J1708CalculateChecksum(uint8_t J1708TxData[],const uint8_t &TxFrameLength){
  uint8_t chk = 0;
  for (int i=0; i<(TxFrameLength-1);i++){
    chk+=J1708TxData[i];
  }
  chk=((~chk<<24)>>24)+1;
  J1708TxData[TxFrameLength-1] = chk;
}
