#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
/* the defenation of ardiuno as websocket */
/*本程序用于带网卡接口的uno板
继电器均接NO 常开状态，即断路不通电
analogRead(pin);
*/
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x49, 0xBD };         //physical mac address
byte ip[] = { 192, 168, 1, 166 };                   // ip in lan
byte subnet[] = { 255, 255, 255, 0 };              //subnet mask
byte gateway[] = { 192, 168, 1, 1 };              // default gateway
EthernetServer server(8081);
#define RELAY_ADDR 0x6D // Default address - open jumper.wall adaptor 7-15V

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
int currentstate=0;  //0代表启电没有动作或手动控制，1代表自动控制
int a[2];
void setup()
{
  Ethernet.begin(mac, ip, gateway, subnet);         // initialize Ethernet device
  server.begin();
  Wire.begin();
  Serial.begin(9600);
  pinMode(2, INPUT);  pinMode(3, INPUT);
  Serial.println("Arduino2 Restart");
  Relayinit();
}

void loop()
{
  EthernetClient client = server.available(); // look for the client
  if (client) {
    //Serial.println("Find a client");
    while (client.connected()) {  
      if (client.available()) {
        char c = "";
        str="";
        while (c != '\n') {
          c = client.read();
          str += c; //store characters to string
        }
        Serial.println(str);
      }
      //设备状态获取
      if(str.indexOf("GetState")>=0){
        Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int j=1;
        String result="{";
        while (Wire.available()) {
          int r=Wire.read();
          Serial.print("Relay");
          Serial.print(j);
          Serial.print(":");
          Serial.println(r);           
          result=result+"\"Relay"+j+"\":"+r+",";
          j++;
        }      
        client.print(result);  
      }
      else if (str.indexOf("VibrationStart") >= 0)   //振动开始
      {
        Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          int r=Wire.read();
          if (r != 0) i[j]=1;
          j++;
	   }
        if(i[0]==0){  
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_ONE);
          Wire.write(ONE_STATUS);
          Wire.endTransmission();
       }
	   //Serial.println("VibrationStart"); 
       break;
      }
      else if (str.indexOf("VibrationEnd") >= 0)   //振动结束
      {
       Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          int r=Wire.read();
          if (r != 0) i[j]=1;
          j++;
	   }
        if(i[0]==1){  
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_ONE);
          Wire.write(ONE_STATUS);
          Wire.endTransmission();
       }
       //Serial.println("VibrationEnd");
       break;
      }
       else if (str.indexOf("BellStart") >= 0)   //警铃开始
       {
        Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          int r=Wire.read();
          if (r != 0) i[j]=1;
          j++;
	      }
        if(i[1]==0){  
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_TWO);
          Wire.write(TWO_STATUS);
          Wire.endTransmission();
        }
        Serial.println("BellStart");
        break;
       }
       else if (str.indexOf("BellEnd") >= 0)   //警铃结束
       {
         Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          int r=Wire.read();
          if (r != 0) i[j]=1;
          j++;
	   }
        if(i[1]==1){  
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_TWO);
          Wire.write(TWO_STATUS);
          Wire.endTransmission();
       }
        Serial.println("BellEnd");
        break;
       }
      
    }
    client.stop();
  }
     

  int op1=digitalRead(2);  
  int op2=digitalRead(3);
  Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          int r=Wire.read();
          if (r != 0) i[j]=1;
          j++;
	      }
        if(i[2]!=op1){  
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_THREE);
          Wire.write(THREE_STATUS);
          Wire.endTransmission();
        }
        if(i[3]!=op2){  
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_FOUR);
          Wire.write(FOUR_STATUS);
          Wire.endTransmission();
        }
  
    delay(1000);
  ////Serial.println("**************");
}

void Relayinit(){
  int op1=digitalRead(2);  
  int op2=digitalRead(3);
 //继电器初始化  全部关闭
  Wire.beginTransmission(RELAY_ADDR);// Change the Wire.write statment below to the desired relay you want to check on.
  Wire.write(ONE_STATUS);  
  Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
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
        //Serial.println(j);
        if(j==4){
        if(i[0]==1){
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_ONE);
          Wire.write(ONE_STATUS);
          Wire.endTransmission();
          }
        if(i[1]==1){
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_TWO);
          Wire.write(TWO_STATUS);
          Wire.endTransmission();
          }
        if(i[2]!=op1){
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_THREE);
          Wire.write(THREE_STATUS);
          Wire.endTransmission();
          }
        if(i[3]!=op2){
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_FOUR);
          Wire.write(FOUR_STATUS);
          Wire.endTransmission();
          }
        }
}
