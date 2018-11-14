#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define WIFI_SSID            "ssid"
#define WIFI_PASS            "pass"
#define MQTT_HOST            "127.0.0.1"
#define MQTT_TOPIC           "topic"
#define MQTT_CLIENTID        "nodeXXXX"
#define MQTT_USERNAME        "user"
#define MQTT_PASSWPRD        "pass"
#define SENSOR_PIN           A0
#define TEMPERATURE_OFFSET   0
int     prevTemperature      = 0;
long    lastMsg              = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWPRD)) {
      Serial.println("connected");
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
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_HOST, 1883); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now  = millis();
  char msg[50];
  int average = 0;
  char temperature[3];
  if (now - lastMsg > 2000) {
    lastMsg = now;

    for (int i=1; i <= 10; i++){
      int analogValue = analogRead(SENSOR_PIN);
      int temp = (3.3 * analogValue * 100) / 1024;
      average = average + temp;
    }
    average = (average/10) + TEMPERATURE_OFFSET;
    
    itoa(average, temperature, 10);
    snprintf (msg, 50, temperature, 10);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(MQTT_TOPIC, msg);
  }
}
