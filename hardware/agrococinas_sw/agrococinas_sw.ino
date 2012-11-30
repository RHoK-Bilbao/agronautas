#include <EEPROM.h>
#include <SdFat.h>
SdFat sd;
SdFile myFile;
//declare variables
const int chipSelect = 10;
float tempInC;
float humedC;
float luminC;
float tempOutC;
char* luminL;

int tempInPin = A0;
int humedPin = A1;
int luminPin = A2;
int tempOutPin = A3;
int led = 7;
int samples[8]; // 8 samples to have a large sample

int addr = 0;
int addr1 = 0;
int addr2 = 0;
int addr3 = 0;

float supplyVolt = 5.0;
int temp_warning = 22;
char* msg = "";


void setup()
{
  Serial.begin(9600); //opens serial port, sets data rate to 9600 bps
   pinMode(led, OUTPUT);   
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);
    
}

void writeSD(){
if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  // open the file for write at end like the Native SD library
  if (!myFile.open("log.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }


  myFile.print("Temperatura Externa: ");
  myFile.print(tempOutC,0); 
  myFile.println();
  myFile.print("Luminosidad: ");
  myFile.print(luminC, 0); 
  myFile.print("  ");
  myFile.print(luminL);
  myFile.println();
  myFile.print("Humedad: ");
  myFile.print(humedC,0);
  myFile.println();
  myFile.print("Temperatura Interna: ");
  myFile.print(tempInC,0);  
  myFile.println();

  // close the file:
  myFile.close();
  Serial.println("done.");

}

void readSD(){
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  // re-open the file for reading:
  if (!myFile.open("log.txt", O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }
  Serial.println("log.txt:");

  // read from the file until there's nothing else in it:
  int data;
  while ((data = myFile.read()) > 0) Serial.write(data);
  // close the file:
  myFile.close();
}

void evaluate_temp()
{
  Serial.println(tempOutC);
  Serial.println(temp_warning);
  if(tempOutC> temp_warning){
    Serial.print("Temperatura superior a ");
    Serial.println(temp_warning);
    digitalWrite(led, HIGH); 
  }else{
     digitalWrite(led, LOW); 
  }
}

void evaluate_lumi()
{
  if(luminC < 75){
    luminL = "Muy baja";
  }else if(luminC < 150){
    luminL = "baja";
  }else if(luminC < 225){
    luminL = "luminoso";
  }else{
    luminL = "muy luminoso";
  }
  
}
void loop()
{
  tempInC = analogRead(tempInPin);           //read the value from the sensor
  tempInC = (supplyVolt * tempInC * 100.0)/1023.0;  //temp in
  tempInC = (tempInC-32)*5/9;
  delay(20);
  tempOutC = analogRead(tempOutPin);           //read the value from the sensor
  tempOutC = (supplyVolt * tempOutC * 100.0)/1023.0;  //convert the analog data to temperature
  evaluate_temp();
  delay(20);
  humedC = analogRead(humedPin);           //read the value from the sensor
  float voltage = humedC/1023. * supplyVolt; // convert to voltage value
  float sensorRH = 161.0 * voltage / supplyVolt - 25.8;
  //humedC = ((((humedC/1023)*5)-0.8)/3.1)*100;  //convert the analog data to HR%
  humedC = (sensorRH)/(1.0546-0.00216*(tempOutC)); // temperature compensation
  delay(20);

  luminC = analogRead(luminPin);           //read the value from the sensor
  evaluate_lumi();
  //luminC = (5.0 * luminC * 100.0)/1024.0;  //convert the analog data 
  Serial.println("=====================");
  Serial.print("Temperatura Externa: ");
  Serial.print(tempOutC,0); 
  Serial.println();
  Serial.print("Luminosidad: ");
  Serial.print(luminC, 0); 
  Serial.print("  ");
  Serial.print(luminL);
  Serial.println();
  Serial.print("Humedad: ");
  Serial.print(humedC,0);
  Serial.println();
  Serial.print("Temperatura Interna: ");
  Serial.print(tempInC,0);  
  Serial.println();
  writeSD();
  msg = "INSERT@43.27123@-2.93855@ProtoCocina@";
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










