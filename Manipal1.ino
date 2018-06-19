#include "Wire.h"
#include "RTClib.h"
RTC_DS3231 rtc;
int count =0;
int z=0;
int m=0;
#define EEPROM_I2C_ADDRESS 0x50
int address[]={
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};                 //17 Bytes
byte dataw[]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
byte datar[]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
/////////////////////////////////////////////////////////////////thermistor
int ThermistorPin = 2;
int Vo;
float R1 = 4700;
float logR2, R2, T, Tc, Tf;
float c4 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
///////////////////////////////////////////////////////////////////////////
int time1=0;
int time2=0;
int time3=0;
int time4=0;
int time5=0;
int timenow=0;
int ack=0;
int test=0;
int k=24;
int a1,b1,c1,d1,e1=0;
int a,b,c=0;
void setup() 
{
  Serial.begin(9600);
  delay(100); 
  Wire.setModule(0);
  pinMode(19,OUTPUT);                                               //PWM Pin
  pinMode(6,OUTPUT);                                                //Bulkhead LED Indicator 
  pinMode(2,INPUT);                                                 //Temperature Input
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  Wire.begin();

}

void loop() 
{
  if(count%k==0)
  {
    Serial.print("brk");
    Serial.print("{\"Lamp10\":{");
    Serial.print("\"Temperature\":");
    Serial.print(Tc-20); 
    Serial.print(",\"Count\":");
    Serial.print(count);
    Serial.print(",\"Loop\":");
    Serial.print(test);
    Serial.print(",\"timenow\":"); 
    Serial.print(timenow); 
    Serial.print(",\"Ack\":"); 
    Serial.print(ack); 
    Serial.println("}}");
    ack=0;
  }

  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = logf(R2);
  T = (1.0 / (c4 + c2*logR2 + c3*logR2*logR2*logR2));
  Tc = T - 273.15;
  Tf = (Tc * 9.0)/ 5.0 + 32.0; 




  DateTime now = rtc.now();
  /*Serial.print(now.hour(), DEC);
   Serial.print(':');
   Serial.print(now.minute(), DEC);
   Serial.print(':');
   Serial.print(now.second(), DEC);
   Serial.println();
   delay(10);*/

  if (Serial.available() > 0) {
    for(int n=0; n<17; n++){
      dataw[n] = Serial.read();
      delay(10);
    }  
  }
  ////////////////////////////////////////////////////////////////////////RF DEBUG
  if(dataw[0]==72)
  {
    count=0;
    dataw[0]=73;
  }
  //////////////////////////////////////////////////////////////////////////count value
  if(dataw[0]==74)
  {
    k=dataw[1];
    dataw[0]=75;
  }
  count++;
  ///////////////////////////////////////////////////////////////////////////////set time
  if(dataw[0]==76)
  {
    rtc.adjust(DateTime(2017, 1, 21, dataw[1], dataw[2], 0));
    dataw[0]=77;
  }
  /////////////////////////////////////////////////////////////////////////////bulkhead
  if(dataw[0]==78)
  {
    m=dataw[1];
  }
  digitalWrite(6,m);

  //////////////////////////////////////////////////////////////////////////////eeprom write
  //  Serial.print(",\"Writing:\"");
  if(dataw[0]==70)
  {

    //  Serial.print("1");
    for(int x=0;x<17;x++)
    {
      writeAddress(address[x], dataw[x]); 
    }
    ack=1;
    dataw[0]=71;
    a1,b1,c1,d1,e1=0;//////////////////////////////////////////////////////////////////////////RESET LEVELS
  }
  //Serial.print("0");

  for(int x=0;x<17;x++)
  {
    datar[x] = readAddress(address[x]);
  }

  timenow =((now.hour()*60)+now.minute());
  time1 = ((datar[1]*60)+datar[2]);
  time2 = ((datar[3]*60)+datar[4]);
  time3 = ((datar[5]*60)+datar[6]);
  time4 = ((datar[7]*60)+datar[8]);
  time5 = ((datar[9]*60)+datar[10]);
  /*
  Serial.print(",\"time1:\""); 
   Serial.print(time1); 
   Serial.print(",\"time2:\""); 
   Serial.print(time2);
   Serial.print(",\"time3:\"");  
   Serial.print(time3);
   Serial.print(",\"time4:\"");  
   Serial.print(time4);
   Serial.print(",\"time5:\"");  
   Serial.print(time5);
   Serial.print(",\"timenow:\""); 
   Serial.print(timenow);
   Serial.println("}"); 
   */
  delay(1000);

  if(time1>time2)
  {
    a1=0;
  }
  else{
    a1=1;
  }
  if(time2>time3)
  {
    b1=0;
  }
  else{
    b1=1;
  }
  if(time3>time4)
  {
    c1=0;
  }
  else{
    c1=1;
  }
  if(time4>time5)
  {
    d1=0;
  }
  else{
    d1=1;
  }
  if(time1>time5)
  {
    e1=0;
  }
  else{
    e1=1;
  }



  if((time1==0) && (time2==0) && (time3==0)&&(time4==0)&&(time5==0))
  {
    analogWrite(19,255);
    test=1;
  }

  else if((time1==time2) && (time1==time2) && (time1==time3)&& (time1==time4)&&(time1==time5))
  {
    analogWrite(19,datar[11]);
    test=2;

  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  if((timenow>=time1) && (a1==1) && (timenow<time2))
  {
    analogWrite(19,datar[11]);
    test=3;
  }
  else if((a1==0) &&(timenow<time2) && (timenow<720) && (timenow<time1))//////////////////////////////repeat same for other loops also 20.07.2017
  {
    analogWrite(19,datar[11]);
    test=4;
  }
  else if((a1==0) &&(timenow>time2) && (timenow>720)&&(timenow>=time1))
  {
    analogWrite(19,datar[11]);
    test=5;
  }
  /////////////////////////////////////////////////////////////////////////////////////////////

  if((timenow>=time2) && (b1==1) && (timenow<time3))
  {
    analogWrite(19,datar[12]);
    test=6;
  }
  else if((b1==0) &&(timenow<time3) && (timenow<720) && (timenow<time2))
  {
    analogWrite(19,datar[12]);
    test=7;
  }
  else if((b1==0) &&(timenow>time3) && (timenow>720) && (timenow>=time2))
  {
    analogWrite(19,datar[12]);
    test=8;
  }
  //////////////////////////////////////////////////////////////////////////////////
  if((timenow>=time3) && (c1==1) && (timenow<time4))
  {
    analogWrite(19,datar[13]);
    test=9;
  }
  else if((c1==0) &&(timenow<time4) && (timenow<720) && (timenow<time3))
  {
    analogWrite(19,datar[13]);
    test=10;
  }
  else if((c1==0) &&(timenow>time4) && (timenow>720) && (timenow>=time3))
  {
    analogWrite(19,datar[13]);
    test=11;
  }
  ////////////////////////////////////////////////////////////////////////////////////
  if((timenow>=time4) && (d1==1) && (timenow<time5))
  {
    analogWrite(19,datar[14]);
    test=12;
  }
  else if((d1==0) &&(timenow<time5) && (timenow<720) && (timenow<time4))
  {
    analogWrite(19,datar[14]);
    test=13;
  }
  else if((d1==0) &&(timenow>time5) && (timenow>720) && (timenow>=time4))
  {
    analogWrite(19,datar[14]);
    test=14;

  }
  //////////////////////////////////////////////////////////////////////////////////////OFF

  if((timenow>=time5) && (timenow>time1) && (e1==1)&&(time1!=time5))
  {
    analogWrite(19,255);
    test=15;
  }
  else if((timenow<time5) && (timenow<time1) && (e1==1)&&(time1!=time5))
  {
    analogWrite(19,255);
    test=16;

  }
  else if((e1==0) &&(timenow<time1) && (timenow<720) && (timenow>=time5))
  {
    analogWrite(19,255);
    test=17;

  }
  else if((e1==0) &&(timenow<time1) && (timenow>720) && (timenow>=time5))
  {
    analogWrite(19,255);
    test=18;
  } 

  delay(50);
}


////////////////////////////////////End of Void Loop
////////////////////////////////////Library Definition 
void writeAddress(int address, byte val)
{
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB


  Wire.write(val);
  Wire.endTransmission();

  delay(5);
}

byte readAddress(int address)
{
  byte rData = 0xFF;

  Wire.beginTransmission(EEPROM_I2C_ADDRESS);

  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB
  Wire.endTransmission();  


  Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);  

  rData =  Wire.read();

  return rData;
}
//////////////////////////////////////////////////End of Library Definition
