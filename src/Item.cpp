#include "Item.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>

#define NODEBUG_PRINT

Item::Item(uint16_t itemId)
{
    this->itemId = itemId;
}

Sensor::Sensor(uint16_t itemId) : Item(itemId)
{
    // empty for now
}

// fill in sensor sub-address and sensor type
void Sensor::initPacket(RFSensorPacket &packet)
{
    packet.srcAdr &= 0xFFFF0000;
    packet.srcAdr |= this->itemId;
    packet.sensorType = this->itemType;
}

void Sensor::announce(RFSensorPacket &packet)
{
    initPacket(packet);
    packet.pktType = RFPacketType::ANNOUNCE;
}

SensorDHTTemp::SensorDHTTemp(uint16_t itemId, DHT_Unified *dht) : Sensor(itemId)
{
    this->itemType = RFSensorType::TEMPERATURE;
    this->dht = dht;
    reqInitTime = 1; // requires at least 2s to be powered before reading
}

void SensorDHTTemp::read(RFSensorPacket &packet)
{
    initPacket(packet);
    sensors_event_t event;
    String v = "NaN";
    Serial.print("  Temp: ");
    dht->temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
#ifndef NODEBUG_PRINT
        Serial.println(F("Error reading temperature!"));
#endif
    }
    else
    {
#ifndef NODEBUG_PRINT
        Serial.print(F("  0x"));
        Serial.print(getId(), HEX);
        Serial.print(F(" Temperature: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
#endif
        v = String(event.temperature);
    }
    Serial.println(v);
    strncpy((char *)packet.payload, v.c_str(), sizeof(packet.payload));
}

SensorDHTHumidity::SensorDHTHumidity(uint16_t itemId, DHT_Unified *dht) : Sensor(itemId)
{
    this->itemType = RFSensorType::HUMIDITY;
    this->dht = dht;
    reqInitTime = 1; // requires at least 2s to be powered before reading
}

void SensorDHTHumidity::read(RFSensorPacket &packet)
{
    initPacket(packet);
    sensors_event_t event;
    String v = "NaN";
    // Get humidity event and print its value.
    Serial.print("  Humidity: ");
    dht->humidity().getEvent(&event);
    if (isnan(event.relative_humidity))
    {
        Serial.println(F("Error reading humidity!"));
    }
    else
    {
#ifndef NODEBUG_PRINT
        Serial.print(F("  0x"));
        Serial.print(getId(), HEX);
        Serial.print(F(" Humidity: "));
        Serial.print(event.relative_humidity);
        Serial.println(F("%"));
#endif
        v = String(event.relative_humidity);
    }
    Serial.println(v);

    strncpy((char *)packet.payload, v.c_str(), sizeof(packet.payload));
}

SensorContact::SensorContact(uint16_t itemId, uint8_t pin) : Sensor(itemId)
{
    this->itemType = RFSensorType::CONTACT;
    this->pin = pin;
    pinMode(pin, INPUT_PULLUP);
}

void SensorContact::read(RFSensorPacket &packet)
{
    initPacket(packet);
    String v = String(!digitalRead(pin));
    Serial.print("  Contact: ");
#ifndef NODEBUG_PRINT
    Serial.print(F("  0x"));
    Serial.print(getId(), HEX);
    Serial.print(F(" Contact: "));
    Serial.println(v);
#endif

    Serial.println(v);
    strcpy((char *)packet.payload, v.c_str());
}
