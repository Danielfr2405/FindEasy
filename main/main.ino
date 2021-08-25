#include <LWiFi.h>
#include <LWiFiClient.h>

#define WIFI_AP "Nome de seu WIF"
#define WIFI_PASSWORD "Senha do seu wifi"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.

LWiFiClient client;
IPAddress server(999, 999, 9, 99); // Ipv4 da API

String latitude = "2327.4283";
String longitude = "04637.2179";
int id = 1;
boolean disconnectedMsg = false;

void setup()
{
  Serial.println("Inicio");
  LWiFi.begin();
  Serial.begin(115200);

  // keep retrying until connected to AP
  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    delay(1000);
  }

  // keep retrying until connected to website
  Serial.println("Connecting to WebSite");
  while (0 == client.connect(server, 5000))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }

  // send HTTP request, ends with 2 CR/LF
  String PostData = montaBody();
  Serial.println("Connected. Sending HTTP GET Request ...."); 
  client.println("POST /Insert HTTP/1.1");  //modify the POST path
  client.println("Host: Localhost");
  client.println("Connection: close");
  client.print("Content-Length: ");
  client.println(PostData.length());
  client.println();
  client.println(PostData);  //send the HTTP POST body

  // waiting for server response
  Serial.println("waiting HTTP response:");
  while (!client.available())
  {
    delay(100);
  }
}

void loop()
{
  // Make sure we are connected, and dump the response content to Serial
  while (client)
  {
    int v =client.read();
    if (v != -1)
    {
      Serial.print((char)v);
    }
    else
    {
      Serial.println("no more content, disconnect");
     client.stop();
      while (1)
      {
        delay(1);
      }
    }
  }

  if (!disconnectedMsg)
  {
    Serial.println("disconnected by server");
    disconnectedMsg = true;
  }
  delay(500);
}

//build JSON method
String montaBody() {
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
