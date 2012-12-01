// Includes
#include <EEPROM.h>
#include <SdFat.h>
#include <LiquidCrystal.h>

// declare variables
SdFat sd;
SdFile myFile;
const int chipSelect = 10;
float tempInC;
float humedC;
float luminC;
float tempOutC;
char* luminL;
boolean advicedHighTemp = false;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(9, 8, 5, 4, 3, 2);
//Botones
const int bDataPin = 0;
const int bStartPin = 6;
const int bFinishPin = 1;
int bDataState = 1;
int bStartState = 1;
int bFinishState = 1;
long int timestampBegin = 0;
long int timestampIntermediate = 0;
long int timestampFinish = 0;
int stage = 0; //0:sin orientar; 1:orientado y calentandose; 2:cocinando; 
//Sensors
int tempInPin = A0;
int humedPin = A1;
int luminPin = A2;
int tempOutPin = A3;
int led = 7;
int samples[8]; // 8 samples to have a large sample
//EPROMM Write
int addr = 0;
int addr1 = 0;
int addr2 = 0;
int addr3 = 0;
// Constants
float supplyVolt = 5.0;
int temp_warning = 22;
char* msg = "";
char* nombre_cocina = "SuperCocina";
char StringFinal[40];


void setup()
{
  Serial.begin(115200); //opens serial port, sets data rate to 115200 bps
   pinMode(led, OUTPUT);   
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);
  pinMode(bStartPin, INPUT); 
  pinMode(bFinishPin, INPUT);
  pinMode(bDataPin, INPUT);  
  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("Orientame al sol y");
  lcd.setCursor(0, 2);
  lcd.print("despues pulsa verde.");   
}

void writeSD(){
if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  // open the file for write at end like the Native SD library
  if (!myFile.open("log.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }
  myFile.println();
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

void generateUpdate()
{
  char arroba[2]="@";
  char aux[10];
  memcpy(StringFinal,"UPDATE@",7); 
  strcat(StringFinal, nombre_cocina);
  strcat(StringFinal, arroba);
  sprintf(aux,"%d",int(luminC));
  strcat(StringFinal, aux);
  strcat(StringFinal, arroba);
  sprintf(aux,"%d",int(tempOutC));
  strcat(StringFinal, aux);
  strcat(StringFinal, arroba);
  sprintf(aux,"%d",int(tempInC));
  strcat(StringFinal, aux);
  strcat(StringFinal, arroba);
  sprintf(aux,"%d",int(humedC));
  strcat(StringFinal, aux);
  delay(10);
  Serial.println(StringFinal);
  //return StringFinal;
}

void showData(float iT, float eT, float h, float l){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp. Int:");
  lcd.setCursor(13, 0);
  lcd.print(iT,1);
  lcd.setCursor(19, 0);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Temp. Ext:");
  lcd.setCursor(13, 1);
  lcd.print(eT,1);
  lcd.setCursor(19, 1);
  lcd.print("C");
  lcd.setCursor(0, 2);
  lcd.print("Humedad:");
  lcd.setCursor(13, 2);
  lcd.print(h,1);
  lcd.setCursor(19, 2);
  lcd.print(char(37));
  lcd.setCursor(0, 3);
  lcd.print("Luminosidad:");
  lcd.setCursor(13, 3);
  lcd.print(l,0);
  lcd.setCursor(18, 3);
  lcd.print("LX");
}

void readSensors()
{
  ////////////////////Leemos los sensores////////////////////////////////////////
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
}

void readButtons()
{
  bStartState = digitalRead(bStartPin);
  bFinishState = digitalRead(bFinishPin);  
  bDataState = digitalRead(bDataPin);
}

void evaluateTemperature()
{
  if ((tempInC>60) && (!advicedHighTemp)){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Estoy a 60 grados.");
  lcd.setCursor(0,1);
  lcd.print("Cuando introduzcas");
  lcd.setCursor(0,2);
  lcd.print("la comida, pulsa el");
  lcd.setCursor(0,3);
  lcd.print("verde.");
  advicedHighTemp=true;
  }
}

void checkShowData()
{
   // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (bDataState == LOW) {
    delay(300);    
    showData(tempInC, tempOutC, humedC, luminC);
    delay(5000);
    lcd.clear();
  } 
}

void checkStageTwo()
{
    if ((bFinishState == LOW)&&(stage == 2)){
    delay(300);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Comida lista");
    lcd.setCursor(0,2);
    lcd.print("Que aproveche");
    delay(3000);
    lcd.clear();
    stage = 0;
  }  
}

void checkStageOne()
{
    if (bStartState == LOW){
    Serial.println("pulsado verde");
    if (stage == 0){
      delay(300);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("La cocina se esta");
      lcd.setCursor(0,2);
      lcd.print("calentando");
      delay(3000);
      lcd.clear();
      stage = 1;
    }
    else if (stage == 1){
      delay(300);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Cocinando");
      delay(3000);
      lcd.clear();
      stage = 2;
    }
  }
}
void loop()
{
  readSensors();
  delay(3000);
  generateUpdate();
  writeSD();
  memset(StringFinal,0,sizeof(StringFinal));
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

  readButtons(); 
  checkStageOne();
  checkStageTwo(); 
  checkShowData();
  evaluateTemperature();
  
}










