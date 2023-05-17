create_clock -period 10.000 -name pcie_refclk [get_pins refclk_ibuf/O]

set_property IOSTANDARD LVCMOS33 [get_ports PCIE_RESET_N]
set_property PULLUP true [get_ports PCIE_RESET_N]
set_property PACKAGE_PIN AC13 [get_ports PCIE_RESET_N]
set_false_path -from [get_ports PCIE_RESET_N]

set_property PACKAGE_PIN U6 [get_ports PCIE_REFCLK_P]

set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]