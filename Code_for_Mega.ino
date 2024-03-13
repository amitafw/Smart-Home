//общий скетч
//--------БИБЛИОТЕКИ-----------------------------------------------------------------
#include <MQ2.h>  // библиотека для датчика газа
#include <Servo.h> //библиотека для работы с сервоприводом
#include <Wire.h>
//--------БИБЛИОТЕКИ-----------------------------------------------------------------
//--------ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ------------------------------------------------------
int command;
int commandYell;
int commandWhite;
bool FLAME=false;
bool GAS=false;
bool EQ=false;
bool GREEN=false;
bool WHITE=false;
bool YELLOW=false;
bool DOOR=false;
#define PIN_MQ2  A0  // инициализируем пин для подключения датчика газа
#define  flamePin  11 // задаем имя для датчика огня
MQ2 mq2(PIN_MQ2);  // создаём объект для работы с д. газа

//датчик вибрации
int pinA0 = A2;                         // Пин к которому подключен A0
int pinD0 = A1;                         // Пин к которому подключен D0
int motionDetected = 0;                 //переменная состояния
int sensorVal = 0;                      //значения датчика

//датчик огня
int  flame;

//ультразвуковой дальномер
int echoPin = 6; //инициализируем ультразвуковой дальномер
int trigPin = 7; //инициализируем ультразвуковой дальномер

//сервопривод
Servo servo; //объявляем переменную servo типа Servo

//инфракрасные датчики движения
byte sensorPin = 3;//инициализация инфр.д.движения1
byte sensorPin2 = 4;//инициализация инфр.д.движения2
byte indicator = 13;//индикатор(светодиод на плате)
//--------ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ------------------------------------------------------

