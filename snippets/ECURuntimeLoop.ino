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
        J1708LoopTimer = 0;
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
        fx = parseJ1708(J1708RxFrame);
        if (fx>0){
          Loop_flag = true;
        }
        else{
          Loop_flag=false;
        }
      }    
    }
  }