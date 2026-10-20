#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>
#include "esp_sntp.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);
const char *nomeRedeWiFi = "SENAI EDUCACIONAL";
const char *senhaRedeWiFi = "info@134";
const String enderecoAPI = "https://wttr.in/Sao+Caetano+do+Sul?m&lang=pt&format=";
//? SERVIDORES NTP
const char *NTP1 = "a.st1.ntp.br";
const char *NTP2 = "pool.ntp.org";
const char *FUSO = "<-03>3";
const char *SEMANA[] = {"DOMINGO", "SEGUNDA", "TERCA", "QUARTA", "QUINTA", "SEXTA", "SABADO"};

String pedirParaAPIClima(String codigo);

void setup()
{
  lcd.init();
  lcd.backlight();

  Serial.begin(115200);

  //? CONFIGURAÇÃO E INICIALIZAÇÃO DO WIFI
  WiFi.begin(nomeRedeWiFi, senhaRedeWiFi);
  Serial.print("\n\rIniciando conexão WiFi\n\r");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("#");
    delay(500);
  }
  Serial.print("\n\rConectado com sucesso!\n\r");
  Serial.printf("Endereço IP: %s\n\r", WiFi.localIP().toString().c_str());
  Serial.printf("Endereço MAC: %s\n\r", WiFi.macAddress().c_str());

  sntp_set_sync_interval(1 * 60 * 60 * 1000); //? Intervalo de atualização da hora na internet padrão (1*60*60*1000)

  //? CONFIGURAÇÃO DE FUSO E SERVIDORES NTP E INICIALIZAÇÃO DA SINCRONIZAÇÃO
  configTzTime(FUSO, NTP1, NTP2);
  struct tm agora;
  Serial.print("\n\rSincronizando a hora\n\r");

  while (!getLocalTime(&agora)) //? O parâmetro é o endereço da estrutura tipo tm.
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\n\rHora sincronizada com sucesso!");
}

void loop()
{
  struct tm agora;
  if (getLocalTime(&agora))
  {
    lcd.setCursor(0, 0);
    lcd.printf("|%s|%02d:%02d|",
               SEMANA[agora.tm_wday], agora.tm_hour, agora.tm_min);
    delay(500);
  }

  String clima;
  int temperatura;
  int ST;
  int umidade;
  clima = pedirParaAPIClima("%C");
  lcd.setCursor(0, 1);
  lcd.printf("|CLIMA: ");
  // lcd.setCursor(0, 1);
  // lcd.printf("%s", clima.c_str());
  temperatura = pedirParaAPIClima("%t").toInt();
  ST = pedirParaAPIClima("%f").toInt();
  lcd.setCursor(0, 2);
  lcd.printf("|TEMP.:%dC|ST.:%dC|", temperatura, ST);
  umidade = pedirParaAPIClima("%h").toInt();
  lcd.setCursor(0, 3);
  lcd.printf("|UMIDADE:%d%%", umidade);
}

String pedirParaAPIClima(String codigo)
{
  HTTPClient http;
  http.begin(enderecoAPI + codigo);

  int codigoHTTP = http.GET(); //? Retorna o código HTTP.

  String resposta;

  if (codigoHTTP == 200)
  {
    resposta = http.getString(); //? Retorna o conteúdo HTML da página.
  }

  else
  {
    Serial.printf("Deu erro. Código: %d", codigoHTTP);
  }

  http.end();
  return resposta;
}
