create_clock -period 10.000 -name pcie_refclk [get_pins refclk_ibuf/O]

set_property IOSTANDARD LVCMOS33 [get_ports PCIE_RESET_N]
set_property PULLUP true [get_ports PCIE_RESET_N]
set_property PACKAGE_PIN U21 [get_ports PCIE_RESET_N]
set_false_path -from [get_ports PCIE_RESET_N]

set_property PACKAGE_PIN V22 [get_ports PCIE_LINK_UP]
set_property IOSTANDARD LVCMOS33 [get_ports PCIE_LINK_UP]

set_property PACKAGE_PIN H6 [get_ports PCIE_REFCLK_P]

set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property BITSTREAM.CONFIG.BPI_SYNC_MODE TYPE2 [current_design]
set_property BITSTREAM.CONFIG.CONFIGRATE 50 [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
set_property CONFIG_MODE BPI16 [current_design]
