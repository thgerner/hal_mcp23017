
/*
 * curtain_control.cpp
 *
 *  Created on: 27.04.2023
 *      Author: thomas
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <climits>
#include <time.h>

#include <linuxcnc/rtapi.h>
#include <linuxcnc/hal.h>

#include <string>

#include "hal_mcp23017.h"
#include "mcp23017reg.h"
#include "mcp23017.h"

static struct option long_options[] =
{
		{"verbose", 0, 0, 'v'},
		{"debug", 0, 0, 'd'},
		{"help", 0, 0, 'h'},
		{"name", 1, 0, 'n'},
		{"i2cbus", 1, 0, 'i'},
		{"address", 1, 0, 'a'},
		{"interval", 1, 0, 'I'},
		{"iodira", 1, 0, 'x'},
		{"iodirb", 1, 0, 'y'},
		{0,0,0,0}
};

static int done;        // exit flag
static struct timespec schedule;

// global flags for debugging
int debug;
int verbose;

// prototypes
int export_io_expander(const char *name, int hal_comp_id, io_expander_data_t *config);
void process_hal_gpio_inputs(io_expander_data_t *config, Mcp23017 *ioexpander);
void apply_gpio_bits(io_expander_data_t *cfg, gpioBits a, gpioBits b);
void process_gpio_ports(io_expander_data_t *config, Mcp23017 *ioexpander);
void process_parameters(io_expander_data_t *config, Mcp23017 *ioexpander);

void usage(char *name)
{
	printf("Usage:  %s [options]\n", name);
	printf(
			"This is a userspace HAL program, typically loaded using the halcmd \"loadusr\" command:\n"
			"    loadusr hal_mcp23017\n"
			"There are several command-line options.\n"
			"-verbose\n"
			"    Turn on verbose messages. This prints major code flow steps.\n"
			"-debug\n"
			"    This also turn on verbose messages\n"
			"    Turn on debugging messages. Debug mode prints I2C actions.\n"
			"-name <string> (default hal_mcp23017)\n"
			"    Set the name of the HAL module.  The HAL comp name will be set to <string>, and all pin\n"
			"    and parameter names will begin with <string>.\n"
			"-i2cbus <number>\n"
			"    Bus number of the I2C bus the MCP23017 port expander is attached. Default: 2\n"
			"-address <number>\n"
			"    I2C address of the MCP23017 port expander in the i2c bus. Default: 0x20\n"
			"-iodira <value>\n"
			"    Set IO direction register A of the MCP23017 to value. This value determines whether\n"
			"    the HAL pins are created as input or as output. On default all pins are created as\n"
			"    input.\n"
			"-iodirb <value>\n"
			"    Set IO direction register B of the MCP23017 to value. This value determines whether\n"
			"    the HAL pins are created as input or as output. On default all pins are created as\n"
			"    input.\n"
			"-interval <duration in nanoseconds>\n"
			"    Duration of the polling interval the module looks for new values of the HAL pins.\n"
			"    Default: 10000000 nanoseconds (10 milliseconds).\n"
	);
}

static void quit(int sig)
{
    done = 1;
}

int check_argument_value(const char *objName, const char *prgName, const char *endptr, long value, long min, long max)
{
	if ((errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) || (errno != 0 && value == 0))
	{
		printf("%s: ERROR: Can't parse %s: %s\n", prgName, objName, strerror(errno));
		return -1;
	}
	if (endptr == optarg)
	{
		printf("%s: ERROR: No digits were found for %s, exit\n", prgName, objName);
		exit(-1);
	}
	if (value < min || value > max)
	{
		printf("%s: ERROR: %s outside allowed range of %ld ... %ld: %ld\n", prgName, objName, min, max, value);
		exit(-1);
	}
	return 0;
}

int main(int argc, char **argv)
{
	char *modname = strdup("hal_mcp23017");
	int i2cbus = 2;
	int i2caddr = 0x20;
	int dira = 0xff, dirb = 0xff;
	long interval = 10000000; // nanoseconds, == 10 milliseconds

	// set global flags
	debug = 0;
	verbose = 0;
	done = 0;

	int opt;
	while ((opt = getopt_long_only(argc, argv, "vdhn:i:a:I:", long_options, NULL)) != -1) {
		char *endptr;
		switch (opt) {
		case 'd' :      // debug
			debug = 1;
			verbose = 1;
			break;
		case 'v' :
			verbose = 1;
			break;
		case 'n':   // module base name
			if (strlen(optarg) > HAL_NAME_LEN-20)
			{
				printf("%s: ERROR: HAL module name too long: %s\n", argv[0], optarg);
				return -1;
			}
			modname = strdup(optarg);
			break;
		case 'i' :
			errno = 0;
			i2cbus = strtol(optarg, &endptr, 10);
			if (check_argument_value("i2cbus", argv[0], endptr, i2cbus, 0, 5) < 0) {
				exit(-1);
			}
			break;
		case 'a' :
			errno = 0;
			i2caddr = strtol(optarg, &endptr, 0);
			if (check_argument_value("i2c address", argv[0], endptr, i2caddr, 0, 0x7f) < 0) {
				exit(-1);
			}
			break;
		case 'x' :
			errno = 0;
			dira = strtol(optarg, &endptr, 0);
			if (check_argument_value("IO direction A", argv[0], endptr, dira, 0, 0xff) < 0) {
				exit(-1);
			}
			break;
		case 'y' :
			errno = 0;
			dirb = strtol(optarg, &endptr, 0);
			if (check_argument_value("IO direction B", argv[0], endptr, dirb, 0, 0xff) < 0) {
				exit(-1);
			}
			break;
    case 'I' :
        errno = 0;
        interval = strtol(optarg, &endptr, 10);
  			if (check_argument_value("interval", argv[0], endptr, interval, 100000, 1000000000) < 0) {
  				exit(-1);
  			}
        break;
		case 'h' :
		default :
			usage(argv[0]);
			exit(0);
		}
	}

  VERBOSE("Initialize HAL\n");
  /* create HAL component */
  int hal_comp_id = hal_init(modname);
  if (hal_comp_id < 0)
  {
      printf("%s: ERROR: hal_init failed\n", modname);
      exit(-1);
  }

  /* grab some shmem to store the HAL data in */
  DEBUG("Allocate memory for MCP23017 io expander\n");
  io_expander_data_t *config = (io_expander_data_t *)hal_malloc(sizeof(io_expander_data_t));
  if (config == 0)
  {
      printf("%s: ERROR: unable to allocate shared memory\n", modname);
      hal_exit(hal_comp_id);
      exit(-1);
  }

  config->iodir_a = dira;
  config->iodir_b = dirb;

  signal(SIGINT, quit);
  signal(SIGTERM, quit);

  Mcp23017 *ioexpander = 0;
  try
  {
  	VERBOSE("Initialize MCP23017 at /dev/i2c-%d address 0x%02x\n", i2cbus, i2caddr);
  	ioexpander = new Mcp23017(i2cbus, i2caddr);
  	// initialize IO direction register
  	Mcp23017::IoDir dir;
  	dir.a.value = dira;
  	dir.b.value = dirb;
  	ioexpander->setIoDir(dir);
  	// read current registers as default values
  	Mcp23017::IoControl iocon = ioexpander->getIoConfiguration();
  	config->iocon = iocon.iocon.value;
  	DEBUG("Read IOCON=0x%02x\n", config->iocon);
  	Mcp23017::OutputLatch olat = ioexpander->getOutputLatch();
  	config->olat_a = olat.a.value;
  	config->olat_b = olat.b.value;
  	DEBUG("Read OLATA=0x%02x OLATB=0x%02x\n", config->olat_a, config->olat_b);
  	Mcp23017::IoPol pol = ioexpander->getIoPolarity();
  	config->ipol_a = pol.a.value;
  	config->ipol_b = pol.b.value;
  	DEBUG("Read IPOLA=0x%02x IPOLB=0x%02x\n", config->ipol_a, config->ipol_b);
  	Mcp23017::InterruptOnChangeEnable gpinten = ioexpander->getIntOnChangeControl();
  	config->gpinten_a = gpinten.a.value;
  	config->gpinten_b = gpinten.b.value;
  	DEBUG("Read GPINTENA=0x%02x GPINTENB=0x%02x\n", config->gpinten_a, config->gpinten_b);
  	Mcp23017::DefaultCompare defval = ioexpander->getDefaultCompare();
  	config->defval_a = defval.a.value;
  	config->defval_b = defval.b.value;
  	DEBUG("Read DEFVALA=0x%02x DEFVALB=0x%02x\n", config->defval_a, config->defval_b);
  	Mcp23017::InterruptControl intcon = ioexpander->getInterruptControl();
  	config->intcon_a = intcon.a.value;
  	config->intcon_b = intcon.b.value;
  	DEBUG("Read INTCONA=0x%02x INTCONB=0x%02x\n", config->intcon_a, config->intcon_b);
  	Mcp23017::PullUpConfiguration gppu = ioexpander->getPullUpConfiguration();
  	config->gppu_a = gppu.a.value;
  	config->gppu_b = gppu.b.value;
  	DEBUG("Read GPPUA=0x%02x GPPUB=0x%02x\n", config->gppu_a, config->gppu_b);
  }
  catch (IOException *e)
  {
  	printf("%s, ERROR in i2c bus: %s\n", modname, e->what());
  	delete e;
  	hal_exit(hal_comp_id);
  	exit(-1);
  }

  if (export_io_expander(modname, hal_comp_id, config) < 0) {
  	ERROR("%s: ERROR: export of hal variable failed\n", modname);
  	hal_exit(hal_comp_id);
  	exit(-1);
  }

  hal_ready(hal_comp_id);

  if (clock_gettime(CLOCK_MONOTONIC, &schedule)) {
  	printf("%s: ERROR: failed to retrieve clock\n", modname);
  	hal_exit(hal_comp_id);
  	exit(-1);
  }

  process_gpio_ports(config, ioexpander);

  DEBUG("Enter main loop.\n");
  int errorcounter = 0;
  while (!done)
  {
  	int result;
  	// process data
  	try {
  		// check for GPIO inputs
  		process_hal_gpio_inputs(config, ioexpander);
  	  process_parameters(config, ioexpander);

  	  // handle interrupt pin
  	  if (!config->old_interrupt && *config->hal_interrupt) {
  	  	// Rising edge, read interrupt related registers
  	  	Mcp23017::InterruptResult intres = ioexpander->getInterruptResult();
  	  	*(config->hal_intcapa) = intres.capa.value;
  	  	*(config->hal_intcapb) = intres.capb.value;
  	  	*(config->hal_intfa) = intres.intfa.value;
  	  	*(config->hal_intfb) = intres.intfb.value;
  	  	DEBUG("Interrupt detected, read GPIUA=0x%02x GPIOB=0x%02x\n", intres.gpioa.value, intres.gpiob.value);
  	  	apply_gpio_bits(config, intres.gpioa, intres.gpiob);
  	  }
  	  config->old_interrupt = *config->hal_interrupt;

  	  errorcounter = 0;
    }
    catch (IOException *e)
    {
    	ERROR("%s, ERROR in i2c bus: %s\n", modname, e->what());
    	errorcounter += 1;
    	if (errorcounter > 5) {
    		ERROR("%s, to many successive errors, giving up.\n", modname);
    		done = 1;
    	}
    	delete e;
    }

  	// sleep till next run
  	schedule.tv_nsec += interval;
  	if(schedule.tv_nsec > 999999999) {
  		schedule.tv_nsec -= 1000000000;
  		schedule.tv_sec ++;
  	}
  	result = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &schedule, NULL);
  	if (result != 0 && result != EINTR) {
  		ERROR("clock_nonosleep() failed with error %d\n", result);
  		done = 1;
  	}
  }
  VERBOSE("Clean up application.\n");
  delete ioexpander;

	hal_exit(hal_comp_id);
	DEBUG("Exit.\n");
	exit(0);
}

