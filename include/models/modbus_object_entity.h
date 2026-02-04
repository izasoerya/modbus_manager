#pragma once
#include <Arduino.h>

typedef enum
{
    signed_integer,
    unsigned_integer
} datatype_t;

struct ModbusObjectProtocol
{
    uint8_t functionCode;
    uint16_t startAddress;
    size_t lengthAddress;
    datatype_t sensorDataType;

    ModbusObjectProtocol(
        uint8_t functionCode,
        uint16_t startAddress,
        size_t lengthAddress,
        datatype_t sensorDataType)
        : functionCode(functionCode),
          startAddress(startAddress),
          lengthAddress(lengthAddress),
          sensorDataType(sensorDataType) {}
};

class ModbusObjectEntity
{
private:
    const uint8_t _slaveId;
    const char *_nameSensor;
    const ModbusObjectProtocol *_protocols;
    const size_t _protocolCount;

public:
    ModbusObjectEntity(
        uint8_t slaveId,
        const char *nameSensor,
        const ModbusObjectProtocol *protocols,
        size_t protocolCount)
        : _slaveId(slaveId),
          _nameSensor(nameSensor),
          _protocols(protocols),
          _protocolCount(protocolCount) {}

    ~ModbusObjectEntity() = default;

    inline uint8_t getSlaveId() const { return _slaveId; }
    inline const char *getNameSensor() const { return _nameSensor; }
    inline const ModbusObjectProtocol *getProtocols() const { return _protocols; }
    inline size_t getProtocolCount() const { return _protocolCount; }
};
