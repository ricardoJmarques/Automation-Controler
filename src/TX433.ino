//###########################################
//# Modulo TX 433Mhz - Protocolo Intertechno

char* comando(char sFamily, int nGroup, int nDevice, boolean bStatus) {
  static char sReturn[13];
  int nReturnPos = 0;
  if ( (byte)sFamily < 65 || (byte)sFamily > 80 || nGroup < 1 || nGroup > 4 || nDevice < 1 || nDevice > 4) {
    return '\0';
  }
  char* sDeviceGroupCode =  dec2binWcharfill(  (nDevice-1) + (nGroup-1)*4, 4, '0');
  char familycode[16][5] = { "0000", "F000", "0F00", "FF00", "00F0", "F0F0", "0FF0", "FFF0", "000F", "F00F", "0F0F", "FF0F", "00FF", "F0FF", "0FFF", "FFFF" };
  for (int i = 0; i<4; i++) {
    sReturn[nReturnPos++] = familycode[ (int)sFamily - 65 ][i];
  }
  for (int i = 0; i<4; i++) {
    sReturn[nReturnPos++] = (sDeviceGroupCode[3-i] == '1' ? 'F' : '0');
  }
  sReturn[nReturnPos++] = '0';
  sReturn[nReturnPos++] = 'F';
  sReturn[nReturnPos++] = 'F';
  if (bStatus) {
    sReturn[nReturnPos++] = 'F';
  } else {
    sReturn[nReturnPos++] = '0';
  }
  sReturn[nReturnPos] = '\0';
  return sReturn;
}

char* dec2binWcharfill(unsigned long Dec, unsigned int bitLength, char fill){
  static char bin[64];
  unsigned int i=0;
  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : fill;
    Dec = Dec >> 1;
  }
  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    }
    else {
      bin[j] = fill;
    }
  }
  bin[bitLength] = '\0';
  return bin;
}

void transmit(int nHighPulses, int nLowPulses) {
    digitalWrite(pinoTX, HIGH);
    delayMicroseconds( PulseLength * nHighPulses);
    digitalWrite(pinoTX, LOW);
    delayMicroseconds( PulseLength * nLowPulses);
}

void sendTriState(char* sCodeWord) {
  for (int nRepeat=0; nRepeat<5; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
      switch(sCodeWord[i]) {
        case '0':
          sendT0();
        break;
        case 'F':
          sendTF();
        break;
        case '1':
          sendT1();
        break;
      }
      i++;
    }
    sendSync();
  }
}

void sendT0() {
  transmit(1,3);
  transmit(1,3);
}

void sendTF() {
  transmit(1,3);
  transmit(3,1);
}

void sendT1() {
  transmit(3,1);
  transmit(3,1);
}

void sendSync() {
    transmit(1,31);
}
//################################