int export_io_expander(const char *name, int hal_comp_id, io_expander_data_t *cfg)
{
  int retval, msg;

  /*
   * This function exports a lot of stuff, which results in a lot of
   * logging if msg_level is at INFO or ALL. So we save the current value
   * of msg_level and restore it later.  If you actually need to log this
   * function's actions, change the second line below
   */
  msg = rtapi_get_msg_level();
  rtapi_set_msg_level(RTAPI_MSG_WARN);

  // create GPIO bits
  int mask = 1;
  for (int i = 0; i < 8; ++i) {
  	if ((cfg->iodir_a & mask) == mask) {
  		DEBUG("Create HAl pin gpioa-gp%d.in as output\n", i);
  		retval = hal_pin_bit_newf(HAL_OUT, &(cfg->hal_gpioa[i]), hal_comp_id, "%s.gpioa-gp%d.in", name, i);
  	} else {
  		DEBUG("Create HAl pin gpioa-gp%d.out as input\n", i);
  		retval = hal_pin_bit_newf(HAL_IN, &(cfg->hal_gpioa[i]), hal_comp_id, "%s.gpioa-gp%d.out", name, i);
  	}
  	if (retval != 0)
  	  return retval;
  	mask <<= 1;
  }
  mask = 1;
  for (int i = 0; i < 8; ++i) {
  	if ((cfg->iodir_b & mask) == mask) {
  		DEBUG("Create HAl pin gpiob-gp%d.in as output\n", i);
  		retval = hal_pin_bit_newf(HAL_OUT, &(cfg->hal_gpiob[i]), hal_comp_id, "%s.gpiob-gp%d.in", name, i);
  	} else {
  		DEBUG("Create HAl pin gpiob-gp%d.out as input\n", i);
  		retval = hal_pin_bit_newf(HAL_IN, &(cfg->hal_gpiob[i]), hal_comp_id, "%s.gpiob-gp%d.out", name, i);
  	}
  	if (retval != 0)
  	  return retval;
  	mask <<= 1;
  }

  retval = hal_pin_bit_newf(HAL_IN, &(cfg->hal_interrupt), hal_comp_id, "%s.interrupt", name);
  if (retval != 0)
    return retval;
  cfg->old_interrupt = *(cfg->hal_interrupt);

  retval = hal_pin_u32_newf(HAL_OUT, &(cfg->hal_intfa), hal_comp_id, "%s.intf-a", name);
  if (retval != 0)
    return retval;

  retval = hal_pin_u32_newf(HAL_OUT, &(cfg->hal_intfb), hal_comp_id, "%s.intf-b", name);
  if (retval != 0)
    return retval;

  retval = hal_pin_u32_newf(HAL_OUT, &(cfg->hal_intcapa), hal_comp_id, "%s.intcap-a", name);
  if (retval != 0)
    return retval;

  retval = hal_pin_u32_newf(HAL_OUT, &(cfg->hal_intcapb), hal_comp_id, "%s.intcap-b", name);
  if (retval != 0)
    return retval;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_ipola), hal_comp_id, "%s.ipolarity-a", name);
  if (retval != 0)
    return retval;
  cfg->hal_ipola = cfg->ipol_a;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_ipolb), hal_comp_id, "%s.ipolarity-b", name);
  if (retval != 0)
    return retval;
  cfg->hal_ipolb = cfg->ipol_b;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_gpintena), hal_comp_id, "%s.gpintenable-a", name);
  if (retval != 0)
    return retval;
  cfg->hal_gpintena = cfg->gpinten_a;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_gpintenb), hal_comp_id, "%s.gpintenable-b", name);
  if (retval != 0)
    return retval;
  cfg->hal_gpintenb = cfg->gpinten_b;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_defvala), hal_comp_id, "%s.defaultvalue-a", name);
  if (retval != 0)
    return retval;
  cfg->hal_defvala = cfg->defval_a;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_defvalb), hal_comp_id, "%s.defaultvalue-b", name);
  if (retval != 0)
    return retval;
  cfg->hal_defvalb = cfg->defval_b;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_intcona), hal_comp_id, "%s.intcontrol-a", name);
  if (retval != 0)
    return retval;
  cfg->hal_intcona = cfg->intcon_a;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_intconb), hal_comp_id, "%s.intcontrol-b", name);
  if (retval != 0)
    return retval;
  cfg->hal_intconb = cfg->intcon_b;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_gppua), hal_comp_id, "%s.gppullup-a", name);
  if (retval != 0)
    return retval;
  cfg->hal_gppua = cfg->gppu_a;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_gppub), hal_comp_id, "%s.gppullup-b", name);
  if (retval != 0)
    return retval;
  cfg->hal_gppub = cfg->gppu_b;

  retval = hal_param_u32_newf(HAL_RW, &(cfg->hal_iocon), hal_comp_id, "%s.iocon", name);
  if (retval != 0)
    return retval;
  cfg->hal_iocon = cfg->iocon;

  // restore saved message level
  rtapi_set_msg_level(msg);
  return 0;
}

