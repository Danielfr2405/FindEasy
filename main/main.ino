#include <LWiFi.h>
#include <LWiFiClient.h>
#include <LTask.h>

#define WIFI_AP "nome_de_seu_wifi"
#define WIFI_PASSWORD "senha_do_seu_wifi"
#define WIFI_AUTH LWIFI_WPA // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.

int id = 1;
LWiFiClient client;

void setup()
{
  LWiFi.begin();
  Serial.begin(115200);
  connectWifi("setup");
}

void loop()
{
  while (1)
  {
    connectAPI();

    connectWifi("loop");

    // send HTTP request, ends with 2 CR/LF
    String PostData = montaBody("9999.4283", "99999.2179");
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

    delay(1000);
  }
}

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
      delay(500);
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
