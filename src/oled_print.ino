void printMenu(int line, int grupo){
    int i = 0;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("####################");
    display.println("#     COMANDO      #");
    for(i=0; i<4; i++){
      if(i==linha){
        display.setTextColor(BLACK, WHITE);
      }
      display.printf("#   ON   %d   OFF   #\n", i+1);
      if(i==linha){
        display.setTextColor(WHITE);
      }
    }
    display.printf("#    GRUPO: %C      #\n", grupo+'A');
    display.println("####################");
    display.display();
}


void printQR(char *str){
  int dispX = 35;
  int dispY = 3;
  display.clearDisplay();
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, str);
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if(qrcode_getModule(&qrcode, x, y) == 1){
        //display.drawPixel(x+dispX, y+dispY, WHITE);
        
        display.drawPixel((x*2)+dispX, (y*2)+dispY, WHITE);
        display.drawPixel((x*2)+dispX+1, (y*2)+dispY, WHITE);
        display.drawPixel((x*2)+dispX, (y*2)+dispY+1, WHITE);
        display.drawPixel((x*2)+dispX+1, (y*2)+dispY+1, WHITE);
        
      }
    }
  }
  display.invertDisplay(true);
  display.display();
}

void printQRLocalIP(void) {
  char str[80] = {0};
  IPAddress ip;
  ip = WiFi.localIP();
  sprintf(str, "http://%d.%d.%d.%d/", ip[0],ip[1],ip[2],ip[3]);
  //Serial.println(str);
  printQR(str);
}

void printLocalTime()
{
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void gogo(void) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  display.setTextColor(BLACK, WHITE);
  display.print("t");
  display.setTextColor(WHITE);
  display.printf("%.0f%c  ", t, 167);
  display.setTextColor(BLACK, WHITE);
  display.print("h");
  display.setTextColor(WHITE);
  display.printf("%.0f%c\n", h, 37);
  display.setTextSize(1);
  display.printf("IC: %.1f%c\n", hic, 167);
  display.setTextSize(2);


  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  else{
    display.println(&timeinfo, " %H:%M:%S");
    display.setTextSize(1);
    int m,d;
    d = timeinfo.tm_wday;
    m = timeinfo.tm_mon;
    display.printf("   %s %d %s %d\n", dia[d], timeinfo.tm_mday, mes[m], 1900+timeinfo.tm_year);
    //display.println(&timeinfo, "%A %d %B %Y");
  }

  display.print("IP: ");
  display.println(WiFi.localIP());

  display.printf("Num: %d. Grp: %c", linha+1, group+'A');   
  
  display.display();
//  printLocalTime();
  return;
}