void process_hal_gpio_inputs(io_expander_data_t *cfg, Mcp23017 *ioexpander)
{
	// check for changed hal_gpio values and update teh MCP23017
	int olata = 0, olatb = 0;
	int mask = 1;
	for (int i = 0; i < 8; ++i) {
		if (((cfg->iodir_a & mask) == 0) && (*(cfg->hal_gpioa[i]) == true)) {
			olata |= mask;
		}
		if (((cfg->iodir_b & mask) == 0) && (*(cfg->hal_gpiob[i]) == true)) {
			olatb |= mask;
		}
		mask <<= 1;
	}

	if (olata != cfg->olat_a || olatb != cfg->olat_b) {
		Mcp23017::OutputLatch olat;
		olat.a.value = olata & 0xff;
		olat.b.value = olatb & 0xff;
		ioexpander->setOutputLatch(olat);
		cfg->olat_a = olata;
		cfg->olat_b = olatb;
		DEBUG("SET OLATA=0x%02x OLATB=0x%02x\n", olata, olatb);
		process_gpio_ports(cfg, ioexpander);
	}
}

void apply_gpio_bits(io_expander_data_t *cfg, gpioBits a, gpioBits b)
{
	int mask = 1;
	for (int i = 0; i < 8; ++i) {
		if ((cfg->iodir_a & mask) == mask) {
			bool val = (a.value & mask) == mask;
			*(cfg->hal_gpioa[i]) = val;
		}
		if ((cfg->iodir_b & mask) == mask) {
			bool val = (b.value & mask) == mask;
			*(cfg->hal_gpiob[i]) = val;
		}
		mask <<= 1;
	}
}

