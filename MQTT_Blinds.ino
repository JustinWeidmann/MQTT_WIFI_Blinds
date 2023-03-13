#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

// Update these with values suitable for your network.

const char* ssid = "Dunder Mufflin INC";
const char* password = "Eighty8680fancy";
const char* mqtt_server = "192.168.1.11";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

const int led = 2;
Servo servo;
bool blindsOpen;
int bootToken = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

////////////////////Logic\\\\\\\\\\\\\\\\\\\\

  if ((char)payload[0] == '1'){ //Open
    digitalWrite(led, LOW); //LED On
    Serial.println("OPEN has been touched!!");
    
    //Blinds open
    if((blindsOpen == false) || (bootToken == 1)){
      //Open blinds
      servo.write(130); //Full close = 142, Safe close = 130
      blindsOpen = true;
      bootToken = 0;
    }
    else{
      blindsInState();
    }
    
  }else if((char)payload[0] == '0'){  //Close
    digitalWrite(led, HIGH);  //LED off
    Serial.println("CLOSE has been touched!!");
    
    //Blinds close
    if((blindsOpen == true) || (bootToken == 1)){
      //Close blinds
      servo.write(0);
      blindsOpen = false;
      bootToken = 0;
    }
    else{
      blindsInState();
    }
    
  }else if((char)payload[0] == '2'){
    Serial.println("MID has been touched!!");
    
      servo.write(45);
  }else{  //Stop
    Serial.println("ERR has been touched!!");
    digitalWrite(led,!digitalRead(led));  //LED blink
    delay(700);
    digitalWrite(led,!digitalRead(led));
    delay(700);
    digitalWrite(led,!digitalRead(led));
    delay(700);
    digitalWrite(led,!digitalRead(led));
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      
      client.publish("JustinsRoom/Blinds/$name", "Auto Blinds B.1"); //Project title
      
      //client.subscribe("JustinsRoom/Blinds");
      client.subscribe("JustinsRoom/Blinds/$state");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(led, OUTPUT);
  bootToken = 1;
  servo.attach(D1); //Servo pin
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
}

void blindsInState(){
  digitalWrite(led,!digitalRead(led));
  delay(50);
  digitalWrite(led,!digitalRead(led));
  delay(50);
  digitalWrite(led,!digitalRead(led));
  delay(50);
  digitalWrite(led,!digitalRead(led));
}
