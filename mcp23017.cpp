/*
 * Mcp23017.cpp
 *
 *  Created on: 16.02.2014
 *      Author: thomas
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
extern "C" {
	#include <linux/i2c-dev.h>
	#include <i2c/smbus.h>
}

#include <string>

#include "mcp23017reg.h"
#include "mcp23017.h"


Mcp23017::Mcp23017(int i2cdev, int address)
: handle(-1)
{
  char devName[32];
  sprintf(devName, "/dev/i2c-%i", i2cdev);

  handle = open(devName, O_RDWR | O_NONBLOCK);
  if (handle < 0)
  {
    int errnum = errno;
    char msg[128];
    sprintf(msg, "Can't open device %s: error %i", devName, errnum);
    throw new IOException(msg);
  }

  if (ioctl(handle, I2C_SLAVE, address))
  {
    int errnum = errno;
    close(handle);
    char msg[128];
    sprintf(msg, "Can't set device address %i to %s: error %i", address, devName, errnum);
    throw new IOException(msg);
  }

  ioconBits iocon;
  iocon.BANK = 0; // this is the default
  iocon.SEQOP = 0;

  if (i2c_smbus_write_byte_data(handle, IOCON, iocon.value)) {
    int errnum = errno;
    close(handle);
    char msg[128];
    sprintf(msg, "Can't configure device %s, set IOCON to %i failed: error %i", devName, iocon.value, errnum);
    throw new IOException(msg);
  }
}

Mcp23017::~Mcp23017()
{
  if (handle >= 0) {
    close(handle);
  }
}

void Mcp23017::setIoDir(IoDir dir)
{
  write2byte(IODIRA, dir.a.value, dir.b.value);
}

Mcp23017::IoDir Mcp23017::getIoDir()
{
	int read = read2byte(IODIRA);
	IoDir ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

void Mcp23017::setIoPolarity(IoPol pol)
{
  write2byte(IPOLA, pol.a.value, pol.b.value);
}

Mcp23017::IoPol Mcp23017::getIoPolarity()
{
	int read = read2byte(IPOLA);
	IoPol ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

void Mcp23017::setIntOnChangeControl(InterruptOnChangeEnable intEnable)
{
  write2byte(GPINTENA, intEnable.a.value, intEnable.b.value);
}

Mcp23017::InterruptOnChangeEnable Mcp23017::getIntOnChangeControl()
{
	int read = read2byte(GPINTENA);
	InterruptOnChangeEnable ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

void Mcp23017::setDefaultCompare(DefaultCompare compare)
{
  write2byte(DEFVALA, compare.a.value, compare.b.value);
}

Mcp23017::DefaultCompare Mcp23017::getDefaultCompare()
{
	int read = read2byte(DEFVALA);
	DefaultCompare ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

void Mcp23017::setInterruptControl(InterruptControl ctrl)
{
  write2byte(INTCONA, ctrl.a.value, ctrl.b.value);
}

Mcp23017::InterruptControl Mcp23017::getInterruptControl()
{
	int read = read2byte(INTCONA);
	InterruptControl ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

void Mcp23017::setIoConfiguration(IoControl ioBits)
{
	// make sure BANK and SEQOP are cleared to make sure reading of two registers at once works
	ioBits.iocon.BANK = 0;
	ioBits.iocon.SEQOP = 0;
  if (i2c_smbus_write_byte_data(handle, IOCON, ioBits.iocon.value)) {
    int errnum = errno;
    close(handle);
    char msg[128];
    sprintf(msg, "Failed writing command %d, iocon %i: error %i", IOCON, ioBits.iocon.value, errnum);
    throw new IOException(msg);
  }
}

Mcp23017::IoControl Mcp23017::getIoConfiguration()
{
  int read = i2c_smbus_read_byte_data(handle, IOCON);
  if (read == -1) {
    int errnum = errno;
    char msg[128];
    sprintf(msg, "Error reading command %i, error %i", IOCON, errnum);
    throw new IOException(msg);
  }
  IoControl ret;
  ret.iocon.value = read & 0xff;
  return ret;
}

void Mcp23017::setPullUpConfiguration(PullUpConfiguration pullUp)
{
  write2byte(GPPUA, pullUp.a.value, pullUp.b.value);
}

Mcp23017::PullUpConfiguration Mcp23017::getPullUpConfiguration()
{
  int read = read2byte(GPPUA);
  PullUpConfiguration ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

Mcp23017::InterruptFlag Mcp23017::getInterruptFlags()
{
  int read = read2byte(INTFA);
  InterruptFlag ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

Mcp23017::InterruptCapture Mcp23017::getInterruptCapture()
{
  int read = read2byte(INTCAPA);
  InterruptCapture ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

void Mcp23017::setOutputLatch(OutputLatch latch)
{
  write2byte(OLATA, latch.a.value, latch.b.value);
}

Mcp23017::OutputLatch Mcp23017::getOutputLatch()
{
  int read = read2byte(OLATA);
  OutputLatch ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

void Mcp23017::setIoPort(IoPort port)
{
  write2byte(GPIOA, port.a.value, port.b.value);
}

Mcp23017::IoPort Mcp23017::getIoPort()
{
  int read = read2byte(GPIOA);
  IoPort ret;
  ret.a.value = read & 0xff;
  ret.b.value = (read >> 8) & 0xff;
  return ret;
}

Mcp23017::InterruptResult Mcp23017::getInterruptResult()
{
	byte data[6];
	int len = i2c_smbus_read_i2c_block_data(handle, INTFA, sizeof(data), &data[0]);
	if (len != sizeof(data) || len < 0) {
    int errnum = errno;
    char msg[128];
    sprintf(msg, "Error reading block interrupt registers, error %i", errnum);
    throw new IOException(msg);
	}
	InterruptResult res;
	res.intfa.value = data[0];
	res.intfb.value = data[1];
	res.capa.value = data[2];
	res.capb.value = data[3];
	res.gpioa.value = data[4];
	res.gpiob.value = data[5];
	return res;
}

void Mcp23017::write2byte(int command, byte a, byte b)
{
  u_int16_t word;
  word = (b << 8) | a;
  if (i2c_smbus_write_word_data(handle, command, word)) {
    int errnum = errno;
    char msg[128];
    sprintf(msg, "Failed writing command %d, a=%i, b=%i: error %i", command, a, b, errnum);
    throw new IOException(msg);
  }
}

int Mcp23017::read2byte(int command)
{
  int read = i2c_smbus_read_word_data(handle, command);
  if (read == -1) {
    int errnum = errno;
    char msg[128];
    sprintf(msg, "Error reading command %i, error %i", command, errnum);
    throw new IOException(msg);
  }

  return read;
}

void Mcp23017::Base::setBit(int bit, byte &a, byte &b)
{
  if (bit < 0 || bit > 15)
    return;
  if (bit < 8) {
    a |= (1 << bit);
  } else {
    bit -= 8;
    b |= (1 << bit);
  }
}

void Mcp23017::Base::clearBit(int bit, byte &a, byte &b)
{
  if (bit < 0 || bit > 15)
    return;
  if (bit < 8) {
    a &= ~(1 << bit);
  } else {
    bit -= 8;
    b &= ~(1 << bit);
  }
}

bool Mcp23017::Base::isSet(int bit, byte a, byte b)
{
  if (bit < 0 || bit > 15)
    return false;
  if (bit < 8) {
    int mask = 1 << bit;
    return (mask & a) == mask;
  }
  bit -= 8;
  int mask = 1 << bit;
  return (mask & b) == mask;
}

void Mcp23017::IoControl::setBit(int bit)
{
	if (bit < 0 || bit > 8)
		return;
	iocon.value |= (1 << bit);
}

void Mcp23017::IoControl::clearBit(int bit)
{
	if (bit < 0 || bit > 8)
		return;
	iocon.value &= ~(1 << bit);
}

bool Mcp23017::IoControl::isSet(int bit)
{
	if (bit < 0 || bit > 8)
		return false;
  int mask = 1 << bit;
  return (mask & iocon.value) == mask;
}

IOException::IOException() :
  msg("")
{
}

IOException::IOException(const char *message) :
    msg(message)
{
}

IOException::~IOException() throw()
{
}

const char* IOException::what() const throw()
{
  return msg.c_str();
}
