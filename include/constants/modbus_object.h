#pragma once

#include "models/modbus_object_entity.h"

// 1) WSD304 — wind + (table also shows rainfall value)
static const ModbusObjectProtocol WSD304_PROTOCOLS[] = {
    {0x03, 0, 1, unsigned_integer}, // wind speed (m/s)
    {0x03, 1, 1, unsigned_integer}, // wind speed (category)
    {0x03, 2, 1, unsigned_integer}, // wind speed (km/h)
    {0x03, 3, 1, unsigned_integer}, // wind direction (deg/10)
    {0x03, 4, 1, unsigned_integer}, // wind direction (category)
};

// 2) SEM226O — illuminance (+ config registers)
static const ModbusObjectProtocol SEM226O_PROTOCOLS[] = {
    {0x03, 0, 1, unsigned_integer}, // rainfall (mm)
    {0x03, 2, 2, unsigned_integer}, // luminance (Lux)
    {0x03, 6, 1, unsigned_integer}, // luminance (Lux*100)
};

// 3) PZEM-017 — power meter readings
// static const ModbusObjectProtocol PZEM017_PROTOCOLS[] = {
//     {0x03, 0, 2, single_read},       // voltage
//     {0x03, 2, 2, single_read},       // current
//     {0x03, 4, 2, single_read},       // power
//     {0x03, 6, 2, single_read},       // energy
//     {0x03, 7, 1, single_read},       // alarm
// };

// 3) EM619002 — power meter readings
static const ModbusObjectProtocol EM619002_PROTOCOLS[] = {
    {0x03, 0, 2, signed_integer},     // total active energy (kWh/100)
    {0x03, 20, 2, unsigned_integer},  // total positive (charge) energy (kWh/100)
    {0x03, 30, 2, unsigned_integer},  // total negative (discharge) energy (kWh/100)
    {0x03, 305, 1, unsigned_integer}, // measured voltage (V/10)
    {0x03, 313, 2, signed_integer},   // measured current (A/1000)
    {0x03, 321, 2, signed_integer},   // measured active power (W/10)
};

// 4) SEM228A — solar radiation + deviation
static const ModbusObjectProtocol SEM228A_PROTOCOLS[] = {
    {0x03, 0, 1, unsigned_integer},  // solar radiation value (W/m2)
    {0x03, 82, 1, unsigned_integer}, // deviation (W/m2)
};

// 5) SEM227 — air temperature & humidity
static const ModbusObjectProtocol SEM227_PROTOCOLS[] = {
    {0x03, 0, 1, unsigned_integer}, // humidity    (rh%/10)
    {0x03, 1, 1, signed_integer},   // temperature (degC/10)
};

// Entities (pointer to array + count). Keep names from the table for clarity.

// static const ModbusObjectEntity powerMeterSensor(
//     0x01,
//     "PZEM-017",
//     PZEM017_PROTOCOLS,
//     sizeof(PZEM017_PROTOCOLS) / sizeof(PZEM017_PROTOCOLS[0]));

static const ModbusObjectEntity powerMeterSensor(
    0x02,
    "EM619002",
    EM619002_PROTOCOLS,
    sizeof(EM619002_PROTOCOLS) / sizeof(EM619002_PROTOCOLS[0]));

static const ModbusObjectEntity illuminanceSensor(
    0x03,
    "SEM226O",
    SEM226O_PROTOCOLS,
    sizeof(SEM226O_PROTOCOLS) / sizeof(SEM226O_PROTOCOLS[0]));

static const ModbusObjectEntity pyranoMeterSensor(
    0x04,
    "SEM228A",
    SEM228A_PROTOCOLS,
    sizeof(SEM228A_PROTOCOLS) / sizeof(SEM228A_PROTOCOLS[0]));

static const ModbusObjectEntity windConditionSensor(
    0x05,
    "WSD304",
    WSD304_PROTOCOLS,
    sizeof(WSD304_PROTOCOLS) / sizeof(WSD304_PROTOCOLS[0]));

static const ModbusObjectEntity airQualitySensor(
    0x06,
    "SEM227",
    SEM227_PROTOCOLS,
    sizeof(SEM227_PROTOCOLS) / sizeof(SEM227_PROTOCOLS[0]));