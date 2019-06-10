/**
 * Complete rewrite of Wifi dual dimmer code using the Dimmer library from circuitar, movingavg and serialcommands libraries.
 *
 * Copyright (c) 2019 Wvdv2002
 * This software is released under the MIT license. See the attached LICENSE file for details.
 */

#include "Dimmer.h"
#define SERIAL_COMMANDS_DEBUG
#include <SerialCommands.h>
#include <movingAvg.h> 
#include <avr/wdt.h>

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

int knob1=0;
int knob2=0;
movingAvg knobAvg[2](10);

Dimmer dimmers[] = {
  Dimmer(9, DIMMER_RAMP,1,MAINS_FREQ),
  Dimmer(8, DIMMER_RAMP,1,MAINS_FREQ),
};


void loop() {
  int z;
  if((z=serComm.ReadSerial())){
    serComm.ClearBuffer();
  }
  inputsTask();
  delay(10);
  wdt_reset();
}


void cmdStatus(SerialCommands* sender){
  Serial* sender->GetSerial()->print("STATUS:");
  int i;
  for(i=0;i<(amountOfDimmers()-1);i++){
    sender->GetSerial()->print(dimmers[i].getSetValue());
    sender->GetSerial()->print(',');
    sender->GetSerial()->print(knobAvg[i].getAvg());
    sender->GetSerial()->print(',');
  }
    sender->GetSerial()->print(dimmers[i].getSetValue());
    sender->GetSerial()->print(',');
    sender->GetSerial()->println(knobAvg[i].getAvg());
}


void cmdUnrecognized(SerialCommands* sender){
  //sender->GetSerial()->println("ERROR NO_PORT");
}

void setDimmer(int i, int val){
  i--;
  if(i<amountOfDimmers() && i>=0){
    if(val>99){val=100;};
    dimmers[i].set(val,val>0);
  }
}

void setRampTime(int i, float val){
  i--;
  if(i<amountOfDimmers() && i>=0){
    dimmers[i].setRampTime(val);
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
  wdt_reset();
  wdt_enable(WDTO_4S);
  pinMode(DLED_RED, OUTPUT);
  pinMode(DLED_GREEN, OUTPUT);
  delay(10);
  digitalWrite(DLED_RED,1);
  digitalWrite(DLED_GREEN,1);
  Serial.begin(115200);
  pinMode(SWITCH_INPIN1,INPUT);//analog
  pinMode(SWITCH_INPIN2,INPUT);//analog
  knobAvg[0].begin();
  knobAvg[1].begin();
  for(int i = 0; i < sizeof(dimmers) / sizeof(Dimmer); i++) {
    dimmers[i].begin();
    dimmers[i].off();
    dimmers[i].setMinimum(5);

  }
  serComm.AddCommand(&cmdStatus_);
  addAllDimmerCommandList();
  serComm.SetDefaultHandler(&cmdUnrecognized);
}


int amountOfDimmers(void){
  return sizeof(dimmers)/sizeof(Dimmer);
}

void inputsTask(){
  knobAvg[0].reading(analogRead(SWITCH_INPIN1));
  knobAvg[1].reading(analogRead(SWITCH_INPIN2));
}
