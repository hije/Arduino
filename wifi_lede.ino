#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

byte pwmVal_up_y = 0;
byte pwmVal_up_w = 0;
byte pwmVal_down_y = 0;

bool pwsw = 0; //开关状态变换检测
int vol;
String comdata = "";



void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(3, OUTPUT);   //将引脚变成输出模式，上白色
  pinMode(5, OUTPUT);   //将引脚变成输出模式，下黄色
  pinMode(6, OUTPUT);   //将引脚变成输出模式，上黄色
  while (Serial.read() >= 0) {} //clear serialbuffer


  //analogWrite(3,pwmVal);
  //analogWrite(5,4);
  //analogWrite(6,4);


  chl_up_w(0);
  chl_up_y(0);
  chl_down_y(0);


}

void loop() {


  //  Serial.println(stage);

  if (analogRead(7) > 600)

  {

    if (pwsw == 0) //如果有改变则执行，而不是一直执行
    {
      chl_up_w(255);
      //chl_up_y(255);
      chl_down_y(255);
    }
    pwsw = 1;
  } else
  {
    if (pwsw == 1)//如果有改变则执行，而不是一直执行
    {
      chl_up_w(0);
      chl_up_y(0);
      chl_down_y(0);
    }

    pwsw = 0;
  }


  if (mySerial.available() > 0)
  {
    comdata = mySerial.readString();


              if (comdata.toInt() < 1000)
              {
                //null
              } else if (comdata.toInt() < 3000 and comdata.toInt() >= 1000 )
              {
                Serial.println("1");
                chl_up_w(byte(comdata.toInt() - 2000));
          
          
              } else if (comdata.toInt() < 4000) {
                Serial.println("2");
                chl_up_y(byte(comdata.toInt() - 3000));
          
              } else if (comdata.toInt() < 5000) { //>3000
                Serial.println("3");
                chl_down_y(byte(comdata.toInt() - 4000));
              } else if (comdata.toInt() == 5001) {
                chl_up_w(255);
                chl_up_y(0);
                chl_down_y(0);
          
          
              } else if (comdata.toInt() == 5002) {
                chl_up_w(255);
                chl_up_y(0);
                chl_down_y(255);
          
          
              } else if (comdata.toInt() == 5003) {
                chl_up_w(0);
                chl_up_y(255);
                chl_down_y(255);
          
          
              } else if (comdata.toInt() == 5004) {
                chl_up_w(0);
                chl_up_y(255);
                chl_down_y(0);
          
          
              } else if (comdata.toInt() == 5005) {
                chl_up_w(255);
                chl_up_y(255);
                chl_down_y(255);
          
          
              } else if (comdata.toInt() == 5006) {
                chl_up_w(0);
                chl_up_y(0);
                chl_down_y(0);
            }



}
comdata = "";



// digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//analogWrite(3,0);


}


//------------------------------------------------------------------------------------
void chl_up_w(int val)
{

  // Serial.println(val);
  if (val >= pwmVal_up_w)
  {
    while (val > pwmVal_up_w)
    {
      pwmVal_up_w++;
      delay(5);
      analogWrite(3, pwmVal_up_w);
    }
  } else {
    while (val < pwmVal_up_w)
    {
      pwmVal_up_w--;
      delay(5);
      analogWrite(3, pwmVal_up_w);
    }

  }


}


void chl_up_y(int val)
{

  // Serial.println(val);
  if (val >= pwmVal_up_y)
  {
    while (val > pwmVal_up_y)
    {
      pwmVal_up_y++;
      delay(5);
      analogWrite(6, pwmVal_up_y);
    }
  } else {
    while (val < pwmVal_up_y)
    {
      pwmVal_up_y--;
      delay(5);
      analogWrite(6, pwmVal_up_y);
    }

  }


}

void chl_down_y(int val)
{

  // Serial.println(val);
  if (val >= pwmVal_down_y)
  {
    while (val > pwmVal_down_y)
    {
      pwmVal_down_y++;
      delay(5);
      analogWrite(5, pwmVal_down_y);
    }
  } else {
    while (val < pwmVal_down_y)
    {
      pwmVal_down_y--;
      delay(5);
      analogWrite(5, pwmVal_down_y);
    }

  }


}
