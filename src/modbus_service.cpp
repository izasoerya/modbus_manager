#include "services/modbus_service.h"

ModbusService::ModbusService(const ModbusObjectEntity *entity) : _entity(entity) {}
ModbusService::~ModbusService() {}

void ModbusService::begin()
{
    pinMode(_dePin, OUTPUT);
    digitalWrite(_dePin, LOW);

    _node.preTransmission(_preTransmission);
    _node.postTransmission(_postTransmission);
    _node.begin(_entity->getSlaveId(), Serial2);
}

int64_t ModbusService::readSingle(uint8_t count)
{
    const ModbusObjectProtocol &p = _entity->getProtocols()[count];

    uint8_t res = 0;
    if (p.functionCode == 0x03)
    {
        res = _node.readHoldingRegisters(p.startAddress, p.lengthAddress);
    }
    else if (p.functionCode == 0x04)
    {
        res = _node.readInputRegisters(p.startAddress, p.lengthAddress);
    }
    // Check communication success
    if (res != _node.ku8MBSuccess)
        return 0; // or handle error differently

    // Combine multiple 16-bit registers into 32-bit
    if (p.sensorDataType == signed_integer)
    {
        int32_t readingValue = 0;
        for (uint8_t i = 0; i < p.lengthAddress; i++)
        {
            readingValue = (readingValue << 16) | _node.getResponseBuffer(i);
        }
        return readingValue;
    }

    if (p.sensorDataType == unsigned_integer)
    {
        uint32_t readingValue = 0;
        for (uint8_t i = 0; i < p.lengthAddress; i++)
        {
            readingValue = (readingValue << 16) | _node.getResponseBuffer(i);
        }
        return readingValue;
    }

    return 0;
}

uint16_t *ModbusService::readAll(uint8_t length)
{
    const ModbusObjectProtocol *protos = _entity->getProtocols();
    const size_t count = _entity->getProtocolCount();

    size_t totalRegs = 0;
    for (size_t i = 0; i < count; ++i)
        totalRegs += protos[i].lengthAddress;

    if (length != 0 && length < totalRegs)
        totalRegs = length;

    uint16_t *out = new uint16_t[totalRegs]; // caller must delete[]
    size_t outIdx = 0;

    for (size_t i = 0; i < count && outIdx < totalRegs; ++i)
    {
        const auto &p = protos[i];
        uint8_t res = 0;
        if (p.functionCode == 0x03)
        {
            res = _node.readHoldingRegisters(p.startAddress, p.lengthAddress);
        }
        else if (p.functionCode == 0x04)
        {
            res = _node.readInputRegisters(p.startAddress, p.lengthAddress);
        }
        else
        {
            for (size_t j = 0; j < p.lengthAddress && outIdx < totalRegs; ++j)
                out[outIdx++] = 0;
            continue;
        }

        if (res == _node.ku8MBSuccess)
        {
            for (size_t j = 0; j < p.lengthAddress && outIdx < totalRegs; ++j)
            {
                out[outIdx++] = _node.getResponseBuffer(j);
            }
        }
        else
        {
            for (size_t j = 0; j < p.lengthAddress && outIdx < totalRegs; ++j)
                out[outIdx++] = 0;
        }
    }
    return out;
}