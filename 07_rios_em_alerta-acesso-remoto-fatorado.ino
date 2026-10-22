//BIBLIOTECAS
#include <WiFi.h>
#include <HTTPClient.h>
#include <Ultrasonic.h>

//CONFIGURAÇÃO DO WI-FI
const char* ssid = "Wifi da Prof";
const char* senha = "agi8vum642@";
//ENDEREÇO DA API
const char* API_URL = "http://localhost:5500";

//PINOS DA ESP
#define verde        22
#define vermelho     21
#define buzzer       19
#define trig         18
#define echo         17
Ultrasonic sensor(trig, echo);

//Variáveis auxiliares
float distancia = -1;
String nivel = "Sem leitura";
unsigned long tempoBuzzer = 0;
bool buzzerLigado = false;

//Verifica conexão com o Wi-Fi
void conectarWiFi() {
  Serial.println();
  Serial.println("Conectando ao Wi-Fi: " + ssid);
  WiFi.begin(ssid, senha);
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 30) {
    delay(500); Serial.print(".");
    tentativas++;
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi conectado!");
    Serial.println("IP do ESP32: " + WiFi.localIP());
  } else {
    Serial.println("Não foi possível conectar ao Wi-Fi.");
  }
}

//Detecta a distância lida pelo sensor
void lerDistancia() {
  distancia = sensor.read();
  controlarLED();
}

//Muda a cor do LED conforme a distância lida
void controlarLED() {
  digitalWrite(vermelho, LOW);
  digitalWrite(verde, LOW);
  digitalWrite(azul, LOW);

  if (distancia < 0) {
    nivel = "Sem leitura";
  }
  else if (distancia > 50) {
    nivel = "Normal";
    digitalWrite(verde, HIGH);
  }
  else if (distancia > 25) {
    nivel = "Atencao";
    digitalWrite(verde, HIGH);
    digitalWrite(vermelho, HIGH);
  }
  else {
    nivel = "Alerta";
    digitalWrite(vermelho, HIGH);
  }
}

//Toca o buzzer somente no nível Alerta
void controlarBuzzer() {
  if (nivel == "Alerta") {
    if (millis() - tempoBuzzer >= 250) {
      tempoBuzzer = millis();
      buzzerLigado = !buzzerLigado;
      if (buzzerLigado) tone(buzzer, 1000); else noTone(buzzer);
    }
  } else {
    buzzerLigado = false;
    noTone(buzzer);
  }
}

//Enviar as informações do sensor para o site
void enviarDados() {
  //Verifica a conexão Wi-Fi antes de enviar os dados  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi desconectado. Tentando reconectar...");
    conectarWiFi();
    return;
  }

  //Inicia o protocolo HTTP para receber dados via JSON
  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");

  //Monta o JSON que será enviado para o servidor
  String dados = "{\"distancia\":" + String(distancia) +
                  ",\"nivel\":\"" + nivel + "\"}";

  //Mostra no monitor serial a resposta do servidor
  int resposta = http.POST(dados);
  Serial.println("Resposta do servidor: " + resposta);
  if (resposta > 0) {
    String respostaServidor = http.getString();
    Serial.println("Servidor respondeu: " + respostaServidor);
  } else {
    Serial.println("Erro ao enviar os dados.");
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  pinMode(verde, OUTPUT);
  pinMode(vermelho, OUTPUT);
  pinMode(buzzer, OUTPUT);

  //Inicializa com tudo desligado
  digitalWrite(verde, LOW);
  digitalWrite(vermelho, LOW);
  noTone(buzzer);

  conectarWiFi();
}

void loop() {  
  lerDistancia();
  delay(100);
  controlarBuzzer();
}