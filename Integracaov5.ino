#include <Arduino.h>
#include <U8g2lib.h>

#include <DHT.h>
#include <DHT_U.h>

#include <Adafruit_Sensor.h>

String buffer;

void waitAndEcho(int t){
  buffer="";
  unsigned long start = millis();
  do{
    if(Serial1.available()){
      buffer += (char)Serial1.read();
    }
  }while(millis() < start+t);
    buffer.replace("\r","\\r");
    buffer.replace("\n","\\n");
    Serial.println(buffer);
}

bool waitAndEcho(int t, String s){
  buffer="";
  unsigned long start = millis();
  unsigned long last = 0;
  unsigned int n = s.length();
  bool ret = false;
  do{
    if(Serial1.available()){
      buffer += (char)Serial1.read();
      last = millis();
      if(buffer.length() >= n){
        if(buffer.substring(buffer.length()-n).equals(s)) {
          ret = true;
          break;
        }
      }
    }
  }while(millis() < start+t);
    buffer.replace("\r","\\r");
    buffer.replace("\n","\\n");
    Serial.println(buffer);
    return ret;
}

void waitAndEchoNoPrint(int t){
  buffer="";
  unsigned long start = millis();
  
  do{
    if(Serial1.available()){
      buffer += (char)Serial1.read();
    }
  }while(millis() < start+t);
    buffer.replace("\r","\\r");
    buffer.replace("\n","\\n");
    //Serial.println(buffer);
}

bool waitAndEchoNoPrint(int t, String s){
  buffer="";
  unsigned long start = millis();
  unsigned long last = 0;
  unsigned int n = s.length();
  bool ret = false;
  do{
    if(Serial1.available()){
      buffer += (char)Serial1.read();
      last = millis();
      if(buffer.length() >= n){
        if(buffer.substring(buffer.length()-n).equals(s)) {
          ret = true;
          break;
        }
      }
    }
  }while(millis() < start+t);
    buffer.replace("\r","\\r");
    buffer.replace("\n","\\n");
    //Serial.println(buffer);
    return ret;
}

DHT dht(A0,DHT11);

String response; //String global que recebera a resposta HTTP
String lastResponse; //String que guarda a ultima mensagem do servidor

// A linha abaixo define as ligacoes e deve ser 
// ajustada conforme o display utilizado. 
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, 6, 5, 4, 7); //Enable, RW, RS, RESET

void u8g_Tela1() { //Tela 1 - Carregamento do Smart Sinalizator

  u8g2.setFont(u8g2_font_ncenB10_tf);  
  u8g2.drawStr( 41, 20, "Smart ");  
  u8g2.drawStr( 24, 35, "Sinalizator");
  u8g2.setFont(u8g2_font_crox1t_tf);
  u8g2.drawStr( 36, 50, "Carregando...");
  u8g2.drawFrame(0,0,128,64);  
  u8g2.drawFrame(2,2,124,60);   
}
   
void u8g_Tela2() //Tela 2 - Tudo 
{
  char * cstr = new char [response.length()+1]; //Cria string em C basico com o tamanho da string em C++
  char * cstr1 = new char [response.length()+1];
  String tempo, tempo1;
  lastResponse = response;
  if(response.indexOf('[') != -1) //Checa a existencia do caracter [ da resposta para fazer quebra de linha
  {
    tempo = response.substring(0,response.indexOf('['));
    tempo1 = response.substring(response.indexOf('[')+1);
    strcpy(cstr,tempo.c_str());
    strcpy(cstr1,tempo1.c_str());
    u8g2.drawRFrame(0,0,128,64,3);  
    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.drawStr( 10, 24, cstr);
    u8g2.drawStr( 10, 36, cstr1);
  }
  else {
    strcpy(cstr,response.c_str()); //Copia a string C++ para string C
    u8g2.drawRFrame(0,0,128,64,3);  
    u8g2.setFont(u8g2_font_profont12_tf);
    u8g2.drawStr( 10, 33, cstr);
  }  
}  

void refreshBaixo(){ //Parte do display com as informações de temperatura e umidade
  char temp[20];
  char umidade[20];
  
    u8g2.setColorIndex(0);
    u8g2.drawRBox(1,54,126,9,1);
    u8g2.setColorIndex(1);
    sprintf(temp,"Temp: %d C",(int)dht.readTemperature());
    sprintf(umidade,"Umid: %d%c",(int)dht.readHumidity(), (char)37);  
    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.drawStr( 117, 58, "o");
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.drawStr( 76, 60, temp);  
    u8g2.drawStr( 3, 60, umidade);
}
   
void setup() 
{    
  dht.begin();
  Serial1.begin(115200);
  Serial.begin(115200);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g_Tela1();
  u8g2.sendBuffer();
  Serial1.println("AT+RST");
  waitAndEcho(3000,"ready\r\n");
  Serial1.println("AT+CWMODE=3");
  waitAndEcho(300,"OK\r\n");
  Serial1.println("AT+CWJAP=\"Matheus' iPhone\",\"conectarepans\"");
  waitAndEcho(15000,"OK\r\n");
  Serial1.println("AT+CIPMUX=0");
  waitAndEcho(100,"OK\r\n");
}  

void sendData() {
  String postString = "GET /read.php HTTP/1.1\r\n"
                      "Host: rpesite.000webhostapp.com\r\n"
                      "Accept: application/x-www-form-urlencoded; charset=utf-8\r\n"
                      "\r\n";
  Serial1.println("AT+CIPSEND="+String(postString.length()));
  //waitAndEchoNoPrint(50);
  waitAndEchoNoPrint(50,">");
  Serial1.print(postString);
  waitAndEchoNoPrint(3000);
}
   
void loop(void) {
  
  Serial1.println("AT+CIPSTART=\"TCP\",\"rpesite.000webhostapp.com\",80"); //(Re)abre a conexao TCP com o site
  waitAndEchoNoPrint(5000,"CONNECT\r\n\r\nOK\r\n");
  sendData();
  response = buffer.substring(buffer.indexOf('{')+1,buffer.indexOf('}')); //Separa do buffer o texto da resposta do HTTP do site
  // picture loop
  if((response != lastResponse) && (buffer.indexOf('{') != -1)) { //Se a resposta atual for diferente da ultima resposta, atualizar o display inteiro
  u8g2.clearBuffer();
  u8g_Tela2();
  refreshBaixo();
  u8g2.sendBuffer();
  }
  else { //Caso for igual, atualiza somente a humidade e a temperatura
    refreshBaixo();
    u8g2.sendBuffer();
  }
  Serial.println(response);
  Serial1.println("AT+CIPCLOSE"); //Fecha conexão TCP com o site
  delay(5000);
}
