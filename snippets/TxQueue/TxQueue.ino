Q_Counter = 0;
Q_flag = false;
Q_Matrix[20][21];
Q_Lengths[20];
ElapsedMillis() Q_Timer;

if (Q_Flag){
	if (Q_Timer>1000){
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