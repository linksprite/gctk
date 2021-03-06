#include <SoftwareSerial.h>
#include <string.h> 
#include <TinyGPS.h>
SoftwareSerial Sim900Serial(7, 8);
byte buffer[64];                             // buffer array for data recieve over serial port
int count=0;                                 // counter for buffer array
SoftwareSerial GPS(2, 3);

TinyGPS gps;
unsigned long fix_age;
long lat, lon;
float LAT, LON;
void gpsdump(TinyGPS &gps);
bool feedgps();
void getGPS();
int PowerKey = 9;

void setup()
{
  pinMode(PowerKey,OUTPUT);
  digitalWrite(PowerKey,HIGH);
  delay(3000);
  digitalWrite(PowerKey,LOW);
  Sim900Serial.begin(19200);     // the SIM900 baud rate
  GPS.begin(9600);                   // GPS module baud rate  
  Serial.begin(9600);                  // the Serial port of Arduino baud rate.
  delay(500);
  Sim900_Inti();
}
 
void loop()
{
  Sim900Serial.listen();
  if (Sim900Serial.available())                     // If date is comming from from GSM shield)
  {
    Serial.println("receive message");
    while(Sim900Serial.available())              // reading data into char array 
    {
      buffer[count++]=Sim900Serial.read();  // writing data into array
      if(count == 64)break;
     }
    Serial.write(buffer,count);                     // if no data transmission ends, write buffer to hardware serial port
    Cmd_Read_Act();                               //Read the 'COMMAND' sent to SIM900 through SMS
    clearBufferArray();                               // call clearBufferArray function to clear the storaged data from the array
    count = 0;                                          // set counter of while loop to zero
  }
  
  if (Serial.available())                       // if data is available on hardwareserial port ==> data is comming from PC or notebook
    Sim900Serial.println(Serial.read());       // write it to the GPRS shield
}
void clearBufferArray()                             // function to clear buffer array
{
  for (int i=0; i<count;i++)
    { buffer[i]=NULL;}                                // clear all index of array with command NULL
}
void Sim900_Inti(void)
{
  Sim900Serial.println("AT+CMGF=1");    // Set GSM shield to sms mode
  Serial.println("AT+CMGF=1");
  delay(500);
  Sim900Serial.println("AT+CNMI=2,2");
  Serial.println("AT+CNMI=2,2");
  delay(500);
}
void Cmd_Read_Act(void)                       //Function reads the SMS sent to SIM900 shield.
{ 
  char buffer2[64];
  for (int i=0; i<count;i++)
  { buffer2[i]=char(buffer[i]);}  
    
  if (strstr(buffer2,"linksprite"))    //Comparing password entered with password stored in program  
  {
      Serial.println("Password Authenticated.");
      Serial.println("Sending reply SMS. ");
      SendTextMessage();            
  }
  
}
void SendTextMessage()
{
  
Sim900Serial.print("AT+CMGF=1\r");    //Sending the SMS in text mode
delay(100);
Sim900Serial.println("AT+CMGS = \"**********\"");//The predefined phone number
delay(100);
Sim900Serial.println("Please wait while Module calculates position");//the content of the message
delay(100);
Sim900Serial.println((char)26);//the ASCII code of the ctrl z is 26
delay(100);
Sim900Serial.println();
int counter=0;
GPS.listen();


for (;;)
{
   long lat, lon;
   unsigned long fix_age, time, date, speed, course;
   unsigned long chars;
   unsigned short sentences, failed_checksum;
   long Latitude, Longitude;
    
   // retrieves /- lat/long in 100000ths of a degree
   gps.get_position(&lat, &lon, &fix_age);
   getGPS();
   Serial.print("Latitude : ");
   Serial.print(LAT/1000000,7);
   Serial.print(" :: Longitude : ");
   Serial.println(LON/1000000,7);
   if (LAT == 0 && LON == 0)
  {
    continue;    
  } 
  counter++;
  if (counter<30)
  {
    continue;    
  }
  
  Sim900Serial.print("AT+CMGF=1\r");    //Sending the SMS in text mode
  delay(100);
  Sim900Serial.println("AT+CMGS = \"**********\"");//The predefined phone number
  delay(100);
  Sim900Serial.print("Latitude : ");
  Sim900Serial.print(LAT/1000000,7);
  Sim900Serial.print(" :: Longitude : ");
  Sim900Serial.println(LON/1000000,7);//the content of the message
  delay(100);
  Sim900Serial.println((char)26);//the ASCII code of the ctrl z is 26
  delay(100);
  Sim900Serial.println();
  counter=0;
  break;
 }      
}

void getGPS()
{
   bool newdata = false;
   unsigned long start = millis();
   while (millis() - start < 1000)
   {
      if (feedgps ())
      {
         newdata = true;
       }
     }
     if (newdata)
   {
      gpsdump(gps);
    }
}
bool feedgps()
{
   while (GPS.available())
 {
      if (gps.encode(GPS.read()))
        return true;
      }return 0;
}
void gpsdump(TinyGPS &gps)
{
   gps.get_position(&lat, &lon);
   LAT = lat;
   LON = lon;
   {
      feedgps(); 
    }
  }

