# riffa
## Introduction
RIFFA (Reusable Integration Framework for FPGA Accelerators) is a simple framework for communicating data from a host CPU to a FPGA via a PCI Express bus. The framework requires a PCIe enabled workstation and a FPGA on a board with a PCIe connector. RIFFA supports Windows and Linux, Altera and Xilinx, with bindings for C/C++, Python, MATLAB and Java.

## New features
* AXI4LITE master interface added as BAR2
* User interrupt added
* rx_port_128.v CHNL falling too early, causing data corrpution. 
* vivado 2018.2 bugfix
* verilog bugfix

## Details
On the software side there are two main functions: data send and data receive. These functions are exposed via user libraries in C/C++, Python, MATLAB, and Java. The driver supports multiple FPGAs (up to 5) per system. The software bindings work on Linux and Windows operating systems. Users can communicate with FPGA IP cores by writing only a few lines of code.

On the hardware side, users access an interface with independent transmit and receive signals. The signals provide transaction handshaking and a first word fall through FIFO interface for reading/writing data. No knowledge of bus addresses, buffer sizes, or PCIe packet formats is required. Simply send data on a FIFO interface and receive data on a FIFO interface. RIFFA does not rely on a PCIe Bridge and therefore is not subject to the limitations of a bridge implementation. Instead, RIFFA works directly with the PCIe Endpoint and can run fast enough to saturate the PCIe link.  Both the software and hardware interfaces have been greatly simplified. Details can be found on the hardware interface page.

RIFFA communicates data using direct memory access (DMA) transfers and interrupt signaling. This achieves high bandwidth over the PCIe link. In our tests we are able to saturate (or near saturate) the link in all our tests. We have implemented RIFFA on the AVNet Spartan LX150T, Xilinx ML605, and Xilinx VC707, as well as the Altera DE5-Net, DE4 and DE2i boards. The RIFFA distribution contains examples and guides for setting up designs on the three development boards listed above. In addition, this website has examples of how to access your design from all the software bindings. RIFFA has been tested on Fedora 13 & 17 (32/64 bit vers.) and Ubuntu Desktop 10.04 LTS & 12.04 LTS (32/64 bit vers.). RIFFA relies on a custom Linux kernel driver which is supported on Linux kernels 2.6.27+ (tested on versions between 2.6.32 - 3.3.0). The Windows driver is supported on: Windows 7 in both 32 bit and 64 bit variants
