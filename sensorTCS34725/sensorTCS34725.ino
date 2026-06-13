// ======================================================
// Sensor de cor
// ======================================================

#include <SoftWire.h>
#include "TCS34725.h"

//Sensor Esquerdo
#define SDA1 4
#define SCL1 3

//Sensor Direito
#define SDA2 SDA
#define SCL2 SCL

TCS34725 tcs1 = TCS34725(
  TCS34725_INTEGRATIONTIME_24MS,
  TCS34725_GAIN_1X
);

TCS34725 tcs2 = TCS34725(
  TCS34725_INTEGRATIONTIME_24MS,
  TCS34725_GAIN_1X
);

uint16_t r1, g1, b1, c1;
uint16_t r2, g2, b2, c2;

bool sensor1OK = false;
bool sensor2OK = false;

// ======================================================
// TB6612FNG - Motores
// ======================================================

// Motor A (Esquerdo)
const int PWMA = 9;
const int AIN1 = 8;
const int AIN2 = 7;

// Motor B (Direito)
const int PWMB = 10;
const int BIN1 = 12;
const int BIN2 = 11;

// ======================================================
// Sensores TCRT5000
// ======================================================

const int extEsquerdo = A5;
const int esquerdo = A4;
const int centro = A3;
const int direito = A2;
const int extDireito = A1;

// ======================================================
// Pinos sensor Ultrasônico
//=======================================================

const int trigPin = 5;
const int echoPin = 6;

// ======================================================
// Configurações
// ======================================================

const int limiarSensor = 55;

const int velBase = 100;
const int velCurva = 90;
const int velMax = 220;
const int parado = 0;

// ======================================================
// Variáveis globais dos sensores
// ======================================================

bool pretoExtEsquerdo;
bool pretoEsquerdo;
bool pretoCentro;
bool pretoDireito;
bool pretoExtDireito;

// ======================================================

void setup() {

  // Identificar se o TCS esstá funcionando
  procurarTCS();

  // Motores
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  // Sensores
  pinMode(extEsquerdo, INPUT);
  pinMode(esquerdo, INPUT);
  pinMode(centro, INPUT);
  pinMode(direito, INPUT);
  pinMode(extDireito, INPUT);

  // Ultrasônico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
}

// ======================================================

void loop() {

  //leitura dos sensores
  leituraTCRT();
  printTCS();

  float distancia = distanciaCM();

  if (distancia < 10) {

    desviar();

    Serial.print("Obstaculo: ");
    Serial.println(distancia);

  }

  // Todos pretos --- início tcs34725
  if (pretoExtEsquerdo && pretoEsquerdo && pretoCentro && pretoDireito && pretoExtDireito) {

    // Volta um pouco
    moverTras(velBase);
    delay(900);

    pararMotores();
    delay(200);

    // Atualiza leituras dos TCS
    if(sensor1OK)
      tcs1.getRawData(&r1, &g1, &b1, &c1);

    if(sensor2OK)
      tcs2.getRawData(&r2, &g2, &b2, &c2);

    bool verdeEsquerda = ehVerde(r1, g1, b1);
    bool verdeDireita = ehVerde(r2, g2, b2);

    Serial.print("Verde E: ");
    Serial.print(verdeEsquerda);

    Serial.print(" Verde D: ");
    Serial.println(verdeDireita);

    // Ambos verdes
    if (verdeEsquerda && verdeDireita) {
      virar180(120, 400);
    }

    // Só esquerda
    else if (verdeEsquerda) {

      virarEsquerda90(120, 9000);
      }

    // Só direita
    else if (verdeDireita) {

      virarDireita90(120, 1200);
    }

    // Nenhum verde
    else {

      moverFrente(velBase);
      delay(300);
    }
  }
  // Linha bem à esquerda
  else if (pretoExtEsquerdo && !pretoExtDireito) {
    virarEsquerda90(velBase, 900);
  }
  // Linha bem à direita
  else if (!pretoExtEsquerdo && pretoExtDireito) {
    virarDireita90(velBase, 900);
  }
  // Vendo apenas Branco
  else if (!pretoEsquerdo && !pretoDireito) {
    moverFrente(velBase);
  }
  // Linha puxando para esquerda
  else if (pretoEsquerdo && !pretoDireito) {
    moverEsquerda(velBase);
  }
  // Linha puxando para direita
  else if (!pretoEsquerdo && pretoDireito) {
    moverDireita(velBase);
  }

  // Perdeu a linha
  else {
    pararMotores();
  }
}

// ======================================================
// TCS34752
// ======================================================

// Procurar o SENSOR
void procurarTCS() {
  Serial.println("Iniciando sensores...");

    if (tcs1.begin(0x29, SDA1, SCL1)) {
      Serial.println("Sensor 1 encontrado");
      sensor1OK = true;
    } else {
      Serial.println("Sensor 1 NAO encontrado");
    }

    if (tcs2.begin(0x29, SDA2, SCL2)) {
      Serial.println("Sensor 2 encontrado");
      sensor2OK = true;
    } else {
      Serial.println("Sensor 2 NAO encontrado");
    }
  }

