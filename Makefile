PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

ifeq ($(BUILD_HOST),devel)
  CFLAGS += -I/home/thomas/develop/linuxcnc-dev/include
  LDFLAGS += -L/home/thomas/develop/linuxcnc-dev/lib
else ifeq ($(BUILD_HOST),beagle)
  CFLAGS += -I/usr/include/linuxcnc
else
define ERROR_BODY

*******************************************************
Build host $(BUILD_HOST) not supported by this Makefile

Please define variable BUILD_HOST to be either
- devel for building on Thomas' debelopment host
- beagle for building on the beagle bone black
*******************************************************

endef
  $(error $(ERROR_BODY))
endif

CFLAGS += -DULAPI
OBJS = hal_mcp23017.o mcp23017.o
LIBS = -li2c -llinuxcnchal

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g -O
else 
# run mode
	CFLAGS += -O2
endif

all:	hal_mcp23017

hal_mcp23017:	$(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(EXTRA_CMDS)

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr hal_mcp23017 $(OBJS) $(EXTRA_CLEAN)
