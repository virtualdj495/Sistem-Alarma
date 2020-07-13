#include <Wire.h>
#include <Servo.h>

Servo motorStare;


#define buzzerPIN  2
#define redPIN  7
#define greenPIN  5
#define trigPIN 9
#define echoPIN 8

long duration;
char stare[4];
int var=0;

union floatToBytes {

  char buff[4];
  float distance;
}converter;

void alarmaOn(){
  digitalWrite(greenPIN,LOW);
    motorStare.write(90);
    tone(buzzerPIN,400);
    digitalWrite(redPIN,HIGH);
    delay(100);
    noTone(buzzerPIN);
    digitalWrite(redPIN,LOW);
    delay(100);
}

void alarmaOff(){
  digitalWrite(greenPIN,LOW);
    motorStare.write(90);
    tone(buzzerPIN,800);
    delay(200);
    noTone(buzzerPIN);
    tone(buzzerPIN,600);
    delay(150);
    noTone(buzzerPIN);
    delay(1000);
}

void alarmaAct(){
  motorStare.write(0);
    tone(buzzerPIN,250);
    delay(30);
    for(int i=1;i<10;i++){
      digitalWrite(greenPIN,HIGH);
      tone(buzzerPIN,100*i);
      delay(30);
      digitalWrite(greenPIN,LOW);
    }

    delay(150);
    noTone(buzzerPIN);
    digitalWrite(greenPIN,HIGH);
}

void alertaPornire(){
    for(int i=0;i<3;i++){
    digitalWrite(redPIN,HIGH);
    digitalWrite(greenPIN,HIGH);
    tone(buzzerPIN,600);    
    delay(200);
    digitalWrite(redPIN,LOW);
    digitalWrite(greenPIN,LOW);
    noTone(buzzerPIN);
    delay(100);
    }
    motorStare.write(0);
    delay(200);
    motorStare.write(90);
  }

void receiveEvent(int howMany) {
  uint8_t index=0;
  while (0 <Wire.available()) { 
    stare[index]= Wire.read();
    index++;
    var=0;
  }
  Serial.println(stare);
}         

void requestEvent(){
  Wire.write(converter.buff, 4);
}

void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);
  motorStare.attach(3);
  pinMode(trigPIN,OUTPUT);
  pinMode(echoPIN,INPUT);
  pinMode(buzzerPIN,OUTPUT);
  pinMode(redPIN,OUTPUT);
  pinMode(greenPIN,OUTPUT); 


}

void loop() {
  digitalWrite(trigPIN,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPIN,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPIN,LOW);

  duration=pulseIn(echoPIN,HIGH);
  converter.distance=duration*0.034/2;
  if(strcmp(stare,"ONN")==0){
    alarmaOn();
  }
  if(strcmp(stare,"OFF")==0&& var==0){
    alarmaOff();
    var=1;
  }
  if(strcmp(stare,"ACT")==0&& var==0){
    alarmaAct();
    var=1;
  }
  if(strcmp(stare,"DEC")==0&& var==0){
    alertaPornire();
    var=1;
  }
} 
