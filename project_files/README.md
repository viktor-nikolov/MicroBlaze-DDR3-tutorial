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
> The app provides no output to the console.
> 
> It drives the Arty A7 pin marked A0 high before the testing loop is executed. The pin is driven low after the loop finishes. Testing loops are repeated indefinitely.  
> You therefore need to measure the duration of a positive pulse created on the pin A0 down to tens of microseconds. Even a cheap scope should be able to do that.

Compilers do not like loops, which read data and do nothing with them. Such code is discarded by a compiler.  
In order to remove any dependency on compiler optimization I wrote the critical piece of code in assembly.  
This is an excerpt from [main.cpp](MicroBlaze_DDR_speed_test_sw/DDR3_read_test/src/main.cpp):

```
/* Sequentially read content of array buff into a register */
asm volatile (
	"xor r0, r0, r0      \n\t"  //make sure r0 is zero
	"addi r10, %0        \n\t"  //load address of buff to r10
	"addi r11, r0, %1    \n\t"  //load value of BUFF_WORDS/4 to r11
	"addi r13, r0, -1    \n\t"  //load value -1 to r13
	"1:                  \n\t"  //label for branching
	//Load four 32b words from memory:
	"lwi  r12, r10, 0    \n\t"  //load 32b word from address r10 to r12
	"lwi  r12, r10, 4    \n\t"  //load 32b word from address r10+4 to r12
	"lwi  r12, r10, 8    \n\t"  //load 32b word from address r10+8 to r12
	"lwi  r12, r10, 12   \n\t"  //load 32b word from address r10+14 to r12
	"addi r10, r10, 4*4  \n\t"  //increment address in r10 to next 4 words
	"add  r11, r11, r13  \n\t"  //decrement counter in r11 (r13 == -1)
	"bgti r11, 1b        \n\t"  //if r11 > 0 then branch backward to label 1
	:                                   //no output operands
	: "m" (buff), "i" (BUFF_WORDS/4)    //input operands
	: "r0","r10","r11","r12","r13","cc" //clobbered registers + CPU condition codes
);
```
dddd