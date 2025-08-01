/*****************************************************************************************
This is a memory read speed benchmarking application for MicroBlaze.
I implemented it as part of demo HW design of MicroBlaze on Arty A7 board
(https://github.com/viktor-nikolov/MicroBlaze-DDR3-tutorial).
Nevertheless, the application can be used on any MicroBlaze design.

An oscilloscope is needed to measure speed of the memory reading code.
A GPIO pin goes high during execution of the benchmarking code.

Even though the benchmarking code is written in assembly, compile
the whole project in the "Release" setting so that all other code
is also optimized for speed.

 -------------------------------------------------------------------------------
 | UART TYPE                      BAUD RATE                                    |
 -------------------------------------------------------------------------------
   Board's UART-to-USB            9600 (configured on the AXI UART Lite IP)


BSD 2-Clause License

Copyright (c) 2025, Viktor Nikolov

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************************/

#include "xil_printf.h"
#include "sleep.h"
#include "xil_cache.h"
#include "xgpio.h"
#include "sleep.h"

#ifndef __MICROBLAZE__
	#error "This program can run only on MicroBlaze because it contains MicroBlaze-specific assembly instructions."
#endif

//Number of words in the array buff. MUST BE A NUMBER DIVISIBLE BY 4!
//#define BUFF_WORDS 1792  //size = 4*1792  =  7 kB
//#define BUFF_WORDS 2560  //size = 4*2560  = 10 kB
//#define BUFF_WORDS 6400  //size = 4*6400  = 25 kB
#define BUFF_WORDS 7680  //size = 4*7680  = 30 kB
//#define BUFF_WORDS 7936  //size = 4*7936  = 31 kB
//#define BUFF_WORDS 8704  //size = 4*8704  = 34 kB
//#define BUFF_WORDS 12800 //size = 4*12800 = 50 kB

//Comment out this macro in order to run the test with instruction and data cache enabled
#define CACHES_DISABLED

//Mask of the GPIO pin, which we are using for measuring performance of the memory reading code
//by an oscilloscope.
#define GPIO_PIN_MASK   0x01 //bit 0
 
static XGpio GpioInstance;
volatile uint32_t buff[BUFF_WORDS];

int initialize()
{
	/************ Initialize instruction and data caches ************/

#if defined(XPAR_MICROBLAZE_USE_ICACHE) && ! defined(CACHES_DISABLED)
	Xil_ICacheEnable();
#else
	#warning "Instruction cache is not active!"
#endif
#if defined(XPAR_MICROBLAZE_USE_DCACHE) && ! defined(CACHES_DISABLED)
	Xil_DCacheEnable();
#else
	#warning "Data cache is not active!"
#endif

	/************ Initialize GPIO ************/
	int Status;

#ifdef SDT // Is System Device Tree used? (I.e., are we using Vitis Unified?)
	// Constants XPAR_* come from the xparameters.h, which is generated based on the HW configuration.
	Status = XGpio_Initialize(&GpioInstance, XPAR_AXI_GPIO_0_BASEADDR);
#else
	Status = XGpio_Initialize(&GpioInstance, XPAR_GPIO_0_DEVICE_ID);
#endif
	if (Status != XST_SUCCESS) {
		xil_printf("Gpio Initialization failed\r\n");
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(&GpioInstance, 1 /*Channel*/, 0 /*DirectionMask*/ );
		//DirectionMask: Bits set to 0 are output and bits set to 1 are input.

	//Set the pin to low
	XGpio_DiscreteClear(&GpioInstance, 1 /*Channel*/, GPIO_PIN_MASK /*Mask*/ );

	return XST_SUCCESS;
} //initialize

/* The function fill_buff is compiled with all optimizations disabled
 * to make sure compiler will not remove the function's code
 */
void __attribute__((optimize("O0"))) fill_buff() {
	uint32_t c = 0;

	//We are filling the array buff with a sequence of numbers
	for( uint32_t i = 0; i < BUFF_WORDS; i++ ) {
		buff[i] = c;
		c += 0x01010101;
	}
}

int main() {
	if( initialize() != XST_SUCCESS ) //Initialize GPIO and caches
		return 1;

	fill_buff(); //Fill the array buff

	while( 1 ) {
		/* The pin goes high so we can measure duration of the following assembly code by an oscilloscope  */
		XGpio_DiscreteSet(&GpioInstance, 1 /*Channel*/, GPIO_PIN_MASK /*Mask*/ );

		/* Sequentially read content of buff into a register */
		asm volatile (
			"xor r0, r0, r0      \n\t"  //make sure r0 is zero
			"addi r10, %0        \n\t"  //[1] load address of buff to r10
			"addi r11, r0, %1    \n\t"  //[2] load value of BUFF_WORDS/4 to r11
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
		/* Note regarding instructions [1] and [2]:
		 *
		 * I assumed that [1] should read "addi r10, r0, %0". However this syntax wouldn't compile.
		 * The "addi r10, %0" compiles for example to the following two instructions
		 *     imm     -32768  //== 0x8000
		 *     addi    r10, r0 , 6344
		 * which load address 0x800018c8 into r10.
		 *
		 * [2] compiles into one or two machine instructions depending on the value of sizeof(buff).
		 * For sizeof(buff) <= 65535 one simple addi is generated.
		 * For sizeof(buff) > 65535 instruction imm is added.
		 * For example sizeof(buff)==65537 generates following two machine instructions
		 *     imm     1
		 *     addi    r11, r0, 1
		 * which load 0x00010001 into r11.
		 */

		/* The pin goes low */
		XGpio_DiscreteClear(&GpioInstance, 1 /*Channel*/, GPIO_PIN_MASK /*Mask*/ );

		/* Following applies when an AXI Timer is not part of the HW design (it is NOT part of
		 * my MicroBlaze tutorial):
		 * The call usleep(10) will sleep for 10 us only when instruction and data cache is enabled.
		 * It will take longer with the caches disabled.
		 * In absence of AXI Timer the usleep is implemented by running an assembly code of known
		 * duration when ran from the cache.
		 * See function sleep_common in ...\bsp\microblaze_0\libsrc\standalone_v8_1\src\microblaze_sleep.c
		 */
		usleep(10);
	}
}
