#pragma once

#include <ModbusMaster.h>
#include "models/modbus_object_entity.h"

class ModbusService
{
protected:
private:
    ModbusMaster _node;
    static const uint8_t _dePin = 5;
    const ModbusObjectEntity *_entity;

    static void _preTransmission()
    {
        digitalWrite(_dePin, HIGH);
    }

    static void _postTransmission()
    {
        digitalWrite(_dePin, LOW);
    }

public:
    ModbusService(const ModbusObjectEntity *entity);
    ~ModbusService();

    void begin();
    int64_t readSingle(uint8_t count);
    uint16_t *readAll(uint8_t length);
    uint8_t getLengthObject() { return sizeof(_entity) / sizeof(ModbusObjectEntity); }
    void healthCheck();
};