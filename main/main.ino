#include <LTask.h>
#include <LGPS.h>
#include <LWiFi.h>
#include <LWiFiClient.h>

#define WIFI_AP "nome_de_seu_wifi"
#define WIFI_PASSWORD "senha_do_seu_wifi"
#define WIFI_AUTH LWIFI_WPA // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.

gpsSentenceInfoStruct info;
int id = 1;
LWiFiClient client;
char latitude[20];
char longitude[20];

/* Realiza o setup do programa */
void setup()
{
  Serial.begin(115200);
  LGPS.powerOn();
  Serial.println("----- Inicio - Setup -----");
  Serial.println("[Setup] GPS inicializado.");
  connectWifi("setup");
  Serial.println("------ Fim - Setup ------");
  Serial.println("-------------------------");
}

/* Chamadas do programa */
void loop()
{
  LGPS.getData(&info);

  Serial.println("---- Inicio - Loop ----");
  printGPGGA();
  Serial.println("-----------------------");

  connectAPI();

  connectWifi("loop");

  // send HTTP request, ends with 2 CR/LF
  String PostData = montaBody(latitude, longitude);
  Serial.println("[API] Connectado. Enviando HTTP Post Request ....");
  client.println("POST /Insert HTTP/1.1"); //modify the POST path
  client.println("Host: Localhost");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(PostData.length());
  client.println();
  client.println(PostData); //send the HTTP POST body

  // waiting for server response
  Serial.println("[API] Aguardando Retorno...");
  while (!client.available())
  {
    delay(100);
  }

  int value;
  while (client.available())
  {
    value = client.read();
    if (value < 0)
    {
      Serial.println("[API] Não houve nenhum retorno da API.");
      break;
    }
    Serial.print((char)value); // Add this line to the code in test case
  }

  Serial.println("[API] Finalizado");
  Serial.println("------- Fim - Loop -------");
  Serial.println("**************************");

  delay(1000);
}

/**
 * Funções para funcionalidade de GPS
 */

/* Transforma cada info recebida do GPS */
const char *nextToken(const char *src, char *buf)
{
  int i = 0;
  while (src[i] != 0 && src[i] != ',')
    i++;
  if (buf)
  {
    strncpy(buf, src, i);
    buf[i] = 0;
  }
  if (src[i])
    i++;
  return src + i;
}

/* Printa cada info recebida do GPS */
void printGPGGA()
{
  bool isFounded = false;

  while (!isFounded)
  {
    const char *str = (char *)info.GPGGA;
    char buf[20];
    const char *p = str;

    Serial.println("------ Info. GPS ------");
    Serial.println((char *)info.GPGGA);
    Serial.println("");

    p = nextToken(p, 0);         // GGA
    p = nextToken(p, 0);         // Time
    p = nextToken(p, latitude);  // Latitude
    p = nextToken(p, 0);         // N
    p = nextToken(p, longitude); // Longitude
    p = nextToken(p, 0);         // E
    p = nextToken(p, buf);       // fix quality

    isFounded = buf[0] == '1';
//    isFounded = strlen(latitude) > 0 && strlen(longitude);

    if (isFounded)
    {
      // GPS fix
      p = nextToken(p, buf); // number of satellites
      Serial.print(atoi(buf));
      Serial.println(" satelite(s) encontrados!");
      Serial.print("Latitude: ");
      Serial.println(latitude);
      Serial.print("Longitude: ");
      Serial.println(longitude);
    }
    else
    {
      Serial.println("Sem conexão com nenhum satelite!");
    }

    delay(100);
  }
}

/**
 * Funções para funcionalidade de WIFI
 */
void connectAPI()
{
  // keep retrying until connected to website
  Serial.println("[API] Conectando");
  IPAddress server(999, 999, 9, 99); // Ipv4 da API
  while (0 == client.connect(server, 5000))
  {
    Serial.println("[API] Re-Conectando");
    delay(100);
  }

  Serial.println("[API] Conectado");
}

void connectWifi(String caminho)
{
  LWifiStatus ws = LWiFi.status();
  boolean status = wifi_status(ws);
  if (!status)
  {
    Serial.println("[WIFI-" + caminho + "] Conectando");
    while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
    {
      Serial.println("[WIFI-" + caminho + "] Re-Conectando");
      delay(100);
    }
  }
  Serial.println("[WIFI-" + caminho + "] Conectado");
}

boolean wifi_status(LWifiStatus ws)
{
  switch (ws)
  {
  case LWIFI_STATUS_DISABLED:
    return false;
    break;
  case LWIFI_STATUS_DISCONNECTED:
    return false;
    break;
  case LWIFI_STATUS_CONNECTED:
    return true;
    break;
  }
  return false;
}

//build JSON method
String montaBody(String latitude, String longitude)
{
  String data = "{";
  data += "\n";
  data += "\"latitude\": \"";
  data += latitude;
  data += "\",";
  data += "\n";
  data += "\"longitude\": \"";
  data += longitude;
  data += "\",";
  data += "\n";
  data += "\"id\": ";
  data += id;
  data += "\n";
  data += "}";

  Serial.println(data);

  return data;
}
