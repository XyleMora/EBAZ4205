## This file is a general .xdc for the Ebaz 4205 "bitcoin miner"
## To use it in a project:
## - uncomment the lines corresponding to used pins
## - rename the used signals according to the project
#
# Tom Trebisky  5-18-2022

# The style of this file is careful to use one line per pin.
# -- and just so you know, this is Tcl
# Another style point.  I try to keep all signal names entirely in lower case.

## LED
#set_property -dict { PACKAGE_PIN W14 IOSTANDARD LVCMOS33 } [get_ports { red_led }];
#set_property -dict { PACKAGE_PIN W13 IOSTANDARD LVCMOS33 } [get_ports { green_led }];

# And now a discussion of voltages on the PL pins that we access via
# the three 20 pin connectors..
# The Zynq puts these into two banks (34 and 34) and each bank can be configured
# to use a different IO voltage.  It could be one of 1.8, 2.5, or 3.3.
# It is important to know which and to get it right.
# Looking at the Ebaz schematic and using the package pin diagram to sort out some
# things, I am convinced that all the pins marked "VCCO_#" get 3.3 volts
# So, all of our 42 IO pins are permanently 3.3 volts on the Ebaz.
# No jumper setting of IO voltages like on some of the starter boards.

# I have been careful and used reasonable care, but there could be typos or
# mistakes in what follows.  Please let me know if any are found.
#  tom@mmto.org  Tom Trebisky

#set_property -dict { PACKAGE_PIN A20 IOSTANDARD LVCMOS33 } [get_ports { d1_5 }];   # DATA1-5
#set_property -dict { PACKAGE_PIN H16 IOSTANDARD LVCMOS33 } [get_ports { d1_6 }];   # DATA1-6
#set_property -dict { PACKAGE_PIN B19 IOSTANDARD LVCMOS33 } [get_ports { d1_7 }];   # DATA1-7
#set_property -dict { PACKAGE_PIN B20 IOSTANDARD LVCMOS33 } [get_ports { d1_8 }];   # DATA1-8
#set_property -dict { PACKAGE_PIN C20 IOSTANDARD LVCMOS33 } [get_ports { d1_9 }];   # DATA1-9
#set_property -dict { PACKAGE_PIN H17 IOSTANDARD LVCMOS33 } [get_ports { d1_11 }];  # DATA1-11
#set_property -dict { PACKAGE_PIN D20 IOSTANDARD LVCMOS33 } [get_ports { d1_13 }];  # DATA1-13
#set_property -dict { PACKAGE_PIN D18 IOSTANDARD LVCMOS33 } [get_ports { d1_14 }];  # DATA1-14
#set_property -dict { PACKAGE_PIN H18 IOSTANDARD LVCMOS33 } [get_ports { d1_15 }];  # DATA1-15
#set_property -dict { PACKAGE_PIN D19 IOSTANDARD LVCMOS33 } [get_ports { d1_16 }];  # DATA1-16
#set_property -dict { PACKAGE_PIN F20 IOSTANDARD LVCMOS33 } [get_ports { d1_17 }];  # DATA1-17
#set_property -dict { PACKAGE_PIN E19 IOSTANDARD LVCMOS33 } [get_ports { d1_18 }];  # DATA1-18
#set_property -dict { PACKAGE_PIN F19 IOSTANDARD LVCMOS33 } [get_ports { d1_19 }];  # DATA1-19
set_property -dict { PACKAGE_PIN K17 IOSTANDARD LVCMOS33 } [get_ports { d1_20 }];  # DATA1-20

