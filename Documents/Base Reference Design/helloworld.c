/******************************************************************************
*
* Based on Xilinx example:
* @file xgpiops_intr_example.c
*
* This file contains a design example using the GPIO driver (XGpioPs) in an
* interrupt driven mode of operation.
*
* The example uses the interrupt capability of the GPIO to detect push button
* events and set the output LEDs based on the input . The user needs to press
* the switches SW2 on the evaluation board to exit from this example.
*
* @note
* This example is to provide support only for EBAZ4205 board on Zynq Platform.
* This example assumes that there is a Uart device in the HW design.
*
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "platform.h"
#include "xparameters.h"
#include "xgpiops.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xplatform_info.h"
#include <xil_printf.h>

#include <stdio.h>
#include "xscutimer.h"
#include "xgpio.h"

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the names of the hardware instances that
 * were created in the EDK XPS system.  They are defined here such that
 * the user can easily change all the needed device IDs in one place.
 */
#define GPIO_DEVICE_ID		XPAR_XGPIOPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_INTERRUPT_ID	XPAR_XGPIOPS_0_INTR

/* The following constants define the GPIO banks that are used. */
#define INPUT_BANK	XGPIOPS_BANK0  /* Bank 0 of the GPIO Device */
//#define OUTPUT_BANK	XGPIOPS_BANK1  /* Bank 1 of the GPIO Device */

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

static int GpioIntrExample(XScuGic *Intc, XGpioPs *Gpio, u16 DeviceId, u16 GpioIntrId);
static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status);
static int SetupInterruptSystem(XScuGic *Intc, XGpioPs *Gpio, u16 GpioIntrId);

static void my_timer_interrupt_handler(void *CallBackRef);

/************************** Variable Definitions *****************************/

/*
 * The following are declared globally so they are zeroed and so they are
 * easily accessible from a debugger.
 */
static XGpioPs Gpio; /* The Instance of the GPIO Driver for PS*/
static XScuGic Intc; /* The Instance of the Interrupt Controller Driver */

/* Declarations of the GPIO instance structs for PL (axi_gpio_1 LEDs & axi_gpio_0 inputs)*/
static XGpio Gpio0, Gpio1;
static XGpio_Config *GPIO_Config0;
static XGpio_Config *GPIO_Config1;

static XScuTimer my_Timer; /* The Instance of the Timer Driver */
static XScuTimer_Config *Timer_Config;

static u32 Input_Pin=20; /* Switch button S2 - A17*/
static u32 Input_Pin_NC=32; /* Switch button S3 - A14 not connected*/
static u32 Output_Pin=22; /* LED pin - B17 not connected*/
static int SW2_value, SW3_value, LED_state=1, InterruptCounter; // Other variables

