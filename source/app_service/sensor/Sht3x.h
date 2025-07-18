////////////////////////////////////////////////////////////////////////////////
//  S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2023, Sensirion AG
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS”
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

/// @file Sht3x.h
///
/// Header file to all functions to read the SHT3x Sensor data and process it

#ifndef SHT3X_H
#define SHT3X_H

#include "assert.h"
#include "utility/StaticCodeAnalysisHelper.h"
#include "utility/scheduler/MessageBroker.h"

/// These are the ids of the message category
/// MESSAGE_BROKER_CATEGORY_SENSOR_VALUE
typedef enum {
  SHT3X_MESSAGE_ID_REQUEST_SENT,  /// The request completed successfully
  SHT3X_MESSAGE_ID_SENSOR_READY,  /// The wait time for the request has elapsed
  SHT3X_MESSAGE_ID_SENSOR_DATA,   /// The message contains the read sensor data
  SHT3X_MESSAGE_ID_ERROR          /// Something went wrong
} Sht3x_MessageId_t;

/// Defines the commands that can be handled by the SHT3x sensor
typedef enum {
  SHT3X_COMMAND_LOW_REPEATABILITY_MEASUREMENT,
  SHT3X_COMMAND_HIGH_REPEATABILITY_MEASUREMENT
} Sht3x_Commands_t;

/// This is the data that is received in response to a
/// communication with the sensor
typedef struct _tSht3x_SensorMessage {
  MessageBroker_MsgHead_t head;  ///< Message head:
                                 ///< Id will be set to a value
                                 ///< of @ref Sht3x_MessageId_t.
                                 ///< The category will be set to SENSOR_VALUE
                                 ///< The param1 will contain the sent command
  union {
    struct {
      uint16_t temperatureTicks;  ///< measured temperature
      uint16_t humidityTicks;     ///< measured humidity
      uint16_t co2Value;          ///< measured CO2 value
    } measurement;                ///< Sensor measurement values
    uint16_t errorCode;           ///< number of the error if error occurred
  } data;  ///< The data needs to be interpreted depending on the message id in
           ///< head.id.
           ///< - id == SHT3X_MESSAGE_ID_REQUEST_SENT: data is invalid
           ///< - id == SHT3X_SENSOR_READY: data is invalid
           ///< - id == SHT3X_MESSAGE_ID_SENSOR_DATA: depending on param1 the
           ///<      message contains the measurement data
} Sht3x_SensorMessage_t;

ASSERT_SIZE_TYPE1_LESS_THAN_TYPE2(Sht3x_SensorMessage_t, uint64_t);

/// Initialize the SHT3x Sensor
///
/// @param broker The message broker that where all messages are published
void Sht3x_Init(MessageBroker_Broker_t* broker);

/// Trigger a sensor request
/// @param command id of the request to be triggered
void Sht3x_StartRequest(Sht3x_Commands_t command);

/// The sensor shall publish a message when the
/// current executing command has finished.
///
void Sht3x_NotifySensorReady();

/// Get the previously requested data
void Sht3x_ReadRequestData();

/// Convert ticks from the SHT to temperature in [°C]
///
/// @param ticks  Temperature value in ticks
/// @return Temperature measured by the SHT in [°C]
float Sht3x_TicksToTemperatureCelsius(uint16_t ticks);

/// Convert ticks from the SHT to temperature in [°F]
///
/// @param ticks  Temperature value in ticks
/// @return Temperature measured by the SHT in [°F]
float Sht3x_TicksToTemperatureFahrenheit(uint16_t ticks);

/// Convert ticks from the SHT to relative humidity in [%rH]
///
/// @param ticks  Relative humidity value in ticks
/// @return Humidity measured by the SHT in [%rH]
float Sht3x_TicksToHumidity(uint16_t ticks);

/// Calculate the dew point from temperature and relative humidity
/// @param temperatureC Temperature in celsius
/// @param humidityRh Relative humidity in %
/// @return computed dew point
float Sht3x_DewPointC(float temperatureC, float humidityRh);

#endif  // SHT3X_H
