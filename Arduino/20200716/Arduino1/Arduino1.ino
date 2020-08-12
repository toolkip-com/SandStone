#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
/* the defenation of ardiuno as websocket */
/*本程序用于带网卡接口的uno板
继电器均接NO 常开状态，即断路不通电
analogRead(pin);
*/
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x51, 0xBD };         //physical mac address
byte ip[] = { 192, 168, 1, 167 };                   // ip in lan
byte subnet[] = { 255, 255, 255, 0 };              //subnet mask
byte gateway[] = { 192, 168, 1, 1 };              // default gateway
EthernetServer server(8081);
#define RELAY_ADDR 0x6D // Default address - open jumper.wall adaptor 7-15V

//料仓旋钮1  X12  3
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
int currentstate=0;  //0代表启电没有动作或手动控制，1代表自动控制
int a[7];
void setup()
{
  Ethernet.begin(mac, ip, gateway, subnet);         // initialize Ethernet device
  server.begin();
  Wire.begin();
  Serial.begin(9600);
  pinMode(2, OUTPUT);  pinMode(3, OUTPUT); 
  pinMode(6, OUTPUT);  pinMode(7, OUTPUT);  
  pinMode(8, OUTPUT);  pinMode(9, OUTPUT); 
  
  Serial.println("Arduino Restart");
  Relayinit();
  delay(1000);
  XButtoninit();
  for(int i=0;i<7;i++) a[i]=0;   a[0]=1;
  delay(1000);
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
        int k=0,silo=0;
        if(a[0]==1) silo=1;
        if(a[1]==1) silo=2;
        if(a[2]==1) silo=3;
        if(a[3]==1) silo=4;
        if(a[4]==1) silo=5;
        result=result+"\"silo\":"+silo;
        result=result+",\"currentstate\":"+currentstate;
        result=result+",\"Vibration\":"+a[5];
        result=result+",\"Bell\":"+a[6];
        result=result+"}";
        client.print(result);  
      }
      else if (str.indexOf("Manual") >= 0)   //手动状态
      {
        currentstate=0; 
        XButtoninit();       
        Relayinit();
        //Serial.println("Manual");
        //client.println("Manual");
        break;
      }
      else if (str.indexOf("Auto") >= 0)
      {
        currentstate=1;
        //关闭5个仓筒的通电        
        digitalWrite(3, HIGH);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
        digitalWrite(8, LOW); 
        digitalWrite(9, LOW);         
        for(int i=0;i<7;i++) a[i]=0;  a[0]=1;
        digitalWrite(2, LOW);  //原com断路
        break;
      }
      else if (str.indexOf("Silo1Start") >= 0 && currentstate==1)  //切换至筒仓1
      {       
        digitalWrite(3, HIGH);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
        digitalWrite(8, LOW); 
        digitalWrite(9, LOW); 

        digitalWrite(2, LOW);
        for(int i=0;i<7;i++) a[i]=0; a[0]=1;
        //Serial.println("Silo1Start"); 
        break;
      }
      else if (str.indexOf("Silo2Start") >= 0&& currentstate==1)   //切换至筒仓2
      {
        digitalWrite(6, HIGH);
        digitalWrite(3, LOW);
        
        digitalWrite(7, LOW);
        digitalWrite(8, LOW); 
        digitalWrite(9, LOW);  
        
        digitalWrite(2, LOW);
        for(int i=0;i<7;i++) a[i]=0; a[1]=1; 
        //Serial.println("Silo2Start"); 
        break;
      }
      else if (str.indexOf("Silo3Start") >= 0&& currentstate==1)  //切换至筒仓3
      {
        digitalWrite(7, HIGH);
        digitalWrite(3, LOW);
        digitalWrite(6, LOW);
        
        digitalWrite(8, LOW); 
        digitalWrite(9, LOW); 

         digitalWrite(2, LOW);
        for(int i=0;i<7;i++) a[i]=0; a[2]=1;
        //Serial.println("Silo3Start"); 
        break;
      }
      else if (str.indexOf("Silo4Start") >= 0&& currentstate==1)  //切换至筒仓4
      {
        digitalWrite(8, HIGH); 
        digitalWrite(3, LOW);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
        
        digitalWrite(9, LOW);  

        digitalWrite(2, LOW);
        for(int i=0;i<7;i++) a[i]=0; a[3]=1;
        //Serial.println("Silo4Start"); 
        break;
      }
      else if (str.indexOf("Silo5Start") >= 0&& currentstate==1)   //切换至筒仓5
      {
        digitalWrite(9, HIGH); 
        digitalWrite(3, LOW);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
        digitalWrite(8, LOW);          

        digitalWrite(2, LOW);
        for(int i=0;i<7;i++) a[i]=0; a[4]=1;
        //Serial.println("Silo5Start");  
        break;
      }     
      else if (str.indexOf("ALLStart") >= 0&& currentstate==1)    //总启
      {       
        Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          int r=Wire.read();
          //Serial.print("Relay");
          //Serial.print(j+1);
          //Serial.print(":");
          //Serial.println(r); 
          if (r != 0) i[j]=1;
          j++;
        }
        if(i[0]==0){  //总停 闭路
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_ONE);
          Wire.write(ONE_STATUS);
          Wire.endTransmission();
          }
        if(i[1]==0){  //总启 闭路
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_TWO);
          Wire.write(TWO_STATUS);
          Wire.endTransmission();
          } 
        //Serial.println("ALLStart"); 
        if(a[0]==0&&a[1]==0&&a[2]==0&&a[3]==0&&a[4]==0) a[0]=1;
        digitalWrite(3, a[0]);
        digitalWrite(6, a[1]);
        digitalWrite(7, a[2]);
        digitalWrite(8, a[3]); 
        digitalWrite(9, a[4]); 
        
        digitalWrite(2, LOW);
        
               
        break;
      }
      else if (str.indexOf("ALLEnd") >= 0&& currentstate==1)  //总停
      {
        Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        //料仓
        digitalWrite(2, LOW);
        
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          int r=Wire.read();
          //Serial.print("Relay");
          //Serial.print(j+1);
          //Serial.print(":");
          //Serial.println(r); 
          if (r != 0) i[j]=1;
          j++;
        }
        if(i[0]==1){  //总停 开路
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_ONE);
          Wire.write(ONE_STATUS);
          Wire.endTransmission();
          }
        if(i[1]==1){  //总启 开路
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_TWO);
          Wire.write(TWO_STATUS);
          Wire.endTransmission();
          }
        //Serial.println("ALLEnd"); 
        break;
      }
      else if (str.indexOf("StartRun") >= 0&& currentstate==1)   //启动
      {
        Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          if (Wire.read() != 0) i[j]=1;
          j++;
        }
        if(i[2]==0){  //停 闭路
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_THREE);
          Wire.write(THREE_STATUS);
          Wire.endTransmission();
          }
        if(i[3]==0){  //启 闭路
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_FOUR);
          Wire.write(FOUR_STATUS);
          Wire.endTransmission();
          }
        //Serial.println("StartRun"); 
        break;
      }
      else if (str.indexOf("EndRun") >= 0&& currentstate==1)   //停止
      {
        Wire.beginTransmission(RELAY_ADDR);
        Wire.write(ONE_STATUS);
        Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          if (Wire.read() != 0) i[j]=1;
          j++;
        }
        if(i[2]==1){  //停 开路
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_THREE);
          Wire.write(THREE_STATUS);
          Wire.endTransmission();
          }
        if(i[3]==1){  //启 开路
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_FOUR);
          Wire.write(FOUR_STATUS);
          Wire.endTransmission();
          }
        //Serial.println("EndRun"); 
        break;
      }
    
    }
    client.stop();
  }
  if (currentstate == 0){
      //手动状态  或  默认进入无操作时
      XButtoninit();
  }
    delay(1000);
}

