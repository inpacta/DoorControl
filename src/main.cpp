#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Define as frequências das notas musicais
#define NOTE_C4 262
#define NOTE_E4 330
#define NOTE_G4 392
#define NOTE_C5 523
#define NOTE_G3 196
#define NOTE_D4 294

// Substitua com suas credenciais de rede
const char* ssid = "NOME_DA_REDE";
const char* password = "SENHA_DO_WIFI";
const char* mqttServer = "endereco_do_broker";
const int mqttPort = 1883;
const char* mqttUser = "usuario"; // Opcional
const char* mqttPassword = "senha"; // Opcional

// Tópico MQTT para receber mensagens
const char *topicStatus_TAG = "SEU/TOPICO";
const char *topicStatus_RF = "SEU/TOPICO";

// Inicializa o cliente WiFi e o cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Define os pinos do
#define buzzPin 4
#define portPin 2
#define buttonPin 12

unsigned long sensorActivatedTime = 0;
bool sensorActive = false;
bool sensorProcessed = false;
String lastMessage_TAG = "";
String lastMessage_RF = "";
unsigned long lastMessageTime_TAG = 0;
unsigned long lastMessageTime_RF = 0;
const unsigned long ignoreIntervalSensor = 2000; // 2 segundos
const unsigned long ignoreInterval = 10000;      // 10 segundos

// Função para tocar o som de inicialização
void playStartupTone()
{
  tone(buzzPin, NOTE_C4, 200);
  delay(250); // Espera a nota terminar + um pequeno intervalo
  tone(buzzPin, NOTE_E4, 200);
  delay(250);
  tone(buzzPin, NOTE_G4, 200);
  delay(250);
  tone(buzzPin, NOTE_C5, 200);
  delay(250);
  noTone(buzzPin); // Para de tocar
}

// Função para tocar o som de usuário aceito
void playOpenDoorTone()
{
  tone(buzzPin, NOTE_G4, 100);
  delay(150);
  tone(buzzPin, NOTE_C5, 100);
  delay(150);
  noTone(buzzPin); // Para de tocar
}

// Função para tocar o som de usuário adicionado ou retirado
void playCloseDoorTone()
{
  tone(buzzPin, NOTE_D4, 200);
  delay(250); // Espera a nota terminar + um pequeno intervalo
  tone(buzzPin, NOTE_E4, 200);
  delay(250);
  tone(buzzPin, NOTE_C4, 200);
  delay(250);
  noTone(buzzPin); // Para de tocar
}

// Função para abrir a porta
void openDoor()
{
  Serial.println("Abrindo a porta");
  digitalWrite(portPin, LOW);
  playOpenDoorTone();
  delay(2000); // Mantém a porta aberta por 2 segundos
  digitalWrite(portPin, HIGH);
  playCloseDoorTone();
  Serial.println("Porta fechada");
}

// Configura a conexão WiFi
void setup_wifi()
{
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado ao WiFi");
}

// Função de callback para quando uma mensagem é recebida
void callback(char *topic, byte *payload, unsigned int length)
{
  String message = "";
  for (unsigned int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }

  unsigned long currentTime = millis();

  if (strcmp(topic, topicStatus_TAG) == 0)
  {
    if (message != lastMessage_TAG || (currentTime - lastMessageTime_TAG) > ignoreInterval)
    {
      openDoor();
      lastMessage_TAG = message;
      lastMessageTime_TAG = currentTime;
    }
    else
    {
      Serial.println("Mensagem TAG ignorada, ainda é a mesma pessoa.");
    }
  }

  if (strcmp(topic, topicStatus_RF) == 0)
  {
    if (message != lastMessage_RF || (currentTime - lastMessageTime_RF) > ignoreInterval)
    {
      openDoor();
      lastMessage_RF = message;
      lastMessageTime_RF = currentTime;
    }
    else
    {
      Serial.println("Mensagem RF ignorada, ainda é a mesma pessoa.");
    }
  }
}

// Reconecta ao MQTT
void reconnectMQTT()
{
  while (!client.connected())
  {
    Serial.print("Tentando conectar ao MQTT...");
    String mac = WiFi.macAddress();
    if (client.connect(mac.c_str(), mqttUser, mqttPassword))
    {
      client.subscribe(topicStatus_TAG);
      client.subscribe(topicStatus_RF);
      Serial.println("Conectado");
    }
    else
    {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(portPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);

  digitalWrite(portPin, HIGH);

  setup_wifi();

  // Conectando ao servidor MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  reconnectMQTT();

  playStartupTone();
}

void loop()
{
  if (!client.connected())
    reconnectMQTT();

  int sensorValue = digitalRead(buttonPin);

  if (sensorValue == HIGH)
  {
    if (!sensorActive && !sensorProcessed)
    {
      sensorActivatedTime = millis();
      sensorActive = true;
    }

    if (sensorActive)
    {
      unsigned long currentTime = millis();
      unsigned long elapsedTime = currentTime - sensorActivatedTime;

      if (elapsedTime < ignoreIntervalSensor)
      {
        openDoor();
        sensorActive = false;   // Reset the sensor state after opening the door
        sensorProcessed = true; // Mark the sensor as processed
      }

      Serial.println("Modo noturno ativado!");
    }
  }
  else
  {
    if (sensorProcessed)
    {
      Serial.println("Modo noturno desativado!");
      sensorActive = false;
      sensorProcessed = false; // Reset the processed state when the sensor is deactivated
    }
  }

  client.loop();
}
