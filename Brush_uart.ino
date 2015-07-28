/*Mega2560
external interrupt int.0    int.1    int.2   int.3   int.4   int.5            
pin                 2        3        21      20      19      18
/*
 * USART_RXinterrupt.pde: 使用 USART 接收資料&#65292;中斷版本
 */
 /*  I2C
     Mega2560	20 (SDA), 21 (SCL)   ;  UNO     A4(SDA), A5(SCL)
 */
#include <Hx711.h>
#include <Wire.h> //I2C
#define SLAVE1_ID 1
 
Hx711 scale_1(A1, A0);    //set analog pins
Hx711 scale_2(A3, A2);    //set analog pins
//Hx711 scale_3(A5, A4);    //set analog pins
int a = 0;
int brushtime = 3000;
/* #1 */
const int Brush1_In1 = 30;  // brush #1
const int Brush1_In2 = 32;  // brush #1
const int Motor1_In3 = 34; // motor #1
const int Motor1_In4 = 36; // motor #1
const int EN1_A = 4;      // Enable Brush(#1)
const int EN1_B = 5;      // Enable motor(#1)
const int led = 10;
/* #2 */
const int Brush2_In1 = 22; // brush #2
const int Brush2_In2 = 24; // brush #2
const int Motor2_In3 = 26; // motor #2
const int Motor2_In4 = 28; // motor #2
const int EN2_A = 6;     // Enable Brush(#2)
const int EN2_B = 7;     // Enable motor(#2)
/*IR MODULE*/
const int ir_module_1 = 2; //INT 0
const int ir_module_2 = 3; //INT 1
const int ir_module_3 = 19; //INT 4
const int ir_module_4 = 18; //INT 5
int readybit1 = 0;
int readybit2 = 0;
volatile int buttonState; 
void setup() {
  /* Set USART0 Interrupt */
  mySerial_begin(9600);  
  /* Set I2C, Communication Between 2 MEGA */
  Wire.begin();        // join i2c bus (address optional for master)
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  
  /*IR PIN*/
  pinMode(ir_module_1, INPUT);
  pinMode(ir_module_2, INPUT);
  pinMode(ir_module_3, INPUT);
  pinMode(ir_module_4, INPUT);
  /* #1 PIN*/
  pinMode(Brush1_In1, OUTPUT);
  pinMode(Brush1_In2, OUTPUT);
  pinMode(Motor1_In3, OUTPUT);
  pinMode(Motor1_In4, OUTPUT);
  pinMode(EN1_A, OUTPUT);
  pinMode(EN1_B, OUTPUT);
  /* #2 PIN*/
  pinMode(EN2_A, OUTPUT);
  pinMode(EN2_B, OUTPUT);    
  pinMode(Brush2_In1, OUTPUT);
  pinMode(Brush2_In2, OUTPUT);
  pinMode(Motor2_In3, OUTPUT);
  pinMode(Motor2_In4, OUTPUT);
/* INTR
  EIMSK |= _BV(INT0); 
  EICRA &= ~_BV(ISC00);  
  EICRA |= _BV(ISC00);
  */
  /* #1 Motor Set Limit Switch*/
  attachInterrupt(0, Touchfront1 , LOW); //INT 0 in PIN 2
  attachInterrupt(1, Touchfront2, LOW);  //INT 1 in PIN 3
  /* #2 Motor Set Limit Switch*/
  attachInterrupt(4, Touchback1, LOW);   //INT 4 in PIN 19
  attachInterrupt(5, Touchback2, LOW);   //INT 5 in PIN 18
}

