// Pinos Ponte H L298N
int IN1 = 8;
int IN2 = 9;
int IN3 = 11;
int IN4 = 10;

// Pinos TCRT5000
int ExEsquerda = A0;
int Esquerda = A1;
int Direita = A2;
int ExDireita = A3;
int Meio = A4;
//Refletância dos Sensores
int ValorExE = 0;
int ValorE = 0;
int ValorD = 0;
int ValorExD = 0;
int ValorM = 0;

void setup() {

  Serial.begin(9600);

  //Motores
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  ValorExE = analogRead(ExEsquerda);
  ValorE = analogRead(Esquerda);
  ValorD = analogRead(Direita);
  ValorExD = analogRead(ExDireita);
  ValorM = analogRead(Meio);
}

//Função Mover
void MoverReto() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}
void MoverDireita(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}
void Parar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

//Funções do SegueLinha

void loop() {
  Serial.print(ValorM);
  Serial.print("");

  // Motor A movendo dependendo da refletância recebida
  if (ValorM < 40) {
    MoverReto();
  }
  if (ValorM > 880) {
    MoverDireita();
  }

}
