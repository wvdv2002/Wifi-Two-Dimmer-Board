/**
 * Dim many lamps simultaneously.
 *
 * Copyright (c) 2016 Circuitar
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Dimmer.h"
#define SERIAL_COMMANDS_DEBUG
#include <SerialCommands.h>

const int DLED_RED=3;
const int DLED_GREEN=4;
const int SWITCH_INPIN1=A5;
const int SWITCH_INPIN2=A4;
const int ZCD_INT=0;
const int ZCD_PIN=2;
const int MAINS_FREQ=50;
const int LASTCOMMUNICATIONTIMEOUT=20000; //If the time after last communication is longer than this, start handling the knobs ourselves.
long lastCommunicationReceived = 0;
/* SETUP ZCD PIN in Dimmer.h in the library*/

char serBuff[32];
SerialCommands serComm(&Serial, serBuff, sizeof(serBuff),"\n", ":,");

struct InSetPoint{
  int value;
  int diffForNewValue;
  int pin;
};

Dimmer dimmers[] = {
  Dimmer(9, DIMMER_RAMP,1.5,MAINS_FREQ),
  Dimmer(8, DIMMER_RAMP,1.5,MAINS_FREQ),
};


void loop() {
  int z;
  if((z=serComm.ReadSerial())){
    serComm.ClearBuffer();
  }
  inputsTask();
}


void cmdStatus(SerialCommands* sender){
  Serial* sender->GetSerial()->print("STATUS:");
  int i;
  for(i=0;i<(amountOfDimmers()-1);i++){
    sender->GetSerial()->print(dimmers[i].getValue());
    sender->GetSerial()->print(',');
    sender->GetSerial()->print(0);
    sender->GetSerial()->print(',');
  }
    sender->GetSerial()->print(dimmers[i].getValue());
    sender->GetSerial()->print(',');
    sender->GetSerial()->println(0);
}


void cmdUnrecognized(SerialCommands* sender){
  //sender->GetSerial()->println("ERROR NO_PORT");
}



void setDimmer(int i, int val){
  i--;
  if(i<amountOfDimmers() && i>=0){
    if(val>100){val=100;};
    dimmers[i].set(val);
  }
}

void setMinDimmer(int i, int val){
  i--;
  if(i<amountOfDimmers() && i>=0){
    if(val>100){val=100;};
    dimmers[i].setMinimum(val);
  }  
}

SerialCommand cmdStatus_("STATUS",cmdStatus);
#include "cmdDimmerList.h"


void setup() {
  Serial.begin(115200);
  for(int i = 0; i < sizeof(dimmers) / sizeof(Dimmer); i++) {
    dimmers[i].begin();
  }
  serComm.AddCommand(&cmdStatus_);
  addAllDimmerCommandList();
  serComm.SetDefaultHandler(&cmdUnrecognized);
}


int amountOfDimmers(void){
  return sizeof(dimmers)/sizeof(Dimmer);
}

void inputsTask(){
  
}
