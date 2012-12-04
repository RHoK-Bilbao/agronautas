// Includes
#include <EEPROM.h>
#include <SdFat.h>
#include <LiquidCrystal.h>

// declare variables
SdFat sd;
SdFile myFile;
const int chipSelect = 53;
float tempInC;
float humedC;
float luminC;
float tempOutC;
char* luminL;
boolean advicedHighTemp = false;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12,11, 5, 4, 3, 2);
//buttons
const int bDataPin = 0;
const int bStartPin = 6;
const int bFinishPin = 1;
int bDataState = 1;
int bStartState = 1;
int bFinishState = 1;
int showState = 0;
//timestamp
long int timestampBegin = 0;
long int timestampIntermediate = 0;
long int timestampFinish = 0;
int stage = 0; //0:not oriented; 1:oriented and heating; 2:cooking; 
//sensors
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
// constants and aux
float supplyVolt = 5.0;
int temp_warning = 22;
char* msg = "";
char* kitchen_name = "kitchen_1";
char aux_string[40];


void setup()
{
  Serial.begin(115200); //opens serial port, sets data rate to 115200 bps
  pinMode(led, OUTPUT);//led pin as output   
  //clean the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);
  pinMode(bStartPin, INPUT); 
  pinMode(bFinishPin, INPUT);
  pinMode(bDataPin, INPUT);  
  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("direct me towards the sun and");
  lcd.setCursor(0, 2);
  lcd.print("press green button");   
}

void writeSD(){
if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  // open the file for write at end like the Native SD library
  if (!myFile.open("log.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("write failed");
  }
  myFile.println();
  myFile.print("Outside temperature: ");
  myFile.print(tempOutC,0); 
  myFile.println();
  myFile.print("Luminosity: ");
  myFile.print(luminC, 0); 
  myFile.print("  ");
  myFile.print(luminL);
  myFile.println();
  myFile.print("Humidity: ");
  myFile.print(humedC,0);
  myFile.println();
  myFile.print("Inside temperatura: ");
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
    sd.errorHalt("read failed");
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
    Serial.print("Temperature over ");
    Serial.println(temp_warning);
    digitalWrite(led, HIGH); 
  }else{
     digitalWrite(led, LOW); 
  }
}

void evaluate_lumi()
{
  if(luminC < 75){
    luminL = "very dark";
  }else if(luminC < 150){
    luminL = "dark";
  }else if(luminC < 225){
    luminL = "bright";
  }else{
    luminL = "very bright";
  } 
}

void generateUpdate()
{
  //aux variables
  char at[2]="@";
  char aux[10];
  //build the string for sending via WiFi
  memcpy(aux_string,"UPDATE@",7); 
  strcat(aux_string, kitchen_name);
  strcat(aux_string, at);
  sprintf(aux,"%d",int(luminC));
  strcat(aux_string, aux);
  strcat(aux_string, at);
  sprintf(aux,"%d",int(tempOutC));
  strcat(aux_string, aux);
  strcat(aux_string, at);
  sprintf(aux,"%d",int(tempInC));
  strcat(aux_string, aux);
  strcat(aux_string, at);
  sprintf(aux,"%d",int(humedC));
  strcat(aux_string, aux);
  delay(10);
  Serial.println(aux_string);
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
  lcd.print("Humidity:");
  lcd.setCursor(13, 2);
  lcd.print(h,1);
  lcd.setCursor(19, 2);
  lcd.print(char(37));
  lcd.setCursor(0, 3);
  lcd.print("Luminosity:");
  lcd.setCursor(13, 3);
  lcd.print(l,0);
  lcd.setCursor(18, 3);
  lcd.print("LX");
}

//read all the sensors
void readSensors()
{
  tempInC = analogRead(tempInPin);           //read the value from the sensor
  tempInC = (supplyVolt * tempInC * 100.0)/1023.0;  //temp in
  tempInC = (tempInC-32)*5/9;
  delay(20);
  tempOutC = analogRead(tempOutPin);           //read the value from the sensor
  tempOutC = (supplyVolt * tempOutC * 100.0)/1023.0;  //change it the analog data to temperature
  evaluate_temp();
  delay(20);
  humedC = analogRead(humedPin);           //read the value from the sensor
  float voltage = humedC/1023. * supplyVolt; // change it to voltage value
  float sensorRH = 161.0 * voltage / supplyVolt - 25.8;
  //humedC = ((((humedC/1023)*5)-0.8)/3.1)*100;  //change it the analog data to HR%
  humedC = (sensorRH)/(1.0546-0.00216*(tempOutC)); // temperature compensation
  delay(20);

  luminC = analogRead(luminPin);           //read the value from the sensor
  evaluate_lumi();
  //luminC = (5.0 * luminC * 100.0)/1024.0;  //change it the analog data 
  Serial.println("=====================");
  Serial.print("Outside temperature: ");
  Serial.print(tempOutC,0); 
  Serial.println();
  Serial.print("Luminosity: ");
  Serial.print(luminC, 0); 
  Serial.print("  ");
  Serial.print(luminL);
  Serial.println();
  Serial.print("Humidity: ");
  Serial.print(humedC,0);
  Serial.println();
  Serial.print("Inside temperature: ");
  Serial.print(tempInC,0);  
  Serial.println();
}

//check the state of the buttons
void readButtons()
{
  bStartState = digitalRead(bStartPin);
  bFinishState = digitalRead(bFinishPin);  
  bDataState = digitalRead(bDataPin);

}

//check if the temperature is good enough to cook
void evaluateTemperature()
{
  if ((tempInC>60) && (!advicedHighTemp)){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("I'm above 60C");
  lcd.setCursor(0,1);
  lcd.print("when the food is inside");
  lcd.setCursor(0,2);
  lcd.print("press the");
  lcd.setCursor(0,3);
  lcd.print("green button");
  advicedHighTemp=true;
  }
}

void checkShowData()
{
   // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if ((bDataState == LOW) || (showState > 0)) {   
    delay(300);    
    showState = 1;
    showData(tempInC, tempOutC, humedC, luminC);
    delay(2000);

  } 
}

//check if the food is cooked
void checkStageTwo()
{
    if ((bFinishState == LOW)&&(stage == 2)){
    delay(300);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Food ready!");
    delay(2000);
    lcd.clear();
    stage = 0;
  }  
}

//check if the kitchen is heating and ready to cook
void checkStageOne()
{
    if (bStartState == LOW){
    Serial.println("green button pressed");
    if (stage == 0){
      delay(300);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Kitchen is");
      lcd.setCursor(0,2);
      lcd.print("heating");
      delay(3000);
      lcd.clear();
      stage = 1;
    }
    else if (stage == 1){
      delay(300);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Cooking");
      delay(3000);
      lcd.clear();
      stage = 2;
    }
  }
}

//main
void loop()
{
  readSensors();
  delay(3000);
  generateUpdate();
  writeSD();
  memset(aux_string,0,sizeof(aux_string));
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
  lcd.clear();
  
}










