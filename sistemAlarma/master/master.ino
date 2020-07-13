#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Wire.h>

const char* ssid="GhostMan";
const char* pass="cubulrubik2";

ESP8266WebServer server(80);   
 
String getContentType(String filename);
bool handleFileRead(String path);

union floatToBytes {

  char buff[4];
  float distance;
}converter;

char stare[4];
char starev[4];
String sir;

void refresh(){
  char msgFinal[50];
  uint8_t index=0;
  Wire.requestFrom(8, 4);
  while(Wire.available()){
    converter.buff[index]= Wire.read();
    index++;
  } 
  sprintf(msgFinal,"%.2f",converter.distance);
  server.send(200,"application/javascript",msgFinal);
}
void stareS(){
  strcpy(starev,stare);
  sir=server.arg("state");
  strcpy(stare, sir.c_str());
  if(strcmp(stare,starev)!=0){
    Wire.beginTransmission(8);
    Wire.write(stare, 4);
    Wire.endTransmission();
    Serial.println("Trimis");
  }
}


void setup() {
  Serial.begin(9600);
  Wire.begin(D1,D2);
  delay(10);
  Serial.println('\n');

  Serial.println(" Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,pass);
  Serial.println("Connection pending");
  while(WiFi.status()!= WL_CONNECTED){

    delay(500);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());             
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           

  if (MDNS.begin("esp8266")) {              
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  
  SPIFFS.begin();                           
  
  server.onNotFound([]() {                              
    if (!handleFileRead(server.uri()))                  
      server.send(404, "text/plain", "404: Not Found"); 
  });
  server.on("/refresh",refresh);
  server.on("/stare", stareS);

  server.begin();                           
  Serial.println("HTTP server started");
  strcpy(stare,"DEC");
  Wire.beginTransmission(8);
  Wire.write(stare, 4);
  Wire.endTransmission();
  }

void loop() {
  server.handleClient();

}

String getContentType(String filename) { 
  if (filename.endsWith(".html")) return "text/html";
  return "text/plain";
}

bool handleFileRead(String path) {
  if (path.endsWith("/")) path += "index.html";         
  String contentType = getContentType(path);            
  if (SPIFFS.exists(path)) {                            
    File file = SPIFFS.open(path, "r");                 
    size_t sent = server.streamFile(file, contentType); 
    file.close();                                       
    return true;
  }
  return false;                                      
}
