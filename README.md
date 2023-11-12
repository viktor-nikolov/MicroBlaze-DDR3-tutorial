# MicroBlaze using DDR demo on ArtyA7

This is a demo HW design of [MicroBlaze](https://www.xilinx.com/products/design-tools/microblaze.html) using DDR on a [Digilent Arty A7 board](https://digilent.com/shop/arty-a7-100t-artix-7-fpga-development-board/).

Included application is a benchmarking tool for memory read speed.

The design was done in Vivado 2023.1 and Vitis 2023.1.

Tested on Arty A7-35 (no longer in production) but should be easy to port to Arty A7-100.

# MicroBlaze HW design with DDR

The design is made in Vivado 2023.1 for Arty A7-35.
**If you have Arty A7-100, you need to change the board to A7-100 in Tools|Settings|General|Project device.** No other changes in the design should be necessary.



![](ArtyA7_MicroBlaze_demo_hw\block_design.png)
