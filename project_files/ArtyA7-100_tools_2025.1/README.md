# Demo HW design of MicroBlaze using DDR3 RAM on Arty A7

The file [ArtyA7_MicroBlaze_demo_hw.2025.1.xpr.zip](ArtyA7_MicroBlaze_demo_hw.2025.1.xpr.zip) contains Vivado project archives of the HW designs created by the [tutorial](../../README.md) in Vivado 2025.1. **This HW design uses the [Arty A7-100](https://digilent.com/shop/arty-a7-100t-artix-7-fpga-development-board/) development board.**  
To use the archive, expand the content of the .zip file to a folder on your PC and open the file ArtyA7_MicroBlaze_demo_hw.xpr by Vivado.

Make sure you have the Digilent board files installed before you open the HW project in Vivado. In the absence of the Arty A7 board file, Vivado will break the configuration of the MIG.  
[This article](https://digilent.com/reference/programmable-logic/guides/install-board-files) provides instructions on how to install the board files.

# Memory read speed benchmarking app

The file [MicroBlaze_DDR_speed_test_sw_2025.1.archive.zip](MicroBlaze_DDR_speed_test_sw_2025.1.archive.zip) is a Vitis 2025.1 project archive, which contains the benchmarking app running on the HW design created by the tutorial.  
To use the archive, open an empty workspace in Vitis 2025.1, select File|Import, and provide the path to the project archive in the field "Import from Archive".

> [!IMPORTANT]
> You need an oscilloscope in order to make use of the app.  
> The app provides no output to the console.
> 
> It drives the Arty A7 pin marked A0 high before the testing loop is executed. The pin is driven low after the loop finishes. Testing loops are repeated indefinitely.  
> You, therefore, need to measure the duration of a positive pulse created on the pin A0 down to tens of microseconds. Even a cheap scope should be able to do that.
> 
> The amount of data read from memory in the testing loop is defined by the macro BUFF_WORDS (i.e., number of 32-bit words) defined in [main.cpp](../main.cpp). The value of the macro must be divisible by 4.

> [!TIP]
> The app will, by default, run with instruction and data cache disabled, in order to measure DDR3 SDRAM read speed.  
> To do the measurement with caches enabled, comment the following line in [main.cpp](../main.cpp):

```c
//Comment out this macro in order to run the test with instruction and data cache enabled
#define CACHES_DISABLED
```

Compilers do not like loops that read data but do nothing with it. Such code is discarded by a compiler.  
In order to remove any dependency on compiler optimization, I wrote the critical piece of the benchmarking code in MicroBlaze assembly.  
This is an excerpt from [main.cpp](../main.cpp):

```c
volatile uint32_t buff[BUFF_WORDS];

/* Sequentially read content of buff into a register */
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

> [!IMPORTANT]
> Even though the critical benchmarking loop is written in assembly, do compile the whole project in the Release Configuration (i.e., with optimization set to -O2 or -O3) so the surrounding code is optimized as well.

## Measurements

I made the following measurements on Arty A7-100 using the exact HW design and SW app published in this repository:

| test buffer size                      | duration with caches disabled | duration with caches enabled |
| ------------------------------------- | ----------------------------- | ---------------------------- |
| 30 kB<br/>(fits into the 32 kB cache) | 7.24 ms                       | 0.087 ms                     |
| 50 kB                                 | 12.05 ms                      | 0.689 ms                     |
