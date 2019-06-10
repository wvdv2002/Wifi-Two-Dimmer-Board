void cmdDimmer(SerialCommands* sender){
  char* valP = sender->Next();
  int dimmer = atoi(valP);
  valP = sender->Next();
  int val = atoi(valP);
  int i;
  if(valP==NULL){
    for(i=1;i<=amountOfDimmers();i++){
      setDimmer(i,dimmer);
    }
  }else{
    setDimmer(dimmer,val);
  }
}

void cmdMinDimmer(SerialCommands* sender){
  int dimmer = atoi(sender->Next());
  char* valP = sender->Next();
  int val = atoi(valP);
  int i;
  if(valP==NULL){
    for(i=1;i<=amountOfDimmers();i++){
      setMinDimmer(i,dimmer);
    }
  }else{
    setMinDimmer(dimmer,val);
  }
}

void cmdAmountDimmers(SerialCommands* sender){
  sender->GetSerial()->print("DIMCOUNT:");
  sender->GetSerial()->println(amountOfDimmers());
}

//Reset into the bootloader to receive new program. Only works with newer Optiboot bootloaders (see 
//reset into bootloader comes from optiboot example: https://github.com/Optiboot/optiboot/blob/master/optiboot/examples/test_reset/test_reset.ino
void cmdResetIntoBootloader(SerialCommands* sender){
#if FLASHEND > 140000
  sender->GetSerial()->println("ERROR"); //Jump to bootloader not supported on chips with >128k memory
#else
  for(int i=1;i<=amountOfDimmers();i++){ //switch off dimmers.
    setDimmer(i,0);
  }
  
  sender->GetSerial()->println("OK"); //Jump to bootloader not supported on chips with >128k memory       
  sender->GetSerial()->flush();
  wdt_disable();
  MCUSR = 0; //clear MCUSR so the bootloader stays on.
  typedef void (*gotoBootLoad_t)(void);
  const gotoBootLoad_t gotoBootLoad = (gotoBootLoad_t)((FLASHEND - 511) >> 1);
  cli(); TCCR0A = TCCR1A = TCCR2A = 0; // make sure interrupts are off and timers are reset.
  gotoBootLoad();
#endif
}

void cmdSetLeds(SerialCommands* sender){
  char* valP = sender->Next();
  int led1 = atoi(valP);
  valP = sender->Next();
  int led2 = atoi(valP);
  digitalWrite(DLED_RED,led1==0);
  digitalWrite(DLED_GREEN,led2==0); 
}

void cmdSetRampTime(SerialCommands* sender){
  char* valP = sender->Next();
  float dimmer = String(valP).toFloat();
  valP = sender->Next();
  if(valP==NULL){
    for(int i=1;i<=amountOfDimmers();i++){
      setRampTime(i,dimmer);
    }
  }else{
      float val = String(valP).toFloat();
      setRampTime(int(dimmer),val);
  }  
}



SerialCommand cmdDimmer_("DIMMER",cmdDimmer);
SerialCommand cmdMinDimmer_("MINDIMMER",cmdMinDimmer);
SerialCommand cmdAmountDimmers_("DIMCOUNT",cmdAmountDimmers);
SerialCommand cmdBootload_("DIMBOOT",cmdResetIntoBootloader);
SerialCommand cmdSetLeds_("SETLEDS",cmdSetLeds);
SerialCommand cmdSetRampTime_("SETRAMP",cmdSetRampTime);


void addAllDimmerCommandList(void){
  serComm.AddCommand(&cmdDimmer_);
  serComm.AddCommand(&cmdMinDimmer_);
  serComm.AddCommand(&cmdAmountDimmers_);
  serComm.AddCommand(&cmdBootload_);
  serComm.AddCommand(&cmdSetLeds_);
  serComm.AddCommand(&cmdSetRampTime_);
}
