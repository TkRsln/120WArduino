
#define read_current A1 // define the Arduino pin A0 as voltage input (V in)

float cutOffLimit = 0.5;// set the current which below that value, doesn't matter. Or set 0.5

#include <Wire.h> 

///////////////////////////////////////[~MAIN~]
int loopDelay=50;

short pin_button_v2=7;    // Menu Geçişi
short pin_button=8;       
short pin_heat=A0;
short pin_potans=A3;
short pin_sensor_volt=A0;
short pin_current=A1;
short pin_fan=9;
short pin_led=6;
short pin_out_volt=5;//A0;
short pin_meter=4;

short menu_longpres_time=5;
short menu_fanpower_time=5;
short menu_ledpower_time=5;

short blink_time=1;

short led_count_time=3;
short potans_tolerance=10;
const float VCC   = 5.0;// supply voltage is from 4.5 to 5.5V. Normally 5V.
float volt_value=0;

short potans_value=0;

///////////////////////////////////////////////
//////////////////////////////////////////[LCD]
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); //  A5 SCL | A4 SDA
int lcd_width=16;
int lcd_height=2;
int lcdFindPointForMid(String str){
  return (lcd_width-str.length())/2;
}
///////////////////////////////////////////////
////////////////////////////////////////[BLINK]
short blink_count=0;
bool canBlink=false;

void onBlink(){
  if(canBlink){
    if(blink_count==blink_time){
      canBlink=false;
      lcd.backlight();
      blink_count=0;
      return;
    }
    else if(blink_count==0)lcd.noBacklight();
    blink_count++;
    
  }
}
void startBlink(){
  canBlink=true;
}


///////////////////////////////////////////////
/////////////////////////////////[AKIM BÖLGESİ]
/*
          "ACS712ELCTR-05B-T",// for model use 0
          "ACS712ELCTR-20A-T",// for model use 1
          "ACS712ELCTR-30A-T"// for model use 2  
sensitivity array is holding the sensitivy of the  ACS712
current sensors. Do not change. All values are from page 5  of data sheet  
*/

float sensitivity = 0.066;// for ACS712ELCTR-30A-T
const float QOV =   0.5 * VCC;// set quiescent Output voltage of 0.5V
float voltage;// internal variable for voltage

float findMotorCurrent(){
  //Robojax.com ACS712 Current Sensor 
  float voltage_raw =   (5.0 / 1023.0)* analogRead(read_current);// Read the voltage from sensor
  voltage =  voltage_raw - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current = voltage / sensitivity;
 
  if(abs(current) > cutOffLimit ){
    return current;
    //lcd.setCursor(0,0);
    //String txt="5V - ";//"V: ";
    //txt+=String(voltage,3);// print voltage with 3 decimal places
    //txt+=("V, I: ");
    //txt+=String(current,2); // print the current with 2 decimal places
    //txt+=("A");
    //lcd.print(txt);
  }else 
  return 0.0f;
}
///////////////////////////////////////////////
//////////////////////////////////////[ON BOOT]
String boot_msg="HOSGELDIN";
void printBoot(){
  lcd.begin();
    lcd.backlight();
    int point=lcdFindPointForMid(boot_msg);
    for(int j=15;j>=0;j--){
      lcd.setCursor(0,0);
      int i=0;
      for(i=0;i<j;i++){
        lcd.print((char)255);
      }
      for(int k=i;k<16;k++){
        lcd.print(k<point?' ':(k-point>=boot_msg.length()?' ':boot_msg[k-point]));
      }
      delay(75);
    }
    delay(500);
}
///////////////////////////////////////////////
///////////////////////////////////////[~MENU~]

short MENU_MAIN =0;
short MENU_VOLTAGE  =1;
short MENU_FAN  =2;
short MENU_LED  =3;
//short MENU_USB  =4;


short menu_current=2;

short menu_longpres_count=0;
short menu_button_count=0;
bool menu_buttonv2=false;

short menu_fanpower=10;

short menu_ledpower=10;
bool menu_ledRefresh=true;


