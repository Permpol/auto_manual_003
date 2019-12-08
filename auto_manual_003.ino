// #include <WiFi.h>
// #include <WiFiClient.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>


char auth[] = "_fiZLMlFG6EP8XymaMiGeJMABZ22SdrG";
char ssid[] = "55";
char pass[] = "12345678";
char server[] = "oasiskit.com";
unsigned port = 8080;

BlynkTimer timer;

BLYNK_CONNECTED(){
    Blynk.syncAll();
}


//ปุ่ม Blynk
#define Widget_Btn_00 V0    //ปุ่ม Auto/Manual
   //Terminal
#define Widget_Btn_Pump V2  //ปุ่ม เปิด-ปิด ปั๊มน้ำ
#define Widget_SoilValue V4 //แสดงค่าความชื่นในดิน
#define Widget_Slider V5 //ตัวตั้งค่าความชื่นในดิน

//สถานะปุ่ม Blynk
#define Widget_LED_Pump V3  //ไฟสถานะปั๊มน้ำ
#define Widget_Btn_01 V1 //ไฟสถานะ AUTO/MANUAL
WidgetLED LedBlynkPump(Widget_LED_Pump);
WidgetLED LedBlynkAM(Widget_Btn_01);

//Pin INPUT
#define SoilSensor_1 36
#define SoilSensor_2 39
#define SoilSensor_3 34
#define SoilSensor_4 35


//Pin OUTPUT
#define Relay1_pump  32

//ประกาศตัวแปร
int SoilSensorValue; //ค่าเฉลี่่ยของ soil sensor
int btn_auto_manual = 0; //สถานะปุ่ม auto(1)/manual(2) 
bool auto1 = false;
int setSoil = 50; //ค่า default ของการตั้งค่า ความชื่นในดิน


void setup(){
    Serial.begin(115200);
    // WiFi.begin(ssid, pass);
    // while (WiFi.status() != WL_CONNECTED){
    //     delay(500);
    //     Serial.print(".");
    // }
    // Serial.println("WiFi connected");
    // Serial.print("Connecting to ");
    // Serial.println(ssid);
    Blynk.begin(auth, ssid, pass, server, port);

    //pinMode INPUT
    pinMode(SoilSensor_1, INPUT);
    pinMode(SoilSensor_2, INPUT);
    pinMode(SoilSensor_3, INPUT);
    pinMode(SoilSensor_4, INPUT);

    //pinMode OUTPUT
    pinMode(Relay1_pump, OUTPUT);
    digitalWrite(Relay1_pump, LOW);

    //เรียกใช้ Function
    timer.setInterval(2000, sendSensor);
    timer.setInterval(2000, SoilSensor);
    //timer.setInterval(2000, averagesoilsensorValue);

    Clear(); //เรียกใช้ function Clear
    
}

void loop(){  
    Blynk.run();
    timer.run();
}


void sendSensor(){
    Blynk.virtualWrite(Widget_SoilValue, SoilSensorValue);
}



//*****BUTTON AUTO/MANUAL*********
BLYNK_WRITE(Widget_Btn_00){
    if(param.asInt() == 2){
        // Blynk.setProperty(LedBlynkAM, "color", "#CCCC00");
        // Blynk.setProperty(LedBlynkAM, "label", "AUTO");
        Serial.print("func(BUTTON): "); Serial.print(btn_auto_manual); Serial.println("auto");
        auto1 = true;
        LedBlynkAM.on();
    }else{
        // Blynk.setProperty(LedBlynkAM, "color", "#CCFF00");
        // Blynk.setProperty(LedBlynkAM, "label", "MANUAL"); 
        Serial.print("func(BUTTON): "); Serial.print(btn_auto_manual); Serial.println("manual");
        auto1 = false;
        LedBlynkAM.on();      
    }
}

/*
**
*************************************SOILSENSOR*********************************************
**
*/

BLYNK_WRITE(Widget_Slider){
    int pinValue = param.asInt();
    setSoil = pinValue;
}

//
//*************MANUAL*********
//****BUTTON ON/OFF PUMP****
//
BLYNK_WRITE(Widget_Btn_Pump){
    int value = param.asInt();
    if(value == 1){
        digitalWrite(Relay1_pump, HIGH);
        Blynk.virtualWrite(Widget_Btn_00, 2);
        // Blynk.setProperty(LedBlynkPump, "color", "#CCCC00");
        // Blynk.setProperty(LedBlynkPump, "label", "ทำงาน");              
        LedBlynkPump.on();
    }else{
        digitalWrite(Relay1_pump, LOW);
        Blynk.virtualWrite(Widget_Btn_00, 2);
        // Blynk.setProperty(LedBlynkPump, "color", "#FF0000");
        // Blynk.setProperty(LedBlynkPump, "label", "ไม่ทำงาน");         
        LedBlynkPump.on();               
    }
}

//******AUTO*******

void SoilSensor(){
    static unsigned long timer = millis();
    const unsigned long timeSerialprint = 1000;
// const unsigned long timePumpOn = 10000;
    if(millis() - timer >= timeSerialprint){
        timer = millis(); 
        Serial.print("func(soilsensor)auto(1)/manual(2) : "); Serial.println(btn_auto_manual);    
    }
    int SoilSensorValue_1 = analogRead(SoilSensor_1);
    int SoilSensorValue_2 = analogRead(SoilSensor_2);
    int SoilSensorValue_3 = analogRead(SoilSensor_3);
    int SoilSensorValue_4 = analogRead(SoilSensor_4);
    int mapSoilSensorValue_1 = map(SoilSensorValue_1, 0, 4095, 100, 0);
    int mapSoilSensorValue_2 = map(SoilSensorValue_2, 0, 4095, 100, 0);
    int mapSoilSensorValue_3 = map(SoilSensorValue_3, 0, 4095, 100, 0);
    int mapSoilSensorValue_4 = map(SoilSensorValue_4, 0, 4095, 100, 0);
    SoilSensorValue = ((mapSoilSensorValue_1 + 
                        mapSoilSensorValue_2 + 
                        mapSoilSensorValue_3 + 
                        mapSoilSensorValue_4) / 4);  

    
        if(SoilSensorValue < setSoil ){
            if(auto1 == true){               
                // if(millis() - timer >= timePumpOn){
                //     timer = millis();
                    digitalWrite(Relay1_pump, HIGH);
                    Blynk.virtualWrite(Widget_Btn_Pump, 1);
                    // Blynk.setProperty(LedBlynkPump, "color", "#CCCC00");
                    // Blynk.setProperty(LedBlynkPump, "label", "ทำงาน");
                    LedBlynkPump.on();
                    Serial.println("Pump\tOn");
                // }             
            }
        }

        if(SoilSensorValue > setSoil){
            if(auto1 == true){
                digitalWrite(Relay1_pump, LOW);
                Blynk.virtualWrite(Widget_Btn_Pump, 0);
                // Blynk.setProperty(LedBlynkPump, "color", "#FF0000");
                // Blynk.setProperty(LedBlynkPump, "label", "ไม่ทำงาน");              
                LedBlynkPump.on();
                Serial.println("Pump\tOff");    
            }       
        }
}


void Clear(){
  Blynk.virtualWrite(Widget_Slider, setSoil);
}

/*
**
*************************************SOILSENSOR*********************************************
**
*/
