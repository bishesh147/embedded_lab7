#define MQTTCLIENT_QOS2 1

#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WifiIPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>

// your network name also called SSID
char ssid[] = "CTNLaptop";
// your network password
char password[] = "7296w:1O";

char printbuf[100];

int arrivedcount = 0;

void messageArrived(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;
  
  sprintf(printbuf, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n", 
    ++arrivedcount, message.qos, message.retained, message.dup, message.id);
  Serial.print(printbuf);
  sprintf(printbuf, "Payload %s\n", (char*)message.payload);
  Serial.print(printbuf);
}

WifiIPStack ipstack;
MQTT::Client<WifiIPStack, Countdown> client = MQTT::Client<WifiIPStack, Countdown>(ipstack);

const char* topic_sub = "energia-sample";
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

float tmp116_get_temp(void)
{
  int16_t register_val;
  register_val = read_register();

  float  temp = (float)register_val * 0.0078125;
  return temp;
}

float read_register(void)
{
  Wire.beginTransmission(0x49);                  
  Wire.write(0x00);                              
  Wire.endTransmission();
  delay(100);
  
  Wire.requestFrom(0x49, 2);                     
  int16_t iData;
  while (Wire.available()) 
  { 
    iData = Wire.read();
    iData = iData<<8;
    iData |= Wire.read();
  }
//  float v_obj = iData * 156.25;
  return iData;
}


void setup()
{
  Serial.begin(115200);
  Wire.begin();
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
  float tmp_116;
  String tmp_116_str;

  if (!client.isConnected())
    connect();
  
  MQTT::Message message;
  char buf[100];

  tmp_116 = tmp116_get_temp();
  // Send and receive QoS 0 message
  int temp_int = (int)tmp_116;
  int temp_frac = (int)((tmp_116 - temp_int) * 100);  // 2 decimal places

  sprintf(buf, "Temp: %d.%02d C", temp_int, temp_frac);
  // sprintf(buf, "Hello World from Bishesh! QoS 0 message");
  Serial.println(buf);
  message.qos = MQTT::QOS0;
  message.retained = false;
  message.dup = false;
  message.payload = (void*)buf;
  message.payloadlen = strlen(buf)+1;
  int rc = client.publish(topic_pub, message);
  
//  while (arrivedcount == 0) {
//    client.yield(1000);
//  }
//  arrivedcount = 0;

  delay(2000);
}
