#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <Adafruit_BMP085.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#include "U8glib.h"
#define DHTPIN 6                                             //定义温湿度针脚号为2号引脚
#define DHTTYPE DHT11                                                      //实例化一个对象
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SGP30 sgp;
Adafruit_BMP085 bmp;
Adafruit_MPU6050 mpu;
/* return absolute humidity [mg/m^3] with approximation formula
  @param temperature [°C]
  @param humidity [%RH]
*/
float acc_x;
float acc_y;
float acc_z;

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
  return absoluteHumidityScaled;
}

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
//void draw(void) {
//  // graphic commands to redraw the complete screen should be placed here
//
//}



void setup() {
  Serial.begin(57600);
  //pinMode(DHT11PIN, OUTPUT);
  Serial.println("HIJE-Air Probe");
  if (! sgp.begin()) {
    Serial.println("Sensor not found :(");
    while (1);
  }
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  //  Serial.print("Found SGP30 serial #");
  //  Serial.print(sgp.serialnumber[0], HEX);
  //  Serial.print(sgp.serialnumber[1], HEX);
  //  Serial.println(sgp.serialnumber[2], HEX);
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  // sgp.setIAQBaseline(0x8480, 0x87F8);  // Will vary for each sensor! -old
  // sgp.setIAQBaseline(0x8C24, 0x87F8);
  //sgp.setIAQBaseline(0x84F3, 0x8F08);
  sgp.setIAQBaseline(0x8f69, 0x98a2);

  Serial.println("Tempeature1(°C)\tHumidity(%RH)\tTempeature2(°C)\tTempeature3(°C)\tAltitude(M)\tPressure(x1000Pa)\tTVOC(ppb)\teCO2(x10ppm)\tAcc(m/s)");                        //打印出Tempeature:
  dht.begin();
}

//int counter = 0;

void loop() {
//蓝牙无线上传、触发重启
 while (Serial.available() > 0) {
    char rece = Serial.read();
    //delay(10);
    if (rece == ' ')//至关重要，判断的结束符合，多次测试出nano为空格。
    {
      Serial.println("reset get...");
      pinMode(A0,OUTPUT);
      digitalWrite(A0, LOW);    // 将reset管脚的电平拉低50ms，起到复位的作用

      delay(50);

      digitalWrite(A0, HIGH); //
     
    }
 }
  
  //int chk = DHT11.read(DHT11PIN);                 //将读取到的值赋给chk

  float tem = dht.readTemperature();             //将温度值赋值给tem
  int hum = dht.readHumidity();              //将湿度值赋给hum
  //pa-altitude
  float bmp_tem = bmp.readTemperature();
  long bmp_pre = bmp.readPressure();
  float bmp_alti = bmp.readAltitude();
  //mpu6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float mpu_tem = temp.temperature;


  Serial.print(tem);                                     //打印温度结果
  Serial.print("\t");

  //Serial.print("\tHumidity:");                            //打印出Humidity:
  Serial.print(hum);                                     //打印出湿度结果
  Serial.print("\t");

  Serial.print(bmp_tem);                                     //
  Serial.print("\t");
  Serial.print(mpu_tem);         //mpu6050-temp
  Serial.print("\t");
  Serial.print(bmp_alti);                                     //
  Serial.print("\t");
  Serial.print(bmp_pre / 1000.00);                                   //
  Serial.print("\t");

  //Serial.println("%");                                  //打印出%
  // If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
//  float temperature = tem; // [°C]
//  float humidity = hum; // [%RH]
//  sgp.setHumidity(getAbsoluteHumidity(temperature, humidity));

  if (! sgp.IAQmeasure()) {
    //   Serial.println("Measurement failed");
    return;
  }
  // Serial.print("TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  // Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");
  int tvoc = sgp.TVOC;
  int eco2 = sgp.eCO2;
  Serial.print(tvoc);
  Serial.print("\t");
  Serial.print(eco2 / 10.00000);
  Serial.print("\t");
  if (! sgp.IAQmeasureRaw()) {
    //    Serial.println("Raw Measurement failed");
    return;
  }
  // Serial.print("Raw H2 "); Serial.print(sgp.rawH2); Serial.print(" \t");
  // Serial.print("Raw Ethanol "); Serial.print(sgp.rawEthanol); Serial.println("");



  //  Serial.print(sgp.rawH2 / 1000.000);
  //  Serial.print("\t");
  //  Serial.println(sgp.rawEthanol / 1000.000);


  float acc_x0 = a.acceleration.x;
  float acc_y0 = a.acceleration.y;
  float acc_z0 = a.acceleration.z;
  float acc_x1 = abs(acc_x0 - acc_x);
  float acc_y1 = abs(acc_y0 - acc_y);
  float acc_z1 = abs(acc_z0 - acc_z);
  
  float max_acc;
  if (acc_x1 >= acc_y1) {
    max_acc = acc_x1;
  } else {
    max_acc = acc_y1;
  }
  if (max_acc <= acc_z1) {
    max_acc = acc_z1;
  }
  acc_x =  acc_x0;
  acc_y =  acc_y0;
  acc_z =  acc_z0;


  Serial.println(max_acc);

  //  Serial.print("\t");
  //  Serial.print(a.acceleration.y);
  //  Serial.print("\t");
  //  Serial.print(a.acceleration.z);


  //
  //  counter++;
  //if (counter == 30) {
  //    counter = 0;

  uint16_t TVOC_base, eCO2_base;
  if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
    //   Serial.println("Failed to get baseline readings");
    return;
  }
  // Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
  //Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);


  // }
  //show oled:
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);
    u8g.setPrintPos(0, 16);
    // call procedure from base class, http://arduino.cc/en/Serial/Print
    u8g.print("T1:"); u8g.print(tem, 1); u8g.print("c Hu:"); u8g.print(hum); u8g.print("%");


    u8g.setPrintPos(0, 27);
    u8g.print("T2:"); u8g.print(bmp_tem, 1); u8g.print(" "); u8g.print("T3:"); u8g.print(mpu_tem); u8g.print("c");
    // u8g.print(DHT11.temperature,0);
    u8g.setPrintPos(0, 39);
    u8g.print("At:"); u8g.print(bmp_alti); u8g.print(" "); u8g.print("@"); u8g.print(max_acc); u8g.print("m");

    //u8g.print(DHT11.humidity,0);
    u8g.setPrintPos(0, 52);
    u8g.print("C2:"); u8g.print(eco2); u8g.print(" P:"); u8g.print(bmp_pre);
    u8g.setPrintPos(0, 63);
    u8g.print("To:"); u8g.print(tvoc); u8g.print(" "); u8g.print(eCO2_base, HEX); u8g.print(" "); u8g.print(TVOC_base, HEX);

  } while ( u8g.nextPage() );

if (eco2>1000 || max_acc> 0.1){
  pinMode(A1,OUTPUT);
        digitalWrite(A1, LOW);    // 将reset管脚的电平拉低50ms，起到复位的作用

      delay(500);

      digitalWrite(A1, HIGH); //
      
       delay(2460);
       
  }else{

 delay(2960);
    
    }

 

}
