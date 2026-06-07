#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);
Servo valvulaExaustao;

const int pinoTemp = A0;
const int pinoLDR = A1;
const int pinoVibracao = A2;

const int pinoBuzzer = 8;
const int pinoLedRed = 9;
const int pinoLedGreen = 10;
const int pinoServo = 7;

const float TEMP_CRITICA = 30.0;
const int VIB_CRITICA = 75;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  
  pinMode(pinoBuzzer, OUTPUT);
  pinMode(pinoLedRed, OUTPUT);
  pinMode(pinoLedGreen, OUTPUT);
  
  valvulaExaustao.attach(pinoServo);
  valvulaExaustao.write(0);
  
  lcd.setCursor(0, 0);
  lcd.print("Iniciando sist..");
  delay(2000);
  lcd.clear();
}

void loop() {
  int leituraTemp = analogRead(pinoTemp);
  float temperaturaC = ((leituraTemp * (5.0 / 1023.0)) - 0.5) * 100.0;
  int leituraLDR = analogRead(pinoLDR);
  int vibracaoPct = map(analogRead(pinoVibracao), 0, 1023, 0, 100);

  bool estadoCritico = false;

  if (temperaturaC >= TEMP_CRITICA || vibracaoPct >= VIB_CRITICA) {
    estadoCritico = true;
     
    digitalWrite(pinoLedGreen, LOW);
    digitalWrite(pinoLedRed, HIGH);
    tone(pinoBuzzer, 1000);
    
    if (temperaturaC >= TEMP_CRITICA) {
      valvulaExaustao.write(90); 
    }
  } else {
    digitalWrite(pinoLedRed, LOW);
    digitalWrite(pinoLedGreen, HIGH);
    noTone(pinoBuzzer);
    valvulaExaustao.write(0);
  }

  Serial.print("{\"temperatura\":"); Serial.print(temperaturaC);
  Serial.print(",\"luminosidade\":"); Serial.print(leituraLDR);
  Serial.print(",\"vibracao\":"); Serial.print(vibracaoPct);
  Serial.print(",\"status_critico\":"); Serial.print(estadoCritico ? "true" : "false");
  Serial.println("}");

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperaturaC, 1);
  lcd.print("C L:");
  lcd.print(leituraLDR);
  lcd.print("   "); 

  lcd.setCursor(0, 1);
  if (estadoCritico) {
    lcd.print("! ALERTA CRITICO!");
  } else {
    lcd.print("Vib: ");
    lcd.print(vibracaoPct);
    lcd.print("% NOMINAL"); 
  }

  delay(1000);
}