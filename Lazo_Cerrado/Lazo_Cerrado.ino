/*
   ****************************** TSC-Lab *******************************
   ***************************** Proyecto  *****************************
   Control de temperatura usando ambos transistores
   Las gráficas se muestran en NodeRed y se reciben los datos por Telegram
   El setpoint es controlado por Matlab
   By: Isaac Zambrano
*/

#include <OneWire.h>
#include <DallasTemperature.h>

//GPIO pin 4 is set as OneWire bus
OneWire ourWire1(0);
//GPIO pin 0 is set as OneWire bus
OneWire ourWire2(4);

//A variable or object is declared for our sensor 1
DallasTemperature sensors1(&ourWire1);
//A variable or object is declared for our sensor 2
DallasTemperature sensors2(&ourWire2);


//pins of transistor
int trans1 = 17;
int trans2 = 16;

//set parameters
int period = 15; //medium period in minutes
int freq_sampling = 100; // sampling time
int ciclos = 20; // sampling time

int dutyCycle1 = 0;
int dutyCycle2 = 0;


// Setting PWM properties
const int freq = 30000;
const int pwmChannell = 0;
const int pwmChannel2 = 1;
const int resolution = 8;


#include <WiFi.h>
#include <HTTPClient.h>


// WiFi
const char *ssid = "COURAGEOUS"; // Enter your WiFi name
const char *password = "Iz12hcjb21Bz";  // Enter WiFi password

//Your Domain name with URL path or IP address with path
const char* serverName = "http://192.168.100.57:1880/update-sensor"; //repleace  192.168.0.101 for your addres


String temperatura1 = "";
String temperatura2 = "";
float temp1;
float temp2;


void setup() {
  delay(1000);
  Serial.begin(115200);
  
  sensors1.begin();   //Sensor 1 starts
  sensors2.begin();   //Sensor 2 starts

  //transistor 1
  pinMode(trans1, OUTPUT);
  //transitor 2
  pinMode(trans2, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(pwmChannell, freq, resolution);
  ledcSetup(pwmChannel2, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(trans1, pwmChannell);
  ledcAttachPin(trans2, pwmChannel2);
  Serial.println("Choose any case: ");

  //wifi
  WiFi.mode(WIFI_STA);
}

void loop() {
  connect_wifi();  
  readData();
  publicData();
  delay(1000);
  
  if (Serial.available())
  {
    String string = Serial.readStringUntil('\n');

    if (string == "case_1") {
      Serial.println("Case 1 started");
      for (int i = 1; i <= ciclos; i++) {
        //transistor 1 deactivate
        dutyCycle1 = 0;
        ledcWrite(pwmChannell, dutyCycle1);
        //transistor 2 deactivate
        dutyCycle2 = 0;
        ledcWrite(pwmChannel2, dutyCycle2);
        readData();
        readData();
      }
      Serial.println("Case 1 finished");
      Serial.println("Choose any case: ");

    }
    else if (string == "case_2") {
      Serial.println("Case 2 started");

      for (int i = 1; i <= ciclos; i++) {
        //transistor 1 activate
        dutyCycle1 = 255;
        ledcWrite(pwmChannell, dutyCycle1);
        readData();
        //transistor 1 deactivate
        dutyCycle1 = 0;
        ledcWrite(pwmChannel2, dutyCycle2);
        readData();
      }
      Serial.println("Case 2 finished");
      Serial.println("Choose any case: ");
    }
    else if (string == "case_3") {
      Serial.println("Case 3 started");

      for (int i = 1; i <= ciclos; i++) {
        //transistor 2 activate
        dutyCycle2 = 255;
        ledcWrite(pwmChannel2, dutyCycle2);
        readData();
        //transistor 2 deactivate
        dutyCycle2 = 0;
        ledcWrite(pwmChannel2, 0);
        readData();
      }
      Serial.println("Case 3 finished");
      Serial.println("Choose any case: ");
    }
    else if (string == "case_4") {
      Serial.println("Case 4 started");

      for (int i = 1; i <= ciclos; i++) {
        //transistor 1 activate
        dutyCycle1 = 255;
        ledcWrite(pwmChannell, dutyCycle1);
        //transistor 2 activate
        dutyCycle2 = 255;
        ledcWrite(pwmChannel2, dutyCycle2);
        readData();
        //transistor 1 deactivate
        dutyCycle1 = 0;
        ledcWrite(pwmChannell, dutyCycle1);
        //transistor 2 deactivate
        dutyCycle2 = 0;
        ledcWrite(pwmChannel2, dutyCycle2);
        readData();
      }
      Serial.println("Case 4 finished");
      Serial.println("Choose any case: ");
    }

  }

}


//method to read data for 15 minute
void readData() {

  //The command is sent to read the temperature
  sensors1.requestTemperatures();
  //Obtain the temperature in ºC of sensor 1
  temp1 = sensors1.getTempCByIndex(0);

  temperatura1 = String(temp1);

  //The command is sent to read the temperature
  sensors2.requestTemperatures();
  //Obtain the temperature in ºC of sensor 2
  temp2 = sensors2.getTempCByIndex(0);
  temperatura2 = String(temp2);
  //print to display the temperature change

}

void connect_wifi(){
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
}

void publicData(){
  WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=DS18B20&value1="+temperatura1+"&value2="+temperatura2;           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      
      
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
}
