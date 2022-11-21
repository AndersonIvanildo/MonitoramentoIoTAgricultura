#define BLYNK_TEMPLATE_ID "##################"
#define BLYNK_DEVICE_NAME "Aplicação do IoT na Agricultura"
#define BLYNK_AUTH_TOKEN "###################"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

// WiFi credentials.
char ssid[] = "";
char pass[] = "";

BlynkTimer timer;
const int pinSensor = A0;
int valorUmidade = 0;
const int pinControleBomba = 12;

int controleAutomatico = 0;
int ligarBomba = 0;
int bombaLigada = 0;

BLYNK_WRITE(V0)
{
  controleAutomatico = param.asInt();
}

BLYNK_WRITE(V1)
{
  ligarBomba = param.asInt();  
}

void flipflopBomba() // Função que mandará o sinal para ligar ou desligar a Bomba
{
  if (bombaLigada == 1)
  {
    digitalWrite(pinControleBomba, LOW);
    bombaLigada = 0;
  }
  else
  {
    bombaLigada = 1;
  }
  
  digitalWrite(pinControleBomba, HIGH);
  delay(500);
  
}

void enviarDadosUmidade() // Função responsável por analizar o valor da umidade atual e enviar para o Blynk
{
  valorUmidade = analogRead(pinSensor); // Os valores de entrada será entre 0 e 1024.
  Serial.println(valorUmidade);
  Blynk.virtualWrite(V4, valorUmidade);
}


void monitoraPlantacao() // Função que irá controlar a bomba caso o controle da plantação seja Automático.
{
    if(valorUmidade > 1000) // Caso o solo esteja seco.
    {
       digitalWrite(pinControleBomba, HIGH);
    }
    else
    {
      digitalWrite(pinControleBomba, LOW);
    }
}

void verificarModoDeControle()
{
  if(controleAutomatico == 1)
  {
      monitoraPlantacao();
  }
  else
  {
    if(ligarBomba == 1)
    {
      digitalWrite(pinControleBomba, HIGH);
    }
    else
    {
      digitalWrite(pinControleBomba, LOW);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  pinMode(pinSensor, INPUT);
  pinMode(pinControleBomba, OUTPUT);

  // Configurando função para ser chamada a cada segundo.
  timer.setInterval(1000L, enviarDadosUmidade);
  timer.setInterval(1000L, verificarModoDeControle);
}

void loop()
{
  Blynk.run();
  timer.run();
}
