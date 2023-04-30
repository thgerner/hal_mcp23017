/*
 * mcp23017reg.h
 *
 *  Created on: 16.02.2014
 *      Author: thomas
 */

#ifndef MCP23017REG_H_
#define MCP23017REG_H_

typedef unsigned char byte;

/*
 * MCP23017 register address defines for IOCON.BANK = 0
 */
#define IODIRA      0x00
#define IODIRB      0x01
#define IPOLA       0x02
#define IPOLB       0x03
#define GPINTENA    0x04
#define GPINTENB    0x05
#define DEFVALA     0x06
#define DEFVALB     0x07
#define INTCONA     0x08
#define INTCONB     0x09
#define IOCON       0x0A
#define GPPUA       0x0C
#define GPPUB       0x0D
#define INTFA       0x0E
#define INTFB       0x0F
#define INTCAPA     0x10
#define INTCAPB     0x11
#define GPIOA       0x12
#define GPIOB       0x13
#define OLATA       0x14
#define OLATB       0x15

union iodirBits {
  struct {
    unsigned IO0        : 1;
    unsigned IO1        : 1;
    unsigned IO2        : 1;
    unsigned IO3        : 1;
    unsigned IO4        : 1;
    unsigned IO5        : 1;
    unsigned IO6        : 1;
    unsigned IO7        : 1;
  };
  byte value;
  iodirBits() : value(0xff) {};
};
#define IODIR_IN        1
#define IODIR_OUT       0

union ipolBits {
  struct {
    unsigned IP0        : 1;
    unsigned IP1        : 1;
    unsigned IP2        : 1;
    unsigned IP3        : 1;
    unsigned IP4        : 1;
    unsigned IP5        : 1;
    unsigned IP6        : 1;
    unsigned IP7        : 1;
  };
  byte value;
  ipolBits() : value(0) {};
};

union gpintenBits {
  struct {
    unsigned GPINT0     : 1;
    unsigned GPINT1     : 1;
    unsigned GPINT2     : 1;
    unsigned GPINT3     : 1;
    unsigned GPINT4     : 1;
    unsigned GPINT5     : 1;
    unsigned GPINT6     : 1;
    unsigned GPINT7     : 1;
  };
  byte value;
  gpintenBits() : value(0) {};
};

union defvalBits {
  struct {
    unsigned DEV0       : 1;
    unsigned DEV1       : 1;
    unsigned DEV2       : 1;
    unsigned DEV3       : 1;
    unsigned DEV4       : 1;
    unsigned DEV5       : 1;
    unsigned DEV6       : 1;
    unsigned DEV7       : 1;
  };
  byte value;
  defvalBits() : value(0) {};
};

union intconBits {
  struct {
    unsigned IOC0       : 1;
    unsigned IOC1       : 1;
    unsigned IOC2       : 1;
    unsigned IOC3       : 1;
    unsigned IOC4       : 1;
    unsigned IOC5       : 1;
    unsigned IOC6       : 1;
    unsigned IOC7       : 1;
  };
  byte value;
  intconBits() : value(0) {};
};

union ioconBits {
  struct {
    unsigned            : 1;
    unsigned INTPOL     : 1;
    unsigned ODR        : 1;
    unsigned HAEN       : 1;
    unsigned DISSLW     : 1;
    unsigned SEQOP      : 1;
    unsigned MIRROR     : 1;
    unsigned BANK       : 1;
  };
  byte value;
  ioconBits() : value(0) {};
};
#define IOCON_INTPOL_BIT		1
#define IOCON_ODR_BIT				2
#define IOCON_HAEN_BIT			3
#define IOCON_DISSLW_BIT		4
#define IOCON_SEQOP_BIT			5
#define IOCON_MIRROR_BIT		6
#define IOCON_BANK_BIT			7

union gppuBits {
  struct {
    unsigned PU0        : 1;
    unsigned PU1        : 1;
    unsigned PU2        : 1;
    unsigned PU3        : 1;
    unsigned PU4        : 1;
    unsigned PU5        : 1;
    unsigned PU6        : 1;
    unsigned PU7        : 1;
  };
  byte value;
  gppuBits() : value(0) {};
};

union intfBits {
  struct {
    unsigned INT0       : 1;
    unsigned INT1       : 1;
    unsigned INT2       : 1;
    unsigned INT3       : 1;
    unsigned INT4       : 1;
    unsigned INT5       : 1;
    unsigned INT6       : 1;
    unsigned INT7       : 1;
  };
  byte value;
  intfBits() : value(0) {};
};

union intcapBits {
  struct {
    unsigned ICP0       : 1;
    unsigned ICP1       : 1;
    unsigned ICP2       : 1;
    unsigned ICP3       : 1;
    unsigned ICP4       : 1;
    unsigned ICP5       : 1;
    unsigned ICP6       : 1;
    unsigned ICP7       : 1;
  };
  byte value;
  intcapBits() : value(0) {};
};

union gpioBits {
  struct {
    unsigned GP0        : 1;
    unsigned GP1        : 1;
    unsigned GP2        : 1;
    unsigned GP3        : 1;
    unsigned GP4        : 1;
    unsigned GP5        : 1;
    unsigned GP6        : 1;
    unsigned GP7        : 1;
  };
  byte value;
  gpioBits() : value(0) {};
};

union olatBits {
  struct {
    unsigned OL0        : 1;
    unsigned OL1        : 1;
    unsigned OL2        : 1;
    unsigned OL3        : 1;
    unsigned OL4        : 1;
    unsigned OL5        : 1;
    unsigned OL6        : 1;
    unsigned OL7        : 1;
  };
  byte value;
  olatBits() : value(0) {};
};

#endif /* MCP23017REG_H_ */
