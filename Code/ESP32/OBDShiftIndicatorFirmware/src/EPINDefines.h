#ifndef PINDEFINES_H
#define PINDEFINES_H

namespace ePIN
{
typedef enum
{
  CAN_RX = 34,
  CAN_TX = 27,
  CAN_EN = 25,
  CAN_NSTB = 26,
  I2C_SDA = 21,
  I2C_SCL = 22,
  DIGITAL_LEDS = 32,
  GPIO2 = 13,
  GPIO1 = 14,
  GPIO0 = 33,
  VSPI_CS = 5,
  VSPI_CLK = 18,
  VSPI_MISO = 19,
  VSPI_MOSI = 23,
  STATUS_LED = 17
} ePIN;
}

#endif
