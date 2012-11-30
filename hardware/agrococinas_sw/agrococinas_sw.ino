#include <EEPROM.h>
//declare variables
float tempInC;
float humedC;
float luminC;
float tempOutC;

int tempInPin = A0;
int humedPin = A1;
int luminPin = A2;
int tempOutPin = A3;
int samples[8]; // 8 samples to have a large sample

int addr = 0;
int addr1 = 0;
int addr2 = 0;
int addr3 = 0;

float supplyVolt = 5.0;
char* msg = "";


void setup()
{
  Serial.begin(9600); //opens serial port, sets data rate to 9600 bps
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);
}
void loop()
{
  tempInC = analogRead(tempInPin);           //read the value from the sensor
  tempInC = (supplyVolt * tempInC * 100.0)/1023.0;  //temp in
  tempInC = (tempInC-32)*5/9;
  delay(20);
  tempOutC = analogRead(tempOutPin);           //read the value from the sensor
  tempOutC = (supplyVolt * tempOutC * 100.0)/1023.0;  //convert the analog data to temperature
  delay(20);
  humedC = analogRead(humedPin);           //read the value from the sensor
  float voltage = humedC/1023. * supplyVolt; // convert to voltage value
  float sensorRH = 161.0 * voltage / supplyVolt - 25.8;
  //humedC = ((((humedC/1023)*5)-0.8)/3.1)*100;  //convert the analog data to HR%
  humedC = (sensorRH)/(1.0546-0.00216*(tempOutC)); // temperature compensation
  delay(20);
  luminC = analogRead(luminPin);           //read the value from the sensor
  //luminC = (5.0 * luminC * 100.0)/1024.0;  //convert the analog data 
  Serial.println("=====================");
  Serial.print("Temperatura Externa: ");
  Serial.print(tempOutC,0); 
  Serial.println();
  Serial.print("Luminosidad: ");
  Serial.print(luminC, 0); 
  Serial.println();
  Serial.print("Humedad: ");
  Serial.print(humedC,0);
  Serial.println();
  Serial.print("Temperatura Interna: ");
  Serial.print(tempInC,0);  
  Serial.println();
  
  msg = "INSERT@"+43.27123+"@"+-2.93855 +"@"+ProtoCocina+"@"+timestamp
  Serial.println(msg);
  /*
Serial.print((byte)humed1C ); 
   Serial.print(",");
   Serial.print((byte)humed2C ); 
   Serial.print(",");
   Serial.print((byte)luminC ); 
   Serial.print(",");
   Serial.print((byte)tempC ); 
   Serial.println();
   */
 // delay(59860);                           //wait 60 second before sending new data
  delay(1360);                           //wait 60 second before sending new data
  addr1 = addr + 1;
  addr2 = addr + 2;
  addr3 = addr + 3;
  EEPROM.write(addr,(byte)tempOutC);
  delay(20);
  EEPROM.write(addr1, (byte)luminC);
  delay(20);
  EEPROM.write(addr2, (byte)humedC);
  delay(20);
  EEPROM.write(addr3, (byte)tempInC);
  delay(20);

  // advance to the next address.  there are 512 bytes in 
  // the EEPROM, so go back to 0 when we hit 512.
  addr = addr + 4;
  if (addr == 512)
    addr = 0;
}










