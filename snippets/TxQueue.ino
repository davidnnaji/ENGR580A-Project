Q_Counter = 1;
Q_flag = false;
Q_Matrix[20][21];
Q_Lengths[20];
ElapsedMillis() Q_Timer;

if (Q_Flag){
	if (Q_Timer>1250){
		Q_Message[] = {};
		for (int i=0;i<Q_Lengths[Q_Counter];i++){
			Q_Message[i] = Q_Matrix[Q_Counter][i];
		}
		J1708Tx(Q_Message,Q_Lengths[Q_Counter],8);
		Q_Counter++;
		if (Q_Counter==TP_Tx_NSegments){
			Q_flag==false;
		}
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

    if (TP_NSegments>1){
    }
    for (int i=TP_StartSegment; i<TP_NSegments+TP_StartSegment; i++){
      if (TP_NBytes>=TP_Default_Segment_Size){
        N = TP_Default_Segment_Size;
        Q_Matrix[i][0]=selfMID;
        Q_Matrix[i][1]=198;
        Q_Matrix[i][2]=N+2;
        Q_Matrix[i][3]=TP_MID;
        Q_Matrix[i][4]=i;
        for(int j=0; j<N;j++){
          Q_Matrix[i][j+5] = TP_Tx_Buffer[j+(N*(i-1))];
          TP_NBytes--;
        }
        //J1708Tx(CDP_message1,N+5+1,8);
        //delay(delay_time);
      }
      else{
        N = TP_NBytes;
				Q_Matrix[i][0]=selfMID;
				Q_Matrix[i][1]=198;
				Q_Matrix[i][2]=N+2;
				Q_Matrix[i][3]=TP_MID;
				Q_Matrix[i][4]=i;
        for (int k=0; k<N; k++){
          Q_Matrix[i][k+5] = TP_Tx_Buffer[k+(TP_Default_Segment_Size*(i-1))];
          TP_NBytes--;
        }
        ////J1708Tx(CDP_message2,N+5+1,8);
        //delay(delay_time);
      }
    }
    TxQueue_flag = true;
    return 1;
  }
  else{
    Serial.print("CTS handler received request but is busy or did not expext CTS. [");Serial.print(selfMID);Serial.println("]");
    //Busy. Do not Respond.
    return 0;
  }
  Serial.print("CTS Handler Complete [");Serial.print(selfMID);Serial.println("]");
}