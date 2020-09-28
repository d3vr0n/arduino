
// Bluetooth Tutorial By Robo India
// Bluetooth module used - HC-06

#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN A5     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

SoftwareSerial Bluetooth(2, 3); // (TXD, RXD) of HC-06

boolean debug = true;
String BT_input; // to store input character received via BT.
// max length of command is 20 chrs
const byte numChars = 20;
char receivedChars[numChars];
boolean newData = false;
byte LEDpin = 13;
byte level10 = 4;
byte level20 = 5;
byte level40 = 6;
byte level60 = 7;
byte level80 = 8;
byte level100 = 9;// A6

float hum;  //Stores humidity value
float temp; //Stores temperature value

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDpin, OUTPUT);     // Arduino Board LED Pin
  pinMode(level10, INPUT);
  pinMode(level20, INPUT);
  pinMode(level40, INPUT);
  pinMode(level60, INPUT);
  pinMode(level80, INPUT);
  pinMode(level100, INPUT);

  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
  
  // HC-06 default serial speed is 9600
  Bluetooth.begin(9600); 
  dht.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Bluetooth.available() > 0)     {  recvWithStartEndMarkers(); }
  if (newData) { parseData(); }

// Keep reading from Arduino Serial Monitor and send to HC-06
  if (Serial.available())
  {  
      char bleData = Serial.read();
      Bluetooth.write(bleData);
  }

}

void parseData()
{  
        newData = false;
        byte idx = 0;
        BT_input = "";
        while(receivedChars[idx] != '\0') {
          BT_input += receivedChars[idx];
          idx++;
          if(idx > 20){
            break;
          }
        }
        if (debug) {  Serial.println( receivedChars ); Serial.println( BT_input ); }
        if (receivedChars[0] == 'O'  && receivedChars[1] == 'N' )  { digitalWrite(LEDpin,HIGH);  }
        if (receivedChars[0] == 'O'  && receivedChars[1] == 'F' )  { digitalWrite(LEDpin,LOW);   }
        if( BT_input == "L") { 
          String level10_val = "level10 > " + String(digitalRead(level10)) + "\r\n";
          Serial.print(level10_val);
          String level20_val = "level20 > " + String(digitalRead(level20)) + "\r\n";
          Serial.print(level20_val);
          String level40_val = "level40 > " + String(digitalRead(level40)) + "\r\n";
          Serial.print(level40_val);
          String level60_val = "level60 > " + String(digitalRead(level60)) + "\r\n";
          Serial.print(level60_val);
          String level80_val = "level80 > " + String(digitalRead(level80)) + "\r\n";
          Serial.print(level80_val);
           // without NL CR it is not printing over bluetooth
          String level100_val = "level100 > " + String(digitalRead(level100)) + "\r\n";
          Serial.print(level100_val);

          //Read data and store it to variables hum and temp
          hum = dht.readHumidity();
          String hum_val = "Humidity > " + String(hum) + "%\r\n";
          Serial.print(hum_val);
          temp= dht.readTemperature();
          String temp_val = "Temp > " + String(temp) + "\r\n";
          Serial.print(temp_val);

          String payload = level10_val + level20_val + level40_val + level60_val + level80_val + level100_val + hum_val + temp_val;
          // delay(1000);
                  
          // Length (with one extra character for the null terminator)
          int str_len = payload.length() + 1;   
          // Prepare the character array (the buffer)
          char char_array[str_len];   
          // Copy it over 
          payload.toCharArray(char_array, str_len);
  
          Serial.println(char_array); Bluetooth.write(char_array); 
        }       
}

void recvWithStartEndMarkers() 
{
 
     // function recvWithStartEndMarkers by Robin2 of the Arduino forums
     // See  http://forum.arduino.cc/index.php?topic=288234.0
 
     static boolean recvInProgress = false;
     static byte ndx = 0;
     char startMarker = '<';
     char endMarker = '>';
     char rc;
 
     if (Bluetooth.available() > 0) 
     {
          rc = Bluetooth.read();
          if (recvInProgress == true) 
          {
               if (rc != endMarker) 
               {
                    receivedChars[ndx] = rc;
                    ndx++;
                    if (ndx >= numChars) { ndx = numChars - 1; }
               }
               else 
               {
                     receivedChars[ndx] = '\0'; // terminate the string
                     recvInProgress = false;
                     ndx = 0;
                     newData = true;
               }
          }
 
          else if (rc == startMarker) { recvInProgress = true; }
          else {
            Serial.println(rc);
          }
     }
}