void process_gpio_ports(io_expander_data_t *cfg, Mcp23017 *ioexpander)
{
	Mcp23017::IoPort ioport = ioexpander->getIoPort();
	DEBUG("Read GPIOA=0x%02x GPIOB=0x%02x\n", ioport.a.value, ioport.b.value);

	apply_gpio_bits(cfg, ioport.a, ioport.b);
}

void process_parameters(io_expander_data_t *cfg, Mcp23017 *ioexpander)
{
	// check for changed parameters and update the MCP23017 accordingly
	if (cfg->hal_ipola != cfg->ipol_a || cfg->hal_ipolb != cfg->ipol_b) {
		Mcp23017::IoPol pol;
		pol.a.value = (cfg->hal_ipola & 0xff);
		pol.b.value = (cfg->hal_ipolb & 0xff);
		ioexpander->setIoPolarity(pol);
		cfg->ipol_a = cfg->hal_ipola;
		cfg->ipol_b = cfg->hal_ipolb;
		DEBUG("SET IPOLA=0x%02x IPOLB=0x%02x\n", cfg->ipol_a, cfg->ipol_b);
	}

	if (cfg->hal_gpintena != cfg->gpinten_a || cfg->hal_gpintenb != cfg->gpinten_b) {
		Mcp23017::InterruptOnChangeEnable gpinten;
		gpinten.a.value = (cfg->hal_gpintena & 0xff);
		gpinten.b.value = (cfg->hal_gpintenb & 0xff);
		ioexpander->setIntOnChangeControl(gpinten);
		cfg->gpinten_a = cfg->hal_gpintena;
		cfg->gpinten_b = cfg->hal_gpintenb;
		DEBUG("Set GPINTENA=0x%02x GPINTENB=0x%02x\n", cfg->gpinten_a, cfg->gpinten_b);
	}

	if (cfg->hal_defvala != cfg->defval_a || cfg->hal_defvalb != cfg->defval_b) {
		Mcp23017::DefaultCompare defval;
		defval.a.value = (cfg->hal_defvala & 0xff);
		defval.b.value = (cfg->hal_defvalb & 0xff);
		ioexpander->setDefaultCompare(defval);
		cfg->defval_a = cfg->hal_defvala;
		cfg->defval_b = cfg->hal_defvalb;
		DEBUG("Set DEFVALA=0x%02x DEFVALB=0x%02x\n", cfg->defval_a, cfg->defval_b);
	}

	if (cfg->hal_intcona != cfg->intcon_a || cfg->hal_intconb != cfg->intcon_b) {
		Mcp23017::InterruptControl intcon;
		intcon.a.value = (cfg->hal_intcona & 0xff);
		intcon.b.value = (cfg->hal_intconb & 0xff);
		ioexpander->setInterruptControl(intcon);
		cfg->intcon_a = cfg->hal_intcona;
		cfg->intcon_b = cfg->hal_intconb;
		DEBUG("Set INTCONA=0x%02x INTCONB=0x%02x\n", cfg->intcon_a, cfg->intcon_b);
	}

	if (cfg->hal_gppua != cfg->gppu_a || cfg->hal_gppub != cfg->gppu_b) {
		Mcp23017::PullUpConfiguration gppu;
		gppu.a.value = (cfg->hal_gppua & 0xff);
		gppu.b.value = (cfg->hal_gppub & 0xff);
		ioexpander->setPullUpConfiguration(gppu);
		cfg->gppu_a = cfg->hal_gppua;
		cfg->gppu_b = cfg->hal_gppub;
		DEBUG("Set GPPUA=0x%02x GPPUB=0x%02x\n", cfg->gppu_a, cfg->gppu_b);
	}

	if (cfg->hal_iocon != cfg->iocon) {
		Mcp23017::IoControl iocon;
		iocon.iocon.value = (cfg->hal_iocon & 0x0f);
		ioexpander->setIoConfiguration(iocon);
		cfg->iocon = cfg->hal_iocon;
		DEBUG("Set IOCON=0x%02x\n", cfg->iocon);
	}
}
