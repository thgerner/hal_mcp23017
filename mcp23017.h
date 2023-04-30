/*
 * Mcp23017.h
 *
 *  Created on: 16.02.2014
 *      Author: thomas
 */

#ifndef MCP23017_H_
#define MCP23017_H_

class Mcp23017
{
public:
  Mcp23017(int i2cdev, int address);
  virtual ~Mcp23017();

private:
  struct Base {
    void setBit(int bit, byte &a, byte &b);
    void clearBit(int bit, byte &a, byte &b);
    bool isSet(int bit, byte a, byte b);
  };

public:
  struct IoDir : private Mcp23017::Base {
    iodirBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct IoPol : private Mcp23017::Base {
    ipolBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct InterruptOnChangeEnable : private Mcp23017::Base {
    gpintenBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct DefaultCompare : private Mcp23017::Base {
    defvalBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct InterruptControl : private Mcp23017::Base {
    intconBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct IoControl {
	ioconBits iocon;
	void setBit(int bit);
	void clearBit(int bit);
	bool isSet(int bit);
  };

  struct PullUpConfiguration : private Mcp23017::Base {
    gppuBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct InterruptFlag : private Mcp23017::Base {
    intfBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct InterruptCapture : private Mcp23017::Base {
    intcapBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct IoPort : private Mcp23017::Base {
    gpioBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct OutputLatch : private Mcp23017::Base {
    olatBits a, b;
    void setBit(int bit) { Base::setBit(bit, a.value, b.value); };
    void clearBit(int bit) { Base::clearBit(bit, a.value, b.value); };
    void setAll() { a.value = 0xff; b.value = 0xff; };
    void clearAll() { a.value = 0; b.value = 0; };
    bool isSet(int bit) { return Base::isSet(bit, a.value, b.value); };
  };

  struct InterruptResult {
  	intfBits intfa, intfb;
  	intcapBits capa, capb;
  	gpioBits gpioa, gpiob;
  };

  void setIoDir(IoDir dir);
  IoDir getIoDir();
  void setIoPolarity(IoPol pol);
  IoPol getIoPolarity();
  void setIntOnChangeControl(InterruptOnChangeEnable intEnable);
  InterruptOnChangeEnable getIntOnChangeControl();
  void setDefaultCompare(DefaultCompare compare);
  DefaultCompare getDefaultCompare();
  void setInterruptControl(InterruptControl ctrl);
  InterruptControl getInterruptControl();
  void setIoConfiguration(IoControl ioBits);
  IoControl getIoConfiguration();
  void setPullUpConfiguration(PullUpConfiguration pullUp);
  PullUpConfiguration getPullUpConfiguration();
  InterruptFlag getInterruptFlags();	//read only register
  InterruptCapture getInterruptCapture();	// read only register
  void setIoPort(IoPort port);
  IoPort getIoPort();
  void setOutputLatch(OutputLatch latch);
  OutputLatch getOutputLatch();
  InterruptResult getInterruptResult();

private:
  int handle;

  void write2byte(int command, byte a, byte b);
  int read2byte(int command);
};

class IOException : public std::exception
{
public:
  IOException();
  IOException(const char *message);
  virtual ~IOException() throw();

  virtual const char* what() const throw();
private:
  std::string msg;
};

#endif /* MCP23017_H_ */