/****************************************************************************/
/**
*
* Main function that invokes the GPIO Interrupt example.
*
* @param	None.
*
* @return
*		- XST_SUCCESS if the example has completed successfully.
*		- XST_FAILURE if the example has failed.
*
* @note		None.
*
*****************************************************************************/
int main(void)
{	init_platform();

	int Status;

	xil_printf("EBAZ4205 GPIO Interrupt Button2LED Example \r\n");

	/*
	 * Run the GPIO interrupt example, specify the parameters that
	 * are generated in xparameters.h.
	 */
	Status = GpioIntrExample(&Intc, &Gpio, GPIO_DEVICE_ID,
				 GPIO_INTERRUPT_ID);

	if (Status != XST_SUCCESS) {
		xil_printf("GPIO Interrupt Example Test Failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran GPIO Interrupt Example Test\r\n");
	return XST_SUCCESS;
}

/****************************************************************************/
/**
* This function shows the usage of interrupt functionality of the GPIO device.
* It is responsible for initializing the GPIO device, setting up interrupts and
* providing a foreground loop such that interrupts can occur in the background.
*
* @param	Intc is a pointer to the XScuGic driver Instance.
* @param	Gpio is a pointer to the XGpioPs driver Instance.
* @param	DeviceId is the XPAR_<Gpio_Instance>_PS_DEVICE_ID value
*		from xparameters.h.
* @param	GpioIntrId is XPAR_<GIC>_<GPIO_Instance>_VEC_ID value
*		from xparameters.h
*
* @return	- XST_SUCCESS if the example has completed successfully.
*		- XST_FAILURE if the example has failed.
*
* @note		None
*
*****************************************************************************/
int GpioIntrExample(XScuGic *Intc, XGpioPs *Gpio, u16 DeviceId, u16 GpioIntrId)
{
	int Status;

	/* Initialize and Config the Timer. */
	Timer_Config = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
	XScuTimer_CfgInitialize(&my_Timer, Timer_Config, Timer_Config->BaseAddr);

	/* GPIO PL Initialisation */
	//  Lookup the config information and store it in the struct "GPIO_Config" for axi_gpio_0
	GPIO_Config0 = XGpio_LookupConfig(XPAR_AXI_GPIO_0_DEVICE_ID);
	//  Initialise the GPIO using a reference to the Gpio PL struct, "GPIO_Config", and a base address value
	XGpio_CfgInitialize(&Gpio0, GPIO_Config0, GPIO_Config0->BaseAddress);
	//  Same for axi_gpio_1
	GPIO_Config1 = XGpio_LookupConfig(XPAR_AXI_GPIO_1_DEVICE_ID);
	XGpio_CfgInitialize(&Gpio1, GPIO_Config1, GPIO_Config1->BaseAddress);

	//  Bits in PL set to 0 are output and bits set to 1 and input channel of the GPIO (1 of 2)
	XGpio_SetDataDirection(&Gpio0, 1, 0xffffffff);
	XGpio_SetDataDirection(&Gpio1, 1, 0x00000000);

	/* Initialize the Gpio PS driver. */
	XGpioPs_Config *ConfigPtr;
	ConfigPtr = XGpioPs_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}
	XGpioPs_CfgInitialize(Gpio, ConfigPtr, ConfigPtr->BaseAddr);

	/* Run a self-test on the GPIO device. */
	Status = XGpioPs_SelfTest(Gpio);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Set the direction for the specified pin to be input in the PS, 0 - in, 1 - out */
	XGpioPs_SetDirectionPin(Gpio, Input_Pin, 0x0);
	XGpioPs_SetDirectionPin(Gpio, Input_Pin_NC, 0x0);

	/* Set the direction for the specified PS pin to be output. */
	XGpioPs_SetDirectionPin(Gpio, Output_Pin, 1);
	XGpioPs_SetOutputEnablePin(Gpio, Output_Pin, 1);
	XGpioPs_WritePin(Gpio, Output_Pin, 0x0);

	/*
	 * Setup the interrupts such that interrupt processing can occur. If
	 * an error occurs then exit.
	 */
	Status = SetupInterruptSystem(Intc, Gpio, GPIO_INTERRUPT_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* GPIO READ/WRITE */
	//	Read/write GPIO pins in PS and PL (LED lights up with 0, pressed button gives 1)
	//  XGpioPs_WritePin(XGpioPs *InstancePtr, int Pin, int Data);
	SW2_value = XGpioPs_ReadPin(Gpio, Input_Pin); // Red LED
	SW3_value = XGpioPs_ReadPin(Gpio, Input_Pin_NC); // Loop Exit

	// Button to red LED and LED_state to green blink enabling
	XGpio_DiscreteWrite(&Gpio1, 1, (LED_state << 1) | (SW2_value));

	xil_printf("\n\rS2 button = 0x%04X, S3 button = 0x%04X, LED = 0x%04X\n\r", SW2_value, SW3_value, LED_state);
	xil_printf("\n\rShort the button S3 contact to exit\n\r");

	/* Creates an infinite loop of nothing-ness
	 * or put your code for something useful
	 */
	while(SW3_value == FALSE){                }

	xil_printf("\n\rIf we see this message, then we've broken out of the while loop\n\r");

	return XST_SUCCESS;
}

/****************************************************************************/
/**
* This function is the user layer callback function for the bank 0 interrupts of
* the GPIO device. It checks if all the switches have been pressed to stop the
* interrupt processing and exit from the example.
*
* @param	CallBackRef is a pointer to the upper layer callback reference.
* @param	Status is the Interrupt status of the GPIO bank.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
//static void IntrHandler(void *CallBackRef)
static void IntrHandler(void *CallBackRef, u32 Bank, u32 Status)
{
	XGpioPs *Gpio_LOCAL = (XGpioPs *)CallBackRef;

	InterruptCounter=InterruptCounter+1;

	xil_printf("\n\r*If we see this message, then we were in the gpio interrupt handler*\n\r");
	xil_printf("\n\rButton = 0x%04X, LED = 0x%04X, Interrupt Counter = 0x%04X\n\r", SW2_value, LED_state, InterruptCounter);

	XGpioPs_IntrClearPin(Gpio_LOCAL, Input_Pin);
	XGpioPs_IntrDisablePin(Gpio_LOCAL, Input_Pin); // Disable S2 button interrupts till bounce end

	// Load the timer with a value that represents one second of real time
	// HINT: The SCU Timer is clocked at half the frequency of the CPU.
	XScuTimer_LoadTimer(&my_Timer, XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 20); // Debounce 0,1sec
	// Enable Auto reload mode on the timer.  When it expires, it re-loads
	// the original value automatically.  This means that the timing interval
	// is never skewed by the time taken for the interrupt handler to run
//	XScuTimer_EnableAutoReload(&my_Timer); // no need
	// Start the SCU timer running (it counts down)
	XScuTimer_Start(&my_Timer);
}

/****************************************************************************/
/**
 * TIMER device handler
******************************************************************************/
static void my_timer_interrupt_handler(void *CallBackRef)
{
	XGpioPs_IntrEnablePin(&Gpio, Input_Pin); // Enable S2 button interrupts again

	// Print something to the UART to show that we're in the interrupt handler
	//int InterruptCounter=0;
	SW2_value = XGpioPs_ReadPin(&Gpio, Input_Pin); //Read S2 after timer expired

	XGpio_DiscreteWrite(&Gpio1, 1, (LED_state << 1) | (SW2_value));

	printf("\n\r*This message comes from the timer interrupt handler, S2(%d)*\n\r\n\n\r", SW2_value);
}

/*****************************************************************************/
/**
*
* This function sets up the interrupt system for the example. It enables falling
* edge interrupts for all the pins of bank 0 in the GPIO device.
*
* @param	GicInstancePtr is a pointer to the XScuGic driver Instance.
* @param	GpioInstancePtr contains a pointer to the instance of the GPIO
*		component which is going to be connected to the interrupt
*		controller.
* @param	GpioIntrId is the interrupt Id and is typically
*		XPAR_<GICPS>_<GPIOPS_instance>_VEC_ID value from
*		xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
static int SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio,
				u16 GpioIntrId)
{
	int Status;

	/* GIC /
	 * Instance of the interrupt controller
	 */
	XScuGic_Config *IntcConfig;
	Xil_ExceptionInit();

	/*
	 * Initialize the interrupt controller driver so that it is ready to use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(GicInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler,
				GicInstancePtr);

	/* GPIO /
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(GicInstancePtr, GpioIntrId,
				(Xil_ExceptionHandler)XGpioPs_IntrHandler,
				(void *)Gpio);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/* Enable edge interrupts for all the pins in Bank 0, 32 bit mask
	 0 Level Sensitive, 1 Edge Sensitive
	 0 Active Low or Falling Edge, 1 Active High or Rising Edge
	 0 single edge, 1 both edges
	 or function for the specified pin
	 */
//	XGpioPs_SetIntrType(Gpio, INPUT_BANK, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	XGpioPs_SetIntrTypePin	(Gpio, Input_Pin, XGPIOPS_IRQ_TYPE_EDGE_BOTH);

	/* Set the handler for gpio interrupts. */
	XGpioPs_SetCallbackHandler(Gpio, (void *)Gpio, IntrHandler);

	/* Enable the GPIO interrupts of Bank 0.
	 * bit positions of 1 will be enabled, 0 will keep the previous setting
	 * or function for the specified pin
	 */
	XGpioPs_IntrClearPin(Gpio, Input_Pin); // clear Input_Pin pending irqs
	XGpioPs_IntrEnable(Gpio, INPUT_BANK, (1 << Input_Pin)); //XGpioPs_IntrEnablePin(Gpio, Input_Pin);

	/* Enable the interrupt for the GPIO device. */
	XScuGic_Enable(GicInstancePtr, GpioIntrId);

	/* TIMER /
	Assign the timer interrupt handler.
	*/
	XScuGic_Connect(&Intc, XPAR_SCUTIMER_INTR,
		(Xil_ExceptionHandler)my_timer_interrupt_handler,
		(void *)&my_Timer);

	/* Enable the interrupt input for the timer on the interrupt controller. */
	XScuGic_Enable(&Intc, XPAR_SCUTIMER_INTR);

	/* Enable the interrupt output on the timer. */
	XScuTimer_EnableInterrupt(&my_Timer);

	/* PS /
	Enable interrupts in the Processor.
	*/
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	return XST_SUCCESS;
}
