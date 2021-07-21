#include <LTask.h>
#include <LGPS.h>

gpsSentenceInfoStruct info;

/* Transforma cada info recebida do GPS */
const char *nextToken(const char* src, char* buf)
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
void printGPGGA(const char* str)

{
  char latitude[20];
  char longitude[20];
  char buf[20];
  const char* p = str;
  // Serial.print(p);
  p = nextToken(p, 0); // GGA
  p = nextToken(p, 0); // Time
  p = nextToken(p, latitude); // Latitude
  p = nextToken(p, 0); // N
  p = nextToken(p, longitude); // Longitude
  p = nextToken(p, 0); // E
  p = nextToken(p, buf); // fix quality

  if (buf[0] == '1')
  {
    // GPS fix
    p = nextToken(p, buf); // number of satellites
    Serial.print("GPS is fixed:");
    Serial.print(atoi(buf));
    Serial.println(" satellite(s) found!");
    Serial.print("Latitude:");
    Serial.println(latitude);
    Serial.print("Longitude:");
    Serial.println(longitude);
  }
  else
  {
    Serial.println("GPS is not fixed yet.");

  }
}

/* Realiza o setup do programa */
void setup() {
  Serial.begin(9600);
    LGPS.powerOn(); 
}

/* Chamadas do programa */
void loop() {
    LGPS.getData(&info);
    printGPGGA((char*)info.GPGGA);
    Serial.println("**************************************");
    delay(1000);
}
