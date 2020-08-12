#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
/* the defenation of ardiuno as websocket */
/*本程序用于带网卡接口的uno板
继电器均接NO 常开状态，即断路不通电
analogRead(pin);
*/
#define RELAY_ADDR 0x6D // Default address - open jumper.wall adaptor 7-15V

//料仓旋钮2  X10  6
//料仓旋钮3  X06  7
//料仓旋钮4  X07  8 
//料仓旋钮5  X11  9

// Here are the commands to turn on and off individual relays.
#define TG_ONE 0x01   //ST301 总停  初始状态 闭路
#define TG_TWO 0x02   //K300  总启  初始状态 开路
#define TG_THREE 0x03 //ST303 停止  初始状态 闭路
#define TG_FOUR 0x04  //K301  启动  初始状态 开路
#define ONE_STATUS 0x05
#define TWO_STATUS 0x06
#define THREE_STATUS 0x07
#define FOUR_STATUS 0x08
String str;//to receive input string
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  pinMode(6, INPUT);  pinMode(7, INPUT); 
  pinMode(8, INPUT);  pinMode(9, INPUT);  
  Serial.println("Arduino Restart");
}

void loop()
{
  int op1=digitalRead(6);  
  int op2=digitalRead(7);
  int op3=digitalRead(8);
  int op4=digitalRead(9);
 //继电器初始化  全部关闭
  Wire.beginTransmission(RELAY_ADDR);// Change the Wire.write statment below to the desired relay you want to check on.
  Wire.write(ONE_STATUS);  
  Wire.endTransmission();
  Wire.requestFrom(RELAY_ADDR, 4);
  int i[4];
  int j=0;
  while (Wire.available()) {
   int r=Wire.read();
   Serial.print("Relay");
   Serial.print(j+1);
   Serial.print(":");
   Serial.println(r);          
   if (r != 0) i[j]=1;
    j++;
   }
   if(j==4){
    if(i[0]!=op1){
     Wire.beginTransmission(RELAY_ADDR);
     Wire.write(TG_ONE);
     Wire.write(ONE_STATUS);
     Wire.endTransmission();
    }
    if(i[1]!=op2){
     Wire.beginTransmission(RELAY_ADDR);
     Wire.write(TG_TWO);
     Wire.write(TWO_STATUS);
     Wire.endTransmission();
    }
    if(i[2]!=op3){
     Wire.beginTransmission(RELAY_ADDR);
     Wire.write(TG_THREE);
     Wire.write(THREE_STATUS);
     Wire.endTransmission();
    }
    if(i[3]!=op4){
     Wire.beginTransmission(RELAY_ADDR);
     Wire.write(TG_FOUR);
     Wire.write(FOUR_STATUS);
     Wire.endTransmission();
    }
   }
  delay(300);
}
