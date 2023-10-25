#### NAME
hal_mcp23017 - Userspace HAL component to connect a MCP23017 port expander

#### SYNOPSIS
loadusr -W hal_mcp23017 [options]

#### DESCRIPTION
hal_mcp23017 is a userspace HAL component which connects to a Microchip MCP23017 
port expander by i2c bus. It can set output pins and read input pins and can configure
all aspects of the chip like interrupt creation by the interrupt on change feature 
of the chip.

The inputs and output of the chip are configured by the ** -iodira ** and ** -iodirb **
options and are reflected by corresponding HAL pins in the component. A output pin of the 
chip is created as input pin in the component with a name ending to ```.out ``` and a input pin
of the chip is created as output pin in the component with a name ending to ```.in ```

The IO expander is not suitable for high speed port bit changes. However, it can be used for
controlling devices like coolant or tool changer.

#### OPTIONS
* -verbose  
    Turn on verbose messages. This prints major code flow steps.

* -debug  
    This also turn on verbose messages
    Turn on debugging messages. Debug mode prints I2C actions.

* -name <string> (default hal_mcp23017)  
    Set the name of the HAL module.  The HAL comp name will be set to <string>, and all pin
    and parameter names will begin with <string>.

* -i2cbus <number>  
    Bus number of the I2C bus the MCP23017 port expander is attached. Default: 2

* -address <number>  
    I2C address of the MCP23017 port expander in the i2c bus. Default: 0x20

* -iodira <value>  
    Set IO direction register A of the MCP23017 to value. This value determines whether
    the HAL pins are created as input or as output. On default all pins are created as
    input.

* -iodirb <value>  
    Set IO direction register B of the MCP23017 to value. This value determines whether
    the HAL pins are created as input or as output. On default all pins are created as
    input.

* -interval <duration in nanoseconds>  
    Duration of the polling interval the module looks for new values of the HAL pins.
    Default: 10000000 nanoseconds (10 milliseconds).

#### PINS
 **\<name\>.gpioa-gp**N**.out**  bit in  
    MCP23017 GPIOA GP pin N is configured as output
    
 **\<name\>.gpiob-gp**N**.out**  bit in  
    MCP23017 GPIOB GP pin N is configured as output
    
 **\<name\>.gpioa-gp**N**.in**  bit out  
    MCP23017 GPIOA GP pin N input value if configured as input or pin output value if configured as output
    
 **\<name\>.gpiob-gp**N**.in**  bit out  
    MCP23017 GPIOA GP pin N input value if configured as input or pin output value if configured as output

 **\<name\>.interrupt**  bit in   
    On rising edge from **false** to **true** reads the interrupt registers and GPIO pins from the MCP23017

 **\<name\>.intf-a** u32 out  
    MCP23017 interrupt flag register INTFA read by the rising edge of pin *interrupt*

 **\<name\>.intf-b** u32 out  
    MCP23017 interrupt flag register INTFB read by the rising edge of pin *interrupt*

 **\<name\>.intcap-a** u32 out  
    MCP23017 interrupt capture register INTCAPA read by the rising edge of pin *interrupt*

 **\<name\>.intcap-b** u32 out  
    MCP23017 interrupt capture register INTCAPB read by the rising edge of pin *interrupt*
    
If the bits of the interrupt registers are needed a bitslice component can be used.

#### PARAMETERS
Parameters set the MCP23017 configuration registers.

 **\<name\>.ipolarity-a** u32 rw  
    MCP23017 input polarity register IPOLA

 **\<name\>.ipolarity-b** u32 rw  
    MCP23017 input polarity register IPOLB

 **\<name\>.gpintenable-a** u32 rw  
    MCP23017 interrupt-on-change control register GPINTENA

 **\<name\>.gpintenable-b** u32 rw  
    MCP23017 interrupt-on-change control register GPINTENB

 **\<name\>.defaultvalue-a** u32 rw  
    MCP23017 default compare register DEFVALA

 **\<name\>.defaultvalue-b** u32 rw  
    MCP23017 default compare register DEFVALB

 **\<name\>.intcontrol-a** u32 rw  
    MCP23017 interrupt control register INTCONA

 **\<name\>.intcontrol-b** u32 rw  
    MCP23017 interrupt control register INTCONB

 **\<name\>.gppullup-a** u32 rw  
    MCP23017 pull-up resistor configuration register GPPUA

 **\<name\>.gppullup-b** u32 rw  
    MCP23017 pull-up resistor configuration register GPPUB

 **\<name\>.iocon** u32 rw  
    MCP23017 configuration register IOCON  
    The IOCON register bits BANK and SEQOP are controlled by the component as they are 
    important for correct functionality of the component.

#### USGAE
loadusr -W hal_mcp23017 -i2cbus 1 -address 0x20 -iodira 0xaa -name ioexpander

    Look for a MCP23017 on I2C bus 1 at I2C address 0x20 and configure register IODIRA with 0xaa, name the component *ioexpander* 

#### AUTHOR
Written by Thomas Gerner

#### SEE ALSO
bitslice (9)

