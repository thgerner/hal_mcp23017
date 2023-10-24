/*
 * hal_mcp23017.h
 *
 *  Created on: 27.04.2023
 *      Author: thomas
 */

#ifndef HAL_MCP23017_H_
#define HAL_MCP23017_H_

typedef struct {
	int iodir_a, iodir_b;					  // IO direction of MCP23017; bit set: in, bit clear: out
	// old register values
	int olat_a, olat_b;
	uint32_t	    ipol_a, ipol_b;
	uint32_t			gpinten_a, gpinten_b;
	uint32_t			defval_a, defval_b;
	uint32_t			intcon_a, intcon_b;
	uint32_t			gppu_a, gppu_b;
	uint32_t			iocon;
	// GPIO registers are splotted to bits
	hal_bit_t 		*hal_gpioa_in[8];		// GPIOA
	hal_bit_t			*hal_gpioa_out[8];
	hal_bit_t 		*hal_gpiob_in[8];		// GPIOB
	hal_bit_t			*hal_gpiob_out[8];
	hal_bit_t			*hal_interrupt;
	// the interrupt registers are provided as int value; if the bits are needed
	// a bitslice component can be used
	bool          old_interrupt;
	hal_u32_t			*hal_intfa;
	hal_u32_t			*hal_intfb;
	hal_u32_t			*hal_intcapa;
	hal_u32_t			*hal_intcapb;
	// parameters
	uint32_t			hal_ipola;
	uint32_t			hal_ipolb;
	uint32_t			hal_gpintena;
	uint32_t			hal_gpintenb;
	uint32_t			hal_defvala;
	uint32_t			hal_defvalb;
	uint32_t			hal_intcona;
	uint32_t			hal_intconb;
	uint32_t			hal_gppua;
	uint32_t			hal_gppub;
	uint32_t			hal_iocon;
} io_expander_data_t;

// global flags for debugging
extern int debug;
extern int verbose;

#define DEBUG(format, args...)  if (debug) printf(format, ##args)
#define VERBOSE(format, args...)  if (verbose) printf(format, ##args)
#define ERROR(format, args...) rtapi_print_msg(RTAPI_MSG_ERR, format, ##args)

#endif /* HAL_MCP23017_H_ */
