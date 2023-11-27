# Demo HW design of MicroBlaze using DDR3 RAM on Arty A7

The folder [ArtyA7_MicroBlaze_demo_hw](ArtyA7_MicroBlaze_demo_hw) contains the HW design project created by the [tutorial](../README.md).

The design was made in Vivado 2023.1 and tested on Arty A7-35 (which is no longer in production).

> [!IMPORTANT]
> **If you have [Arty A7-100](https://digilent.com/shop/arty-a7-100t-artix-7-fpga-development-board/)**, you need to change the board to A7-100 in Tools|Settings|General|Project device.
> 
> No other changes in the design should be necessary.

# Memory read speed benchmarking app

The folder [MicroBlaze_DDR_speed_test_sw](MicroBlaze_DDR_speed_test_sw) is a Vitis 2023.1 workspace of the benchmarking app.

> [!IMPORTANT]
> You need an oscilloscope in order to make use of the app.
> 
> The app provides no output to the console.
> 
> It drives the Arty A7 pin marked A0 high before the testing loop is executed. The pin is driven low after the loop finishes. Testing loops are repeated indefinitely.
> 
> You therefore need to measure the duration of a positive pulse created on the pin A0 down to tens of microseconds. Even a cheap scope should be able to do that.

dddd