#include <AltSoftSerial.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <math.h>
#include <Wire.h>

const String EMERGENCY_PHONE = "+918595579084";
//GSM Module Conected Pin D2 & D3
SoftwareSerial gsmSerial(6, 7);  // RX, TX for GSM module

//GPS Modue Conected Pin D9 & D
TinyGPSPlus gps;
SoftwareSerial neogps(9,8);




String sms_status, sender_number, received_date, msg;
String latitude, longitude;

#define BUZZER 12
#define BUTTON 11
#define xPin A0
#define yPin A1
#define zPin A2

byte updateflag;

int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 2, devibrate = 75;
int magnitude = 0;
int sensitivity = 100;
double angle;
boolean impact_detected = false;
unsigned long time1;
unsigned long impact_time;
unsigned long alert_delay = 30000;

void setup() {

  Serial.begin(9600);
gsmSerial.begin(9600);
  Serial.println("SIM800L serial initialize");
  //sim800.begin(9600);
  Serial.println("NEO6M serial initialize");




  neogps.begin(9600);
  
  
  
  
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  sms_status = "";
  sender_number = "";
  received_date = "";
  msg = "";

  // sim800.println("AT");
  // delay(1000);
  // sim800.println("ATE1");
  // delay(1000);
  // sim800.println("AT+CPIN?");
  // delay(1000);
  // sim800.println("AT+CMGF=1");
  // delay(1000);
  // sim800.println("AT+CNMI=1,1,0,0,0");
  // delay(1000);
  gsmSerial.println("AT");
  delay(2000);
  gsmSerial.println("AT+CMGF=1");  // Set SMS mode to text
  delay(2000);
  time1 = micros();
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
}

void loop() {

  if (micros() - time1 > 1999) Impact();

  if (updateflag > 0) {
    updateflag = 0;
    Serial.println("Impact detected!!");
    Serial.print("Magnitude:");
    Serial.println(magnitude);

    getGps();
    digitalWrite(BUZZER, HIGH);
    impact_detected = true;
    impact_time = millis();

  }

  if (impact_detected == true) {
    if (millis() - impact_time >= alert_delay) {
      digitalWrite(BUZZER, LOW);
     
      delay(2000);
      sendAlert();
      impact_detected = false;
      impact_time = 0;
    }
  }

  if (digitalRead(BUTTON) == LOW) {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;
  }
  while (gsmSerial.available()) {
    parseData(gsmSerial.readString());
  }
  while (Serial.available()) {
    gsmSerial.println(Serial.readString());
  }
}

void Impact() {

  time1 = micros();
  int oldx = xaxis;
  int oldy = yaxis;
  int oldz = zaxis;

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);

  vibration--;
  if (vibration < 0) vibration = 0;

  if (vibration > 0) return;
  deltx = xaxis - oldx;
  delty = yaxis - oldy;
  deltz = zaxis - oldz;

  magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));
  if (magnitude >= sensitivity) {
    updateflag = 1;
    vibration = devibrate;
  } else {
    if (magnitude > 15)
      Serial.println(magnitude);
    magnitude = 0;
  }
}

void parseData(String buff) {
  Serial.println(buff);

  unsigned int len, index;
  index = buff.indexOf("\r");
  buff.remove(0, index + 2);
  buff.trim();

  if (buff != "OK") {

    index = buff.indexOf(":");
    String cmd = buff.substring(0, index);
    cmd.trim();

    buff.remove(0, index + 2);

    if (cmd == "+CMTI") {

      index = buff.indexOf(",");
      String temp = buff.substring(index + 1, buff.length());
      temp = "AT+CMGR=" + temp + "\r";
      gsmSerial.println(temp);
    }

    else if (cmd == "+CMGR") {

      if (buff.indexOf(EMERGENCY_PHONE) > 1) {
        buff.toLowerCase();
        if (buff.indexOf("get gps") > 1) {
          getGps();
          String sms_data;
          sms_data = "GPS Location Data\r";
          sms_data += "http://maps.google.com/maps?q=loc:";
          sms_data += latitude + "," + longitude;

          sendSms(sms_data);
        }
      }
    }

  } else {
  }
}

void getGps() {

  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;) {
    while (neogps.available()) {
      if (gps.encode(neogps.read())) {
        newData = true;
        break;
      }
    }
  }

  if (newData) {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
    newData = false;
  } else {
    Serial.println("No GPS data is available");
    latitude = "";
    longitude = "";
  }

  Serial.print("Latitude= ");
  Serial.println(latitude);
  Serial.print("Longitude= ");
  Serial.println(longitude);
}

void sendAlert() {
  String s;
  s = "Accident Alert!!\r";
  s += "http://maps.google.com/maps?q=loc:";
  s += latitude + "," + longitude;
  
  sendSms(s);
 

}

void sendSms(String s) {
   gsmSerial.println("AT+CMGS=\"+918306211581\"");  // Replace with the recipient's phone number
    delay(1000);
    gsmSerial.print(s); //content
    delay(1000);
    gsmSerial.write(26);  // End the SMS with Ctrl+Z
}


boolean SendAT(String at_command, String expected_answer, unsigned int timeout) {

  uint8_t x = 0;
  boolean answer = 0;
  String response;
  unsigned long previous;

  //Clean the input buffer
  while (gsmSerial.available() > 0) gsmSerial.read();

  gsmSerial.println(at_command);

  x = 0;
  previous = millis();

  do {

    if (gsmSerial.available() != 0) {
      response += gsmSerial.read();
      x++;
      if (response.indexOf(expected_answer) > 0) {
        answer = 1;
        break;
      }
    }
  } while ((answer == 0) && ((millis() - previous) < timeout));

  Serial.println(response);
  return answer;
}