void XButtoninit()
{
  digitalWrite(2, HIGH); //旋钮com通电
  digitalWrite(3, LOW);  //X12不通电
  digitalWrite(6, LOW);  //X10不通电
  digitalWrite(7, LOW);  //X06不通电
  digitalWrite(8, LOW);  //X07不通电
  digitalWrite(9, LOW);  //X11不通电  
  for(int i=0;i<7;i++) a[i]=0; 
  ////Serial.println("**************");
}

void Relayinit(){
 //继电器初始化
  Wire.beginTransmission(RELAY_ADDR);// Change the Wire.write statment below to the desired relay you want to check on.
  Wire.write(ONE_STATUS);  
  Wire.endTransmission();
        Wire.requestFrom(RELAY_ADDR, 4);
        int i[4]; i[0]=0;  i[1]=0;  i[2]=0;  i[3]=0;  //0 断开   1闭合
        int j=0;
        while (Wire.available()) {
          int r=Wire.read();
          //Serial.print("Relay");
          //Serial.print(j+1);
          //Serial.print(":");
          //Serial.println(r);          
          if (r != 0) i[j]=1;
          j++;
        }
        //Serial.println(j);
        if(j==4){
        if(i[0]==0){
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
        if(i[2]==0){
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_THREE);
          Wire.write(THREE_STATUS);
          Wire.endTransmission();
          }
        if(i[3]==1){
          Wire.beginTransmission(RELAY_ADDR);
          Wire.write(TG_FOUR);
          Wire.write(FOUR_STATUS);
          Wire.endTransmission();
          }
        }
}