void onMainMenu(){
  short button=menuButton();
  if(button!=0)Serial.println(button);
  //Serial.println(button);
  if(button==4){
    menu_current=(menu_current+1)%4;
    menu_longpres_count=0;
    lcd.clear();
  }
  printMenu(button);
  //printHeat();
  //fanPower();
  //*/
}
void showMenu(int menu){
  lcd.clear();
  menu_current=menu;
}

void printMenu(short button){
  if(menu_current== MENU_MAIN)
    menuMotor(button);
  else if(menu_current == MENU_FAN)
    menuFan(button);
  else if(menu_current == MENU_LED)
    menuLed(button);
  else if(menu_current == MENU_VOLTAGE)
    menuVoltage(button);
  
}
void menuVoltage(short button){
  
  double pw= 10-(potans_value/1024.0*10);
  if( potans_value>=1020)pw=0;
  lcd.setCursor(0,0);
  lcd.print("Port Power:");
  lcd.setCursor(0,1);
  lcd.print("[");
     int i=0;
     for(i=0;i<pw;i++)
        lcd.print((char)255);
     for(;i<10;i++)
        lcd.print(' ');
     lcd.setCursor(11,1);
     lcd.print("]%"+String((int)(pw*10)) + " ");
}
void menuLed(short button){
  if(button == 3){
      menu_longpres_count++;
      if(menu_longpres_count>menu_ledpower_time){
        menu_ledpower=(menu_ledpower+1)%11;
        menu_longpres_count=0;
        menu_ledRefresh=true;
      }
    }else if(button==1){
      menu_ledpower=(menu_ledpower+1)%11;
      menu_ledRefresh=true;
    }
     lcd.setCursor(0,0);
     lcd.print("Led Power: ");
     lcd.setCursor(0,1);
       lcd.print("[");
       int i=0;
       for(i=0;i<menu_ledpower;i++)
          lcd.print((char)255);
       for(;i<10;i++)
          lcd.print(' ');
       lcd.setCursor(11,1);
       lcd.print("]%"+String(menu_ledpower)+"0 ");
}
void menuMotor(short button){
  if(button == 1){
      menu_longpres_count=(menu_longpres_count+1)%2;
      //Serial.println(menu_longpres_count);
     
    }
      if(menu_longpres_count==0){
        lcd.setCursor(0,0);
        lcd.print("Volt:  "+String(volt_value));
        lcd.setCursor(0,1);
        lcd.print("Amper: "+String(findMotorCurrent()));
      }
      else{
        lcd.setCursor(0,0);
        lcd.print("RPM = "+String("50"));
        lcd.setCursor(0,1);
        lcd.print("Amper: "+String(findMotorCurrent()));
      }
}
void menuFan(short button){
  if(button == 3){
      menu_longpres_count++;
      if(menu_longpres_count>menu_fanpower_time){
        menu_fanpower=(menu_fanpower+1)%12;
        fanPower();
        menu_longpres_count=0;
      }
    }else if(button==1){
      menu_ledpower=(menu_ledpower+1)%12;
      fanPower();
    }
     lcd.setCursor(0,0);
     lcd.print("Fan Power: ");
     lcd.setCursor(0,1);
     if(menu_fanpower<11){
       lcd.print("[");
       int i=0;
       for(i=0;i<menu_fanpower;i++)
          lcd.print((char)255);
       for(;i<10;i++)
          lcd.print(' ');
       lcd.setCursor(11,1);
       lcd.print("]%"+String(menu_fanpower)+"0 ");
     }
     else
      lcd.print(" AUTO ");
}

void printHeat(){
  lcd.setCursor(13,0);
  lcd.print(String(findDegre()) + ((char)223));
}

short menuButton(){
  bool curnt_val2 =(digitalRead(pin_button_v2)==HIGH);
  bool curnt_val =(digitalRead(pin_button)==HIGH);
  //Serial.println("B1:"+String(curnt_val)+" - B2:"+String(curnt_val2));
  if(curnt_val2){
    //Serial.println("v2: "+String(curnt_val2));
    menu_buttonv2=true;
    return 0;
  }else if(menu_buttonv2){
    menu_buttonv2=false;
    return 4;
  }
  
  if(curnt_val){
    //Serial.println("v1: "+String(curnt_val));
    menu_button_count++;
    return menu_button_count>menu_longpres_time?3:0;
  }
  else{
    if(menu_button_count>menu_longpres_time){
        menu_button_count=0;
        return 2;
    }else if(menu_button_count>1){
        menu_button_count=0;
        return 1;
    }else
        return 0;
  }//*/
}
///////////////////////////////////////////////
/////////////////////////////////[VOLTAGE READ]

