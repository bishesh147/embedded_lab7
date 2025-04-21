#define MQTTCLIENT_QOS2 1

#include <SPI.h>
#include <WiFi.h>
#include <WifiIPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>


#define LED RED_LED

//// your network name also called SSID
//char ssid[] = "CTNLaptop";
//// your network password
//char password[] = "7296w:1O";

// your network name also called SSID
char ssid[] = "PhewaTal_Wifi";
// your network password
char password[] = "pOkHaRa@69!";

char printbuf[100];

int arrivedcount = 0;

void check_temperature(float temp){
  float threshold = 20.0;
  if (temp > threshold){
    Serial.print("Temperature greater than threshold\n");
    digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
  else{
    Serial.print("Temperature is within the threshold\n");
    digitalWrite(LED, LOW);   // turn the LED on (HIGH is the voltage level)
  }
}

void messageArrived(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;
  float temp;
  
  sprintf(printbuf, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n", 
    ++arrivedcount, message.qos, message.retained, message.dup, message.id);
  Serial.print(printbuf);
  sprintf(printbuf, "Payload %s\n", (char*)message.payload);
  Serial.print(printbuf);
  temp = atof((char*)message.payload);
  check_temperature(temp);
}

WifiIPStack ipstack;
MQTT::Client<WifiIPStack, Countdown> client = MQTT::Client<WifiIPStack, Countdown>(ipstack);

const char* topic_sub = "bishesh-publish";
const char* topic_pub = "Bishesh-publish";

void connect()
{
  char hostname[] = "mqtt.eclipseprojects.io";
  int port = 1883;
  sprintf(printbuf, "Connecting to %s:%d\n", hostname, port);
  Serial.print(printbuf);
  int rc = ipstack.connect(hostname, port);
  if (rc != 1)
  {
    sprintf(printbuf, "rc from TCP connect is %d\n", rc);
    Serial.print(printbuf);
  }

  Serial.println("MQTT connecting");
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
  data.MQTTVersion = 3;
  data.clientID.cstring = (char*)"energia-sample";
  rc = client.connect(data);
  if (rc != 0)
  {
    sprintf(printbuf, "rc from MQTT connect is %d\n", rc);
    Serial.print(printbuf);
  }
  Serial.println("MQTT connected");

  rc = client.subscribe(topic_sub, MQTT::QOS1, messageArrived);   
  if (rc != 0)
  {
    sprintf(printbuf, "rc from MQTT subscribe is %d\n", rc);
    Serial.print(printbuf);
  }
  Serial.println("MQTT subscribed");
}

void setup()
{
  pinMode(LED, OUTPUT);     
  Serial.begin(115200);
  Serial.print("Attempting to connect to Network named: ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an IP address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  Serial.println(WiFi.localIP());
  
  Serial.println("MQTT Hello example");
  connect();
}

void loop()
{
  if (!client.isConnected())
    connect();
  
  MQTT::Message message;
  char buf[100];

 
 while (arrivedcount == 0) {
   client.yield(1000);
 }
 arrivedcount = 0;
 delay(1000);
}
