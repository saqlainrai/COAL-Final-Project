
// for esp32 code

#include <WiFi.h>
#include <PubSubClient.h>
#include <ThingSpeak.h>

// Details of WIFI Connection
const char *ssid = "Saqlain";
const char *password = "11223344";

// Details of ThingSpeak Channel
long myChannelNumber = 2390848;
const char *myWriteAPIKey = "QYFPMA31BRT11Z26";

// Details of MQTT Broker and topics
const char *mqttServer = "test.mosquitto.org";
const int mqttPort = 1883;
const char *mqttClientId = "2022CS80/device";
const char *inTopic = "2022-CS-80/value";      // threshold value will be taken here
const char *outTopic = "2022-CS-80/intensity"; // intensity of current light will be published here

// Random Variables
long currentTime, lastTime;
int count = 0;
char messages[50];

WiFiClient espClient;
PubSubClient client(espClient);

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
void setup()
{
    Serial.begin(115200);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    ThingSpeak.begin(espClient);

    // Setup MQTT Server
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    client.subscribe(inTopic);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    // Check if data is available on Serial
    if (Serial.available() > 0)
    {
        // Read the LDR value from Serial
        // After a value is read from Serial
        // 1. Display it on Serial Monitor
        // 2. Save it in ThingSpeak Database
        // 3. Publish it on MQTT broker

        int ldrValue = Serial.parseInt();

        // Print the received value on Serial Monitor
        Serial.println("Received LDR Value: " + String(ldrValue));

        // Send data to ThingSpeak
        ThingSpeak.writeField(myChannelNumber, 1, ldrValue, myWriteAPIKey);

        // Publish the Value on the MQTT Broker
        snprintf(messages, 10, "%ld", ldrValue);
        Serial.println(messages);
        client.publish(outTopic, messages);

        // >>>>>>> This is logic to print integer in continuous loop
        // currentTime = millis();
        // if (currentTime - lastTime > 2000)
        // {
        //     count++;
        //     snprintf(messages, 75, "Count: %ld", count);
        //     Serial.print("Sending messages: ");
        //     Serial.println(count);
        //     // client.publish(outTopic, messages);
        //     lastTime = millis();
        // }

        Serial.flush();
        delay(1000); // Delay before sending the next data
    }
}