int volt_offset =20;// set the correction offset value
int volt_tolerance=2;
float readMotorVoltage(){
  int volt = analogRead(pin_sensor_volt);// read the input
  float voltage = map(volt,0,1023, 0, 2500) + volt_offset;// map 0-1023 to 0-2500 and add correction offset
  
  voltage /=100;// divide by 100 to get the decimal values
  /*if(menu_current!=MENU_MAIN){
    if(abs(volt_value-voltage)>volt_tolerance){
      showMenu(MENU_MAIN);
      startBlink();
    }
  }else*/
    volt_value=voltage;
  return voltage;
  
}

///////////////////////////////////////////////
//////////////////////////////////////////[LED]
short led_curentPower=0;
short led_count=0;
//led_count_time
void ledPower(){
  if(!menu_ledRefresh)return;
 
  int val_target = (int)((float)(70.0*menu_ledpower/10));
  
  if(val_target==led_curentPower){
    menu_ledRefresh=false;
    return;
  }
  led_curentPower+=(led_curentPower<val_target)?1:-1;
  analogWrite(pin_led,led_curentPower);
}
///////////////////////////////////////////////
//////////////////////////////////////////[FAN]
void fanPower(){
  int val = (int)((float)(110.0*menu_fanpower/10));
  analogWrite(pin_fan,val);
}
void fanAuto(){
  
}
///////////////////////////////////////////////
///////////////////////////////////////////[ISI]
short* degre=new short[10];
short degre_curent=0;
short degre_value=0;

short findDegre(){
  //float gerilimDeger = (analogRead(A0)/1023.0)*5000;
  // Gerilim değerini sıcaklık değerine dönüştürüyoruz.
  degre[degre_curent] = (short)( ((analogRead(pin_heat)/1023.0)*5000) / 10.0);
  degre_curent=(degre_curent+1)%10;
  int total=0;
  for(int i=0;i<10;i++){
    total+=degre[i];
  }
  degre_value=(short)(total/10);
  if(menu_fanpower==11)
    fanAuto();
  return degre_value;
  //degre[degre_curent]=sicaklikDeger
}
///////////////////////////////////////////////
///////////////////////////////////////[POTANS]

short potans_max=0;

void readPotans(){
  short data = analogRead(pin_potans);
  //Serial.println("pot:"+String(data));
  if(menu_current!=MENU_VOLTAGE){
    if(abs(data-potans_value)>potans_tolerance){
      showMenu(MENU_VOLTAGE);
      startBlink();
    }
  }else
    potans_value=data;
  if(data>=1020){
    //digitalWrite(pin_meter,LOW);
    data=1024;
    //analogWrite(pin_out_volt,0);
    digitalWrite(pin_out_volt,LOW);
    Serial.println("Volt is 0");
  }
  else{
    //digitalWrite(pin_meter,HIGH);
    analogWrite(pin_out_volt,255-(int)(data*255.0/1024.0));
  }
  
}

///////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
    //TCCR2B = TCCR2B & B11111000 | B00000001; 
    TCCR2B = TCCR2B & B11111000 | B00000111;  // 9,10 for PWM frequency of 30.64 Hz
    //TCCR0B = TCCR0B & B11111000 | B00000101; // 5,6 - for PWM frequency of 61.04 Hz
    printBoot();

    pinMode(pin_fan,OUTPUT);
    pinMode(pin_meter,OUTPUT);
    pinMode(pin_out_volt,OUTPUT);
    digitalWrite(pin_meter,HIGH);
    

    pinMode(8,INPUT);
    
    lcd.clear();
    readMotorVoltage();
    readPotans();
    fanPower();
}

void loop() {

      //bool readValue=digitalRead(8);
      onMainMenu();
      ledPower();
      readPotans();
      readMotorVoltage();
      //onBlink();
      delay(loopDelay);
} 

