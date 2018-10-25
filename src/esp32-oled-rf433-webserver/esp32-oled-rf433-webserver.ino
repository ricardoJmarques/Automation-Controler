#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <WiFi.h>
#include <Time.h>
#include <WebServer.h>
#include "qrcode.h"

char mes[12][4] {"Jan","Fev","Mar","Abr","Mai","Jun","Jul","Ago","Set","Out","Nov","Dec"};
char dia[7][4] {"Dom","Seg","Ter","Qua","Qui","Sex","Sab"};

WebServer server(80);

#define DHTPIN 22    // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// If using software SPI (the default case):
#define OLED_MOSI   23
#define OLED_CLK   18
#define OLED_DC    16
#define OLED_CS    5
#define OLED_RESET 17

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

int pinoTX = 19;
int PulseLength = 350; //tempo de duracao do estado de sinal do protocolo TX

String webPage = "<html><head><title>Automacao Casa</title><style>html,body{margin:0;padding:0;overflow:hidden;}body{display:flex;flex-direction:column;justify-content:center;align-items:center;width:100vw;height:100vh;}table{background-color:CCCCCC;border:0;width:100%;height:100%;text-align:center;font-size:36px;}input{width:90%;height:50%;font-size:36px;}select{width:50%;height:50%;text-align:center;font-size:36px;}form{width:100%;height:100%;}.in{width:43%;}.lb{width:16%;}</style></head><body><table><tr><td colspan=\"3\">Comando Universal</td></tr><tr><td colspan=\"3\"></td></tr><tr><td class=\"in\"><input type=\"button\" onclick=\"Dados('1','1')\" value=\"ON\"></td><td class=\"lb\">1</td><td class=\"in\"><input type=\"button\" onclick=\"Dados('1','0')\" value=\"OFF\"></td></tr><tr><td class=\"in\"><input type=\"button\" onclick=\"Dados('2','1')\" value=\"ON\"></td><td class=\"lb\">2</td><td class=\"in\" align=\"center\"><input type=\"button\" onclick=\"Dados('2','0')\" value=\"OFF\"></td></tr><tr><td class=\"in\"><input type=\"button\" onclick=\"Dados('3','1')\" value=\"ON\"></td><td class=\"lb\">3</td><td class=\"in\"><input type=\"button\" onclick=\"Dados('3','0')\" value=\"OFF\"></td></tr><tr><td class=\"in\"><input type=\"button\" onclick=\"Dados('4','1')\" value=\"ON\"></td><td class=\"lb\">4</td><td class=\"in\"><input type=\"button\" onclick=\"Dados('4','0')\" value=\"OFF\"></td></tr><tr><td colspan=\"3\">Grupo</td></tr><tr><td colspan=\"3\"><form name=\"formGrupo\"><select name=\"selectGrupo\"><option value=\"1\">I</option><option value=\"2\">II</option><option value=\"3\">III</option><option value=\"4\">IV</option></select></form></td></tr><tr><td colspan=\"3\">Canal</td></tr><tr><td colspan=\"3\"><form name=\"formCanal\"><select name=\"selectCanal\"><option value=\"A\">A</option><option value=\"B\">B</option><option value=\"C\">C</option><option value=\"D\">D</option><option value=\"E\">E</option><option value=\"F\">F</option><option value=\"G\">G</option><option value=\"H\">H</option><option value=\"I\">I</option><option value=\"J\">J</option><option value=\"K\">K</option><option value=\"L\">L</option><option value=\"M\">M</option><option value=\"N\">N</option><option value=\"O\">O</option><option value=\"P\">P</option></select></form></td></tr></table></body><script language=\"javascript\">var m_XML=false;function ProcessResponse(){if(m_XML.readyState==4){if(m_XML.status==200){}}}function Doxml(canal,numero,grupo,funcao){try{if(!m_XML){if(window.XMLHttpRequest){m_XML=new XMLHttpRequest();}else if(window.ActiveXObject){m_XML=new ActiveXObject(\"Microsoft.XMLHTTP\");}}if(m_XML){m_XML.open(\"POST\",\"automacao?canal=\"+canal+\"&numero=\"+numero+\"&grupo=\"+grupo+\"&funcao=\"+funcao);m_XML.onreadystatechange=ProcessResponse;m_XML.send(null);}}catch (e){}}function Dados(numero, funcao){var canal=formCanal.selectCanal.value;var grupo=formGrupo.selectGrupo.value;Doxml(canal,numero,grupo,funcao);}</script></html>";

