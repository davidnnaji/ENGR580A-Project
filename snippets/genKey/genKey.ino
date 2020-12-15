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