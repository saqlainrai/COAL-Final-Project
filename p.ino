
#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "SaqlainPC";
const char *password = "11223344";

const char *mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
const char *mqttClientId = "2022CS80/device";
const char *inTopic = "2022-CS-80/value";
const char *outTopic = "2022-CS-80/intensity";

unsigned long lastMsgTime = 0;
long currentTime, lastTime;
int count = 0;
char messages[50];

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");

        if (client.connect(mqttClientId))
        {
            Serial.println("connected");
            // Once connected, you can subscribe or publish to topics
            client.subscribe(inTopic);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    // This function will be called when a message is received on the subscribed topic
    Serial.print("Message received on topic: ");
    Serial.println(inTopic);

    Serial.print("Payload: ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Add your logic to process the received message here
}

void setup_wifi()
{
    delay(100);
    Serial.print("\nConnecting to: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        // Serial.print(ssid);
        delay(1000); // Increase delay to 1 second
        Serial.println(WiFi.status());
        Serial.print("- ");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("\nConnected to: ");
        Serial.println(ssid);
    }
    else
    {
        Serial.println("\nConnection failed. Check your credentials or try again.");
    }
}

void setup_mqtt()
{
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}

void setup()
{
    Serial.begin(115200);
    setup_wifi();
    setup_mqtt();
    client.subscribe(inTopic);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    currentTime = millis();
    if (currentTime - lastTime > 2000)
    {
        count++;
        snprintf(messages, 75, "Count: %ld", count);
        Serial.print("Sending messages: ");
        Serial.println(count);
        // client.publish(outTopic, messages);
        lastTime = millis();
    }
}