int pinoMenu = 2;
int pinoBotao = 0;

int pinoEsq = 14;
int pinoDir = 27;

const char* ssid     = "MARQUES2";
const char* password = "rjsbm1983";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

float temperatura = 0;
float humidade = 0;
struct tm timeinfo;

void setup()   {                
  pinMode(pinoTX, OUTPUT);
  delay(10);
  digitalWrite(pinoTX, LOW);
  delay(10);
  pinMode(pinoBotao, INPUT);
  delay(10);
  pinMode(pinoMenu, INPUT);
  delay(10);
  pinMode(pinoEsq, INPUT_PULLDOWN);
  delay(10);
  pinMode(pinoDir, INPUT_PULLDOWN);
  delay(10);
  
  Serial.begin(115200);
 
  dht.begin();

  display.begin(SSD1306_SWITCHCAPVCC);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Connecting to ");
  display.print(ssid);
  display.display();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.display();
  }
  display.println();
  display.println("WiFi connected");
  display.println("IP address: ");
  display.println(WiFi.localIP());
  display.display();
    
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  display.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  display.display();


  delay(2000);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Starting Http server");
  display.display();
  
  //disconnect WiFi as it's no longer needed
  //WiFi.disconnect(true);
  //WiFi.mode(WIFI_OFF);

  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });
  
  server.on("/automacao", [](){
    String Scanal = server.arg("canal");
    String Snumero = server.arg("numero");
    String Sgrupo = server.arg("grupo");
    String Sfuncao = server.arg("funcao");
    char canal = Scanal[0];
    int grupo = (int)Sgrupo[0]-'0';
    int numero = (int)Snumero[0]-'0';
    int funcao = Sfuncao[0]-'0';
    sendTriState(comando(canal, grupo, numero, funcao));
    String f = "Canal: " + (String)canal + "\nGrupo: " + (String)grupo +"\nNumero: " + (String)numero + "\nFuncao: ";
    if(funcao==1)
      f+= "On";
    else
      f+="Off";
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(f);
    display.display();
    //sendTriState(comando(canal, grupo, numero, funcao));
    server.send(200, "text/plain", "OK");
  });

  server.begin();
  delay(500);
  display.println("HTTP server started");
  display.display();
  delay(500);
  display.println("Starting system");
  display.display();
  delay(500);
  printQRLocalIP();
  delay(2000);
  display.clearDisplay();
  display.invertDisplay(false);
  display.display();
}

time_t agora = 0;
time_t ant = 0;
bool stopWatch = 0;
bool menu = 0;
int delayGogo = 10;
int linha = 0;
int group = 6;

void loop(){
  agora = time(nullptr);
  
  if((digitalRead(pinoBotao) == LOW) && !stopWatch){
    printQRLocalIP();
    stopWatch = 1;
    delayGogo = 10;
    ant = agora;
  }
  
  if((digitalRead(pinoMenu) == HIGH) && !stopWatch){
    printMenu(linha, group);
    stopWatch = 1;
    menu = 1;
    delayGogo = 3;
    ant = agora;
  }
  
  if(digitalRead(pinoEsq) == HIGH){
    Serial.println("Esquerda");
    char cmd = group+'A';
    if (!menu){
      sendTriState(comando(cmd, 1, linha+1, 1));
      stopWatch = 1;
      delayGogo = 1;
      ant = agora;
      String f = "Canal: " + (String)cmd + "\nGrupo: I\nNumero: " + (String)(linha+1) + "\nFuncao: On";
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println(f);
      display.display();
    }
    else {
      ant = agora;
      linha = (linha+1) % 4;
      printMenu(linha, group);
    }
    delay(200);
  }
  
  if(digitalRead(pinoDir) == HIGH){
    Serial.println("Direita");
    char cmd = group+'A';
    if(!menu){
      sendTriState(comando(cmd, 1, linha+1, 0));
      stopWatch = 1;
      delayGogo = 1;
      ant = agora;
      String f = "Canal: " + (String)cmd + "\nGrupo: I\nNumero: " + (String)(linha+1) + "\nFuncao: Off";
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println(f);
      display.display();
    }
    else{
      ant = agora;
      group = (group+1) % 16;
      printMenu(linha, group);
    }
    delay(200);
  }
  
  if(!stopWatch){
    gogo();
  }
  
  else {
    if (agora-ant > delayGogo){
      stopWatch = 0;
      menu = 0;
      display.clearDisplay();
      display.invertDisplay(false);
      display.display();
    }
  }
  server.handleClient();
}



