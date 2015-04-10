
/* created on 08/04/2014
   by Debasish Dutta/deba168
   
   This code is in the public domain.
   If you modify please inform me
   send a modified copy which will be helpfull for me
 */

#include <LiquidCrystal.h>
float solar_volt =0; // variable for solar panel voltage
float bat_volt=0;    // variable for battery voltage
float sample1=0;     // reading form Arduino pin A0
float sample2=0;     // reading form Arduino pin A1
int pwm=6;           // pwm out put to mosfet
int load=9;          //load is connected to pin-9
int charged_percent =0; 
LiquidCrystal lcd(12,11, 10, 5, 4, 3, 2);
int backLight = 13;  // pin 13 will control the backlight
int RED=7;           // To indicate discharged condition of battery
int GREEN=8;         // for charging and battery fully charged

void setup()
{
  TCCR0B = TCCR0B & 0b11111000 | 0x05; // setting prescaar for 61.03Hz pwm
  Serial.begin(9600);
  pinMode(pwm,OUTPUT);
  pinMode(load,OUTPUT);
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  digitalWrite(pwm,LOW);
  digitalWrite(load,LOW);
  digitalWrite(RED,LOW);
  digitalWrite(RED,LOW);
  pinMode(backLight, OUTPUT);        	//set pin 13 as output
  analogWrite(backLight, 150);       	//controls the backlight intensity 0-255
  lcd.begin(16,2);                     // columns, rows. size of display
  lcd.clear();                         // clear the screen
}
void loop()
{
  lcd.setCursor(16,1); // set the cursor outside the display count
  lcd.print(" ");      // print empty character
  
  ///////////////////////////  VOLTAGE SENSING ////////////////////////////////////////////
  for(int i=0;i<150;i++)
  {
    sample1+=analogRead(A0);  //read the input voltage from solar panel
    sample2+=analogRead(A1);  //read the battery voltage 
    delay(2);
  }
   sample1=sample1/150; 
   sample2=sample2/150; 
   // actual volt/divider output=3.127
   //2.43 is eqv to 520 ADC
   // 1 is eqv to .004673
   solar_volt=(sample1*4.673* 3.127)/1000;
   bat_volt=(sample2*4.673* 3.127)/1000;
   Serial.print("solar input voltage :");
   Serial.println(solar_volt);
   Serial.print("battery voltage :");
   Serial.println(bat_volt);
   
// ///////////////////////////PWM BASED CHARGING ////////////////////////////////////////////////
// As battery is gradually charged the charge rate (pwm duty) is decreased
// 7.2v = fully charged(100%) 
//6v =fully discharged(0%)
// when battery voltage is less than 6.2v, give you alart by glowing RED LED and displaying "DISCHARGED..."

if((solar_volt > bat_volt)&& ( bat_volt <= 6.96 ))
{
analogWrite(pwm,242.25); // @ 95% duty // boost charging// most of the charging done here
Serial.print("pwm duty cycle is :");
Serial.println("95%");
}
else if((solar_volt > bat_volt)&&(bat_volt > 6.96)&& (bat_volt <= 7.2 ))
{
analogWrite(pwm,25.5);  // 10% duty // float charging 
Serial.print("pwm duty cycle is :");
Serial.println("10%");
}
// // shut down when battery is fully charged or when sunlight is not enough
else if ((bat_volt > 7.2) or (solar_volt < bat_volt))
{
analogWrite(pwm,0); 
Serial.print("pwm duty cycle is :");
Serial.println("0%");
digitalWrite(GREEN,LOW); // green LED will off as no charging is done during this time
}
///////////////////////////////////////// BATTERY STATUS INDICATOR  ////////////////////////////////////////////////
//The map() function uses integer math so will not generate fractions
// so I multiply battery voltage with 10 to convert float into a intiger value
// when battery voltage is 6.0volt it is totally discharged ( 6*10 =60)
// when battery voltage is 7.2volt it is fully charged (7.2*10=72)
// 6.0v =0% and 7.2v =100%
charged_percent=bat_volt*10;   
charged_percent=map(bat_volt*10, 60 , 72, 0, 100);
/*
 if (solar_volt > bat_volt)&&( bat_volt <=7.2))
 {
 Serial.print (charged_percent);
 Serial.println("% charged");
 Serial.println("");
 Serial.println("**********************************************************************************");
 }
 
 else if (bat_volt < 6)
{
 Serial.println("BATTERY IS DEAD !!!!! ");
}
*/
////////////////////////////////////LCD DISPLAY /////////////////////////////////////////////////////
  lcd.setCursor(0,0); // set the cursor at 1st col and 1st row
  lcd.print("SOL:");
  lcd.print(solar_volt);
  lcd.print(" BAT:");
  lcd.print(bat_volt);
  lcd.setCursor(1,1); // set the cursor at 1st col and 2nd row
  // LCD will show the %charged during charging period only
 if ((bat_volt > 6) && (bat_volt <=7.2))
 {
  lcd.print(charged_percent);
  lcd.print("% Charged ");
 }
 // LCD will alart when battery is dead by displaying the message "BATTERY IS DEAD!!"
 else if (bat_volt < 6)
{
  lcd.print("BATTERY IS DEAD!!");
}
/////////////////////////////LOAD ////LED INDICATION ///////////////////////////////////////////////////////
if ((solar_volt < 3 ) && (bat_volt > 6.2))
// when there is no sunlight(night) and battery is charged,
//load will switched  on automatically
{
  digitalWrite(load,HIGH); 
}
///load will be disconnected during day time(solar_volt > 6) or when battery is discharged condition
if ((bat_volt < 6.2 )or (solar_volt > 6 )) 
{
  digitalWrite(load,LOW); // prevent battery from complete discharging
 
}
//////////////////////////////LED INDICATION DURING CHARGING////////////////////////////////////////

if (  solar_volt > bat_volt && bat_volt <7.2)
{
///Green LED will blink continiously indicating charging is going on
 digitalWrite(GREEN,HIGH);
 delay(5);
 digitalWrite(GREEN,LOW);
 delay(5);
}
// Red LED will glow when battery is discharged
// also display in LCD 
if (bat_volt < 6.2 && bat_volt > 6)
// seecond restriction is given for indicating battery is dead
// if you omit the (bat_volt > 6) when ever battery is dead also display bat discharged
{
  digitalWrite(RED,HIGH); // indicating battery is discharged
  lcd.setCursor(1,1);
  lcd.print("BAT DISCHARGED..");
 }
 // Red LED will OFF when battery is not discharged
if (bat_volt > 6.2)
{
  digitalWrite(RED,LOW);
}
//Green LED will glow when battery is fully charged
if(bat_volt >=7.2) 
{
digitalWrite(GREEN,HIGH);
}

}