void loop() {
  /*
  Wire.requestFrom(SLAVE1_ID, 1); // request 1 bytes from slave device #1(SLAVE1_ID) 此為發出要求
  if(scale_3.getGram() > 500.0){
    /*放LED看有沒有到定位(左輪)(Master)*/
  /*  while(Wire.available()){
      if(Wire.read()=='Y')
      b == 1;
      /*放LED看有沒有到定位(右輪)(Slave)*/
 /*     if(Wire.read()=='N')
      b == 0;
    }
  }
  */
  if(a == 1){
    digitalWrite(led, HIGH);
    /*想一下master傳給slave的UART指令要放在哪裡*/
    float result1 = scale_1.getGram();  /*1kg strain gauge*/
    float result2 = scale_2.getGram();  /*1kg strain gauge*/
    //Serial.println(result1);
    analogWrite(EN1_A, 155);
    analogWrite(EN1_B, 155);
    analogWrite(EN2_A, 155);
    analogWrite(EN2_B, 155);
      if(result1 < -8.0) {
        motorstop(Motor1_In3, Motor1_In4);
        motorstop(Motor2_In3, Motor2_In4);
        readybit1 = 1;
        digitalWrite(8, HIGH);
        if(result2 < -8.0){
          motorstop(Motor2_In3, Motor2_In4);
          readybit2 = 1;  
        }
        else{
          forward(Motor2_In3, Motor2_In4);
          readybit2 = 0;
        }
      }
      else if(result2 <-8.0){
        motorstop(Motor2_In3, Motor2_In4); 
        readybit2 = 1; 
        digitalWrite(9, HIGH);
        //Serial.println("B");  
      }
      else{
        forward(Motor1_In3, Motor1_In4);
        motorstop(Brush1_In1, Brush1_In2);
        forward(Motor2_In3, Motor2_In4);
        motorstop(Brush2_In1, Brush2_In2);
        readybit1 = 0;
        readybit2 = 0;
        //Serial.println("C");
      }
  }
  if(a == 0){
    motorstop(Motor1_In3, Motor1_In4);
    motorstop(Motor2_In3, Motor2_In4);
    motorstop(Brush1_In1, Brush1_In2);
    motorstop(Brush2_In1, Brush2_In2);
    digitalWrite(led, LOW);
  }
}
// 讀取 buttonPin 的狀態&#65292;並反應到 ledPin 上
/*
void buttonStateChanged() {
  buttonState = digitalRead(ir_module_1);
  digitalWrite(44, buttonState);
}
ISR(INT0_vect){
  buttonStateChanged();
}*/

ISR(USART0_RX_vect)
{
  char ReceivedChar;
  ReceivedChar  = UDR0;
  if(ReceivedChar=='1'){
  UDR0 = ReceivedChar;
  Wire.beginTransmission(SLAVE1_ID); // transmit to device #1
  Wire.write('1');              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  a = 1;
  }
  else if(ReceivedChar=='0'){
  UDR0 = ReceivedChar;
  Wire.beginTransmission(SLAVE1_ID); // transmit to device #1
  Wire.write('0');              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  a = 0;
  }
  
}
void led_HIGH(){
  digitalWrite(led, HIGH);
  delay(5000);
}
void led_LOW(){
  digitalWrite(led, LOW);
  delay(5000);
}

void mySerial_begin(unsigned long baud)
{
  // F_CPU is CPU clock, for Arduino UNO or Duemilinove, it is 16000000
  unsigned int baud_setting = (F_CPU / 16 / baud) - 1;
  
  // Set the baud rate 
  UBRR0H = (unsigned char)(baud_setting >> 8);
  UBRR0L = (unsigned char)baud_setting;
  
  // Enable receiver and transmitter, enable RX interrupt
  UCSR0B |= _BV(RXEN0) | _BV(TXEN0); 
  UCSR0B |= _BV(RXCIE0);
  UCSR0B &= ~_BV(TXCIE0);
  
  // Set frame format: No parity check, 8 Data bits, 1 stop bit
  UCSR0C |= _BV(UCSZ01) | _BV(UCSZ00);
}
void brush(int t){
  motorstop(Motor1_In3, Motor1_In4); 
  motorstop(Motor2_In3, Motor2_In4); 
  forward(Brush1_In1, Brush1_In2);
  forward(Brush2_In1, Brush2_In2);
  delay(t*1000);
}

void forward(int Motor1, int Motor2){
  digitalWrite(Motor1, HIGH);
  digitalWrite(Motor2, LOW);
  delay(50);
}

void backward(int Motor1, int Motor2){
  digitalWrite(Motor1, LOW);
  digitalWrite(Motor2, HIGH); 
  delay(50);
}
void motorstop(int Motor1, int Motor2){
  digitalWrite(Motor1, LOW);
  digitalWrite(Motor2, LOW);
  delay(50);
}
void Touchfront1(){
  motorstop(Motor1_In3, Motor1_In4);
  delay(50);
  backward(Motor1_In3, Motor1_In4);
  digitalWrite(38, HIGH);
  delay(5000);
  //Serial.println("INTR");
}

/*Interrupt Function*/
void Touchback1(){
  motorstop(Motor1_In3, Motor1_In4);
  delay(50);
  digitalWrite(40, HIGH);
  delay(5000);
  forward(Motor1_In3, Motor1_In4);
  //Serial.println("INTR");
}
void Touchfront2(){
  motorstop(Motor2_In3, Motor2_In4);
  delay(50);
  digitalWrite(42, HIGH);
  delay(5000);
  backward(Motor2_In3, Motor2_In4);
   //Serial.println("INTR");
}

void Touchback2(){
  motorstop(Motor2_In3, Motor2_In4);
  delay(50);
  digitalWrite(44, HIGH);
  delay(5000);
  forward(Motor2_In3, Motor2_In4);
   //Serial.println("INTR");
}