// Informações SENSOR
void printTCS() {
  // =====================
  // SENSOR 1
  // =====================

    if (sensor1OK) {

      tcs1.getRawData(&r1, &g1, &b1, &c1);

      Serial.print("S1 -> ");
      Serial.print("R:");
      Serial.print(r1);

      Serial.print(" G:");
      Serial.print(g1);

      Serial.print(" B:");
      Serial.print(b1);

      Serial.print(" C:");
      Serial.print(c1);

      Serial.print(" | ");
    }

  // =====================
  // SENSOR 2
  // =====================

    if (sensor2OK) {

      tcs2.getRawData(&r2, &g2, &b2, &c2);

      Serial.print("S2 -> ");
      Serial.print("R:");
      Serial.print(r2);

      Serial.print(" G:");
      Serial.print(g2);

      Serial.print(" B:");
      Serial.print(b2);

      Serial.print(" C:");
      Serial.print(c2);
    }

    Serial.println();

}

// Lógica verde
  bool ehVerde(uint16_t r, uint16_t g, uint16_t b) {

    if (g > r * 1.5 && g > b * 1.5) {
      return true;
    }

    return false;
  }



// ======================================================
// Leitura dos sensores
// ======================================================

  void leituraTCRT() {

    pretoExtEsquerdo = analogRead(extEsquerdo) > limiarSensor;
    pretoEsquerdo = analogRead(esquerdo) > limiarSensor;
    pretoCentro = analogRead(centro) > limiarSensor;
    pretoDireito = analogRead(direito) > limiarSensor;
    pretoExtDireito = analogRead(extDireito) > limiarSensor;

    Serial.print(pretoExtEsquerdo);
    Serial.print(" ");

    Serial.print(pretoEsquerdo);
    Serial.print(" ");

    Serial.print(pretoCentro);
    Serial.print(" ");

    Serial.print(pretoDireito);
    Serial.print(" ");

    Serial.println(pretoExtDireito);
  }

// ======================================================
// Ultrasônico Medição
// ======================================================

  float distanciaCM() {

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duracao = pulseIn(echoPin, HIGH, 30000);

    if (duracao == 0) {
      return 999;
    }

    return duracao * 0.034 / 2.0;
  }

// ======================================================
// Movimentos
// ======================================================

  void moverFrente(int vel) {

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);

    analogWrite(PWMA, vel + 35);
    analogWrite(PWMB, vel);
  }

  void moverTras(int vel) {

    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);

    analogWrite(PWMA, vel + 20);
    analogWrite(PWMB, vel);
  }

  void moverDireita(int vel) {

    analogWrite(PWMA, (vel + 35) / 3);
    analogWrite(PWMB, vel);

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  }

  void moverEsquerda(int vel) {

    analogWrite(PWMA, vel + 35);
    analogWrite(PWMB, vel / 3);

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  }

  void pararMotores() {

    analogWrite(PWMA, 0);
    analogWrite(PWMB, 0);
  }

  // ======================================================
  // Curvas de 90°
  // ======================================================

  void virarEsquerda90(int vel, int time) {
    // Frente para alinhar
    moverFrente(velBase);

    delay(300);

    // Esquerdo frente
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    // Direito ré
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);

    analogWrite(PWMA, vel + 35);
    analogWrite(PWMB, vel);

    delay(time);

    pararMotores();
  }

  void virarDireita90(int vel, int time) {
    // Frente para alinhar
    moverFrente(velBase);

    delay(300);

    // Esquerdo ré
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

    // Direito frente
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);

    analogWrite(PWMA, vel + 35);
    analogWrite(PWMB, vel);

    delay(time);

    pararMotores();
  }

  // Curvar de 180°

  void virar180(int vel, int time) {

    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);

    analogWrite(PWMA, vel + 35);
    analogWrite(PWMB, vel);

    delay(time);

    pararMotores();
  }

  void direita90(int vel, int time) {
    // Esquerdo ré
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);

    // Direito frente
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);

    analogWrite(PWMA, vel + 35);
    analogWrite(PWMB, vel);

    delay(time);
  }
  void esquerda90(int vel, int time) {
    // Esquerdo ré
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    // Direito frente
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);

    analogWrite(PWMA, vel + 35);
    analogWrite(PWMB, vel);

    delay(time);
  }

  // Desviar do Obstáculo:
  void desviar() {

    direita90(velBase, 1780);

    moverFrente(velBase);
    delay(1970);

    esquerda90(velBase, 1755);

    moverFrente(velBase);
    delay(3800);

    esquerda90(velBase, 1700);

    moverFrente(velBase);
    delay(1800);

    direita90(velBase, 1760);

  }