#include <SoftwareSerial.h>;
#include <IRremote.h>
#include <IRremoteInt.h>
#include <PS2X_lib.h>
#include <Servo.h>


//Drive Commands

byte Rstop[6]={137,0,0,0,0}; //stopping the drive motors is just a "zero" speed selection
byte Rleft[6]={137,2,0,0,1}; //plus 1 radius means turn in place - right
byte Rright[6]={137,2,0,255,-1};  //-1 radius means turn in place - left
byte Rfwd[6]={137,1,244,0x80,0};    //radius hex 8000 is straight
byte Rback[6]= {137,254,12,0x80,0};// testing here (285,84)
byte RCircleR[6]={137,0,100,0,128}; //Circle Right
byte RCircleL[6]= {137,0,100,-1,-128}; //Circle Left
byte Song0[17]={140,0,7,31,60,43,55,55,50,67,45,79,40,91,35,103,30};
byte Song1[35]={140,1,16, 36,20, 37,20, 0,40,  36,20, 37,20, 0,30,  36,20, 37,20, 0,20,  36,20, 37,20, 0,10,  36,20, 37,20, 0,5,  38,64}; //Jaws
byte Song9[33]={140,9,15, 36,20, 37,20, 0,65,  36,20, 37,20, 0,60,  36,20, 37,20, 0,55,  36,20, 37,20, 0,50,  36,20, 37,20, 0,45};
byte Song2[13]={140,2,5,79,48,81,48,77,48,65,50,72,64}; //Strange Encounters 1
byte Song3[13]={140,3,5,69,48,71,48,67,48,55,48,62,64}; //Strange Encounters 2
byte Song15[11]={140,15,4,33,16,45,20,43,12,31,40};
byte RoombaPlay[2] = {141,1};  //songs on 0,1,3 not sure why there isnt anything on 2
byte RoombaMotorsOff[2] = {138,0};  //motors off code (brushes, vac)
byte Battery[3] = {148,1,25};
byte RoombaOff = 133;
byte zero = 0;

// testing new stuff

byte Rwheel[6]={145,1,244,0,0};
byte Lwheel[6]={145,0,0,1,244};
byte Rwheelstop[6]={145,0,0,0,0};
byte Lwheelstop[6]={145,0,0,0,0};

byte Lighton[4]={139,0,255,255};

int RECV_PIN = 2;          //  The digital pin that the signal pin of the sensor is connected to
IRrecv receiver(RECV_PIN);  //  Create a new receiver object that would decode signals to key codes
decode_results results;     //  A varuable that would be used by receiver to put the key code into

//ps2 controller
PS2X ps2x;

Servo RXservo;
Servo RYservo;

int error = 0; 
bool pwr = false;
byte type = 0;
byte vibrate = 0;

//end of new stuff

int ptr, cnt, cmd;
char inbuf[128], cin;  //I tried buffering input from Roomba, but that dropped characters, the tight loop works better
unsigned long t1,t2;
boolean play = false;
SoftwareSerial mySerial(10, 11); // RX, TX (instance of serial object)

// Initialization Code ============================================

void setup(){
  // Open serial communications and wait for port to open:

  Serial.begin(115200);
  Serial.println("Menu 1");

  // set the data rate for the SoftwareSerial port

  mySerial.begin(115200);
  ptr=0;
  cnt=0;
  t1=millis();

  receiver.enableIRIn();

  //ps2 controller
  RXservo.attach(31);
  RYservo.attach(33);


  error = ps2x.config_gamepad(5,4,3,6, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

  if(error == 0)
    Serial.println("Controller found! You may now send commands");

  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  type = ps2x.readType(); 
  switch(type) 
  {
  case 0:
    Serial.println("Unknown Controller type");
    break;
  case 1:
    Serial.println("DualShock Controller Found");
    break;
  }
}

// Main Loop ======================================================

void loop()

{
  //oldcmd();   // also not needed
  //oldIR();   //not needed anymore but i dont want to delete it
  
  if (mySerial.available())
    Serial.write(mySerial.read());

  t2=millis()-t1;  //T2 is time since last t1 reset

  if (Serial.available()){
    cmd = Serial.read();
  }


//ps2 time

if(error == 1) //skip loop if no controller found
    return; 

  else { //DualShock Controller
    ps2x.read_gamepad(false, vibrate);

    RXservo.write(map(ps2x.Analog(PSS_RX), 0, 255, 0, 179));
    RYservo.write(map(ps2x.Analog(PSS_RY), 0, 255, 0, 179));

    if ((ps2x.Button(PSB_SELECT)) and (pwr == false)){
        mySerial.write(128);
        delay(20);
        mySerial.write(132); //full
        delay(20);  
        Serial.println("Roomba Initialized");
        mySerial.write(Lighton,4);
        mySerial.write(Song1,35);
        delay(1000);
        pwr = true;
      }
      else if ((ps2x.Button(PSB_SELECT)) and (pwr == true))
      {
        mySerial.write(RoombaOff);
        Serial.write("off");
        delay(1000);
        pwr = false;
      }
    if (ps2x.Button(PSB_R1)){
      if (ps2x.Button(PSB_PAD_UP))
      {
        Serial.write("Forward \n");
        mySerial.write(Rfwd,6);
      }
      else if (ps2x.Button(PSB_PAD_DOWN))
      {
        Serial.write("Backward \n");
        mySerial.write(Rback,6);
      }
      else if (ps2x.Button(PSB_PAD_LEFT))
      {
        Serial.write("Left \n");
        TurnLeft();
      }
      else if (ps2x.Button(PSB_PAD_RIGHT))
      {
        Serial.write("Right \n");
        TurnRight();
      }
      else if (ps2x.Button(PSB_CROSS))
      {
        Serial.println("X");
        mySerial.write(Rstop,6);
      }
      else 
      {
        // do nothing
      }
    }else{
      mySerial.write(Rstop,6);
    }

    if(ps2x.Analog(PSS_LY) >= 136 && ps2x.Analog(PSS_LY) <= 255)//real center value is 128, but 140 is needed because controller is HIGHLY sensitive
    {
      Serial.write("1 \n");
    }
    else if(ps2x.Analog(PSS_LY) >= 0 && ps2x.Analog(PSS_LY) <= 120) //Same as above
    {
      Serial.write("2 \n");
    }  
    else {} 
  }

  delay(50);

}