# THIS FILE IS AUTOMATICALLY GENERATED
# Project: D:\TT2 Code - Second Edition - TT2 line following\CS301_Class.cyprj.Archive01-20190825T234813Z-001\CS301_Class.cyprj.Archive01\CS301_Class.cydsn\CS301_Class.cyprj
# Date: Thu, 03 Oct 2019 06:12:26 GMT
#set_units -time ns
create_clock -name {ADC_IntClock(routed)} -period 949.99999999999989 -waveform {0 475} [list [get_pins {ClockBlock/dclk_2}]]
create_clock -name {CyILO} -period 10000 -waveform {0 5000} [list [get_pins {ClockBlock/ilo}] [get_pins {ClockBlock/clk_100k}] [get_pins {ClockBlock/clk_1k}] [get_pins {ClockBlock/clk_32k}]]
create_clock -name {CyIMO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/imo}]]
create_clock -name {CyPLL_OUT} -period 16.666666666666664 -waveform {0 8.33333333333333} [list [get_pins {ClockBlock/pllout}]]
create_clock -name {CyMASTER_CLK} -period 16.666666666666664 -waveform {0 8.33333333333333} [list [get_pins {ClockBlock/clk_sync}]]
create_generated_clock -name {CyBUS_CLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 2 3} [list [get_pins {ClockBlock/clk_bus_glb}]]
create_generated_clock -name {Clock_QENC} -source [get_pins {ClockBlock/clk_sync}] -edges {1 5 11} -nominal_period 83.333333333333314 [list [get_pins {ClockBlock/dclk_glb_0}]]
create_generated_clock -name {ADC_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 57 115} [list [get_pins {ClockBlock/dclk_glb_2}]]
create_generated_clock -name {UART_IntClock} -source [get_pins {ClockBlock/clk_sync}] -edges {1 131 261} [list [get_pins {ClockBlock/dclk_glb_3}]]
create_generated_clock -name {Clock2} -source [get_pins {ClockBlock/clk_sync}] -edges {1 601 1201} [list [get_pins {ClockBlock/dclk_glb_4}]]
create_clock -name {Clock2(fixed-function)} -period 10000 -waveform {0 16.6666666666667} [get_pins {ClockBlock/dclk_glb_ff_4}]
create_generated_clock -name {Clock_PWM} -source [get_pins {ClockBlock/clk_sync}] -edges {1 13 25} [list [get_pins {ClockBlock/dclk_glb_1}]]
create_clock -name {Clock_PWM(fixed-function)} -period 199.99999999999997 -waveform {0 16.6666666666667} [get_pins {ClockBlock/dclk_glb_ff_1}]

set_false_path -from [get_pins {__ONE__/q}]

# Component constraints for D:\TT2 Code - Second Edition - TT2 line following\CS301_Class.cyprj.Archive01-20190825T234813Z-001\CS301_Class.cyprj.Archive01\CS301_Class.cydsn\TopDesign\TopDesign.cysch
# Project: D:\TT2 Code - Second Edition - TT2 line following\CS301_Class.cyprj.Archive01-20190825T234813Z-001\CS301_Class.cyprj.Archive01\CS301_Class.cydsn\CS301_Class.cyprj
# Date: Thu, 03 Oct 2019 06:12:12 GMT
