
const int ldr_pin = A0;
const int led_pin = 13;

int threshold = 200;
int intensity = 0;

void setup()
{
    // put your setup code here, to run once:
    pinMode(ldr_pin, INPUT);
    pinMode(led_pin, OUTPUT);
    Serial.begin(115200);
}

void loop()
{
    // put your main code here, to run repeatedly:
    if (analogRead(ldr_pin) > threshold)
    {
        digitalWrite(led_pin, HIGH);
    }
    else
    {
        digitalWrite(led_pin, LOW);
    }
    intensity = analogRead(ldr_pin);
    Serial.println(intensity);
    delay(1000);

    if (Serial.available() > 0)
    {
        int ldrValue = Serial.parseInt();
        while (Serial.available() > 0)
        {
            Serial.read();
        }
        // Print the received value on Serial Monitor
        // Serial.println("Received Threshold Value: " + String(ldrValue));
        threshold = ldrValue;
        Serial.print("Threshold Value is Updated to ");
        Serial.println(threshold);
    }
}