//функция setup---------------------------------------------------------------------------------------------------------------------------------
void setup() {
 Wire.begin(8);                /* join i2c bus with address 8 */
 Wire.onReceive(receiveEvent); /* register receive event */
 Wire.onRequest(requestEvent); /* register request event */

  Serial.begin(9600);//задаём скорость работы монитора последовательного порта
  mq2.begin();//метод для инициализации работы с датчиком MQ2

  pinMode(sensorPin,INPUT);//устанавливаем инфр.д.движения1 как выход
  pinMode(sensorPin2,INPUT);//устанавливаем инфр.д.движения2 как выход
  pinMode(indicator,OUTPUT);//устанавливаем индикатор(светодиод на плате) как выход
  //ультразвуковой дальномер
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  pinMode(33, OUTPUT);//LED лента желтая
  pinMode(31, OUTPUT);//LED лента белый холодный
  pinMode(12, OUTPUT);//LED лента зелёная

  //инициализация сервопривода
  servo.attach(9); //привязываем привод к порту 9

  //датчик вибрации
  pinMode (pinA0, INPUT);              // Установим вывод A0 как вход
  pinMode (pinD0, INPUT);              // Установим вывод A1 как вход

  //датчик огня
  pinMode(flamePin, INPUT);

  //пьезоизлучатель
  pinMode(10, OUTPUT); // объявляем пин 10 как выход
}
//функция setup----------------------------------------------------------------------------------------------------------------------------------
void loop() { 
  delay(100);
//----------------считывание инфр.дат.движ1------------------
  byte state = digitalRead(sensorPin);//считываем состояние инф.д.дв.1
  digitalWrite(indicator,state);
   if(state == 1){
    commandYell=1;
    YELLOW=true;
    pinMode(33, HIGH);
  }
 else {
  commandYell=0;
  if(YELLOW==false){
  pinMode(33, LOW); 
  delay(100);}}  
 //----------------считывание инфр.дат.движ1------------------
 //----------------считывание инфр.дат.движ2------------------
  byte state2 = digitalRead(sensorPin2);//считываем состояние инф.д.дв.2
  digitalWrite(indicator,state2);

   if(state2 == 1){
    commandWhite=1;
    WHITE=true;
    pinMode(31, HIGH);
  }
 else {
  commandWhite=0;
  if(WHITE==false){
  pinMode(31, LOW); 
  delay(100);}}  
  //----------------считывание инфр.дат.движ2------------------
  //----------------считывание датчика газа--------------------
   float* values = mq2.read(true);
   Serial.println("LPG: "+ String(mq2.readLPG()));
   Serial.println("CO: "+ String(mq2.readCO()));///mq2.readCO()> 0.01000
   Serial.println("Smoke: "+ String(mq2.readSmoke()));
   if(mq2.readLPG()>0.0108)  {
    GAS=true;
   }
  else  if(mq2.readSmoke()>0.0500) //при обнаружении газа включается сигнализация
   {
    GAS=true;
   }
  else GAS=false;

//----------------считывание датчика газа--------------------
 //---------------ультразвуковой дальномер-------------------
  int duration, cm; 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  duration = pulseIn(echoPin, HIGH); 
  cm = duration / 58;
  Serial.print(cm); 
  Serial.println(" cm"); 
  if(cm<=5){
    command=1;
    GREEN=true;
    pinMode(12, HIGH);//LED лента зелёная
  }
 else {
  command=0;
  if(GREEN==false){
  pinMode(12, LOW);//LED лента зелёная  
  delay(100);}
 }  
 //---------------ультразвуковой дальномер-------------------
 
 //---------------датчик вибрации----------------------------
  motionDetected = digitalRead(pinD0); // Считываем показания  
  sensorVal = analogRead(pinA0);       // Считываем показания 
  Serial.print("Anaologue: ");         
  Serial.print(sensorVal);
  Serial.print(" Digital :");
  Serial.println(motionDetected);
  if(sensorVal<1010) //при обнаружении землетрясения включаем сигнализацию
   {
    EQ=true;
    Serial.println("ЗЕМЛЕТРЯСЕНИЕ_Earthquake");
    tone(10, 900); // включаем на пьезодинамик 900 Гц
      delay(1000); // ждем 1 секунду
      noTone(10); // отключаем пьезодинамик
      delay(1000); // ждем 1 секунду
  }
  else{
    EQ=false;
  }
//---------------датчик вибрации----------------------------
//---------------датчик огня--------------------------------
  flame = digitalRead(flamePin);  // считываем данные с датчика
   Serial.print("Flame Sensor - ");  // выводим данные на монитор
   Serial.println(flame);

   Serial.println(" ");
   if(flame==0) //при пожаре включаем сигнализацию
   {
      FLAME=true;
      tone(10, 900); // включаем на пьезодинамик 900 Гц
      delay(1000); // ждем 1 секунду
      noTone(10); // отключаем пьезодинамик на пин 11
      delay(1000); // ждем 1 секунду
   }
   if(flame!=0) FLAME=false; 
   delay(200);
   //---------------датчик огня--------------------------------
   receiveEvent(1);
   requestEvent();
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
 while (0 <Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    Serial.print(c);           /* print the character */
    switch(c)
    {
      case '0':   
      if (command==0){    
            pinMode(12, LOW);//LED лента зелёная
            GREEN=false;}
            break;
      case '1':             
            pinMode(12, HIGH);//LED лента зелёная
            GREEN=true;
            break;   
      case '2': 
      if (commandYell==0){
            pinMode(33, LOW);//LED лента желтая  
            YELLOW=false;}        
            break;         
      case '3': 
            pinMode(33, HIGH);//LED лента желтая  
            YELLOW=true;        
            break;
      case '4': 
      if (commandWhite==0){
            pinMode(31, LOW);//LED лента белая
            WHITE=false;}
            break;
      case '5': 
            pinMode(31, HIGH);//LED лента белая
            WHITE=true;
            break;
      case '6':       
            servo.write(0); //ставим вал1 под 0
            delay(100); //ждем 1 мсек
            servo.write(45); //ставим вал1 под 45
            delay(100); //ждем 1 мсек
            servo.write(90); //ставим вал1 под 90
            delay(100); //ждем 1 мсек
            servo.write(120); //ставим вал1 под 120
            delay(100); //ждем 1 мсек
            servo.write(160); //ставим вал1 под 120
            delay(100); //ждем 1 мсек
            servo.write(180); //ставим вал1 под 175
            delay(100); //ждем 1 мсек            
            break;
      case '7': 
            servo.write(0); //ставим вал1 под 0            
            break;
        default: 
           Serial.println("c is undefined");
            break;
    }    
  }
 Serial.println();             /* to newline */
}

void requestEvent() {  
  if(FLAME)
    Wire.write("1");
  else if(GAS)
    Wire.write("2");
  else if(EQ)
  Wire.write("3"); 
  else Wire.write("0");
}