#set_property -dict { PACKAGE_PIN G20 IOSTANDARD LVCMOS33 } [get_ports { d2_5 }];   # DATA2-5
#set_property -dict { PACKAGE_PIN J18 IOSTANDARD LVCMOS33 } [get_ports { d2_6 }];   # DATA2-6
#set_property -dict { PACKAGE_PIN G19 IOSTANDARD LVCMOS33 } [get_ports { d2_7 }];   # DATA2-7
#set_property -dict { PACKAGE_PIN H20 IOSTANDARD LVCMOS33 } [get_ports { d2_8 }];   # DATA2-8
#set_property -dict { PACKAGE_PIN J19 IOSTANDARD LVCMOS33 } [get_ports { d2_9 }];   # DATA2-9
#set_property -dict { PACKAGE_PIN K18 IOSTANDARD LVCMOS33 } [get_ports { d2_11 }];  # DATA2-11
#set_property -dict { PACKAGE_PIN K19 IOSTANDARD LVCMOS33 } [get_ports { d2_13 }];  # DATA2-13
#set_property -dict { PACKAGE_PIN J20 IOSTANDARD LVCMOS33 } [get_ports { d2_14 }];  # DATA2-14
#set_property -dict { PACKAGE_PIN L16 IOSTANDARD LVCMOS33 } [get_ports { d2_15 }];  # DATA2-15
#set_property -dict { PACKAGE_PIN L19 IOSTANDARD LVCMOS33 } [get_ports { d2_16 }];  # DATA2-16
#set_property -dict { PACKAGE_PIN M18 IOSTANDARD LVCMOS33 } [get_ports { d2_17 }];  # DATA2-17
#set_property -dict { PACKAGE_PIN L20 IOSTANDARD LVCMOS33 } [get_ports { d2_18 }];  # DATA2-18
#set_property -dict { PACKAGE_PIN M20 IOSTANDARD LVCMOS33 } [get_ports { d2_19 }];  # DATA2-19
#set_property -dict { PACKAGE_PIN L17 IOSTANDARD LVCMOS33 } [get_ports { d2_20 }];  # DATA2-20

#set_property -dict { PACKAGE_PIN M19 IOSTANDARD LVCMOS33 } [get_ports { d3_5 }];   # DATA3-5
#set_property -dict { PACKAGE_PIN N20 IOSTANDARD LVCMOS33 } [get_ports { d3_6 }];   # DATA3-6
#set_property -dict { PACKAGE_PIN P18 IOSTANDARD LVCMOS33 } [get_ports { d3_7 }];   # DATA3-7
#set_property -dict { PACKAGE_PIN M17 IOSTANDARD LVCMOS33 } [get_ports { d3_8 }];   # DATA3-8
#set_property -dict { PACKAGE_PIN N17 IOSTANDARD LVCMOS33 } [get_ports { d3_9 }];   # DATA3-9
#set_property -dict { PACKAGE_PIN P20 IOSTANDARD LVCMOS33 } [get_ports { d3_11 }];  # DATA3-11
#set_property -dict { PACKAGE_PIN R18 IOSTANDARD LVCMOS33 } [get_ports { d3_13 }];  # DATA3-13
#set_property -dict { PACKAGE_PIN R19 IOSTANDARD LVCMOS33 } [get_ports { d3_14 }];  # DATA3-14
#set_property -dict { PACKAGE_PIN P19 IOSTANDARD LVCMOS33 } [get_ports { d3_15 }];  # DATA3-15
#set_property -dict { PACKAGE_PIN T20 IOSTANDARD LVCMOS33 } [get_ports { d3_16 }];  # DATA3-16
#set_property -dict { PACKAGE_PIN U20 IOSTANDARD LVCMOS33 } [get_ports { d3_17 }];  # DATA3-17
#set_property -dict { PACKAGE_PIN T19 IOSTANDARD LVCMOS33 } [get_ports { d3_18 }];  # DATA3-18
#set_property -dict { PACKAGE_PIN V20 IOSTANDARD LVCMOS33 } [get_ports { d3_19 }];  # DATA3-19
#set_property -dict { PACKAGE_PIN U19 IOSTANDARD LVCMOS33 } [get_ports { d3_20 }];  # DATA3-20

# THE END
