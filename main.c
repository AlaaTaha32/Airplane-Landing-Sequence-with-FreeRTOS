// NANENG 410 - Project 2: RTOS
//------------------------------------------------------------------------------------------------------------------

// Including header files
//------------------------------------------------------------------------------------------------------------------
#include "TM4C123.h"                    // Device header
#include "FreeRTOSConfig.h"             // ARM.FreeRTOS::RTOS:Config
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
//------------------------------------------------------------------------------------------------------------------

// PortB Registers
//------------------------------------------------------------------------------------------------------------------
#define SYSCTL_RCGCGPIO_R   (*((volatile int *) 0x400FE608))
#define GPIO_PORTB_DEN_R    (*((volatile int *) 0x4000551C))
#define GPIO_PORTB_DIR_R    (*((volatile int *) 0x40005400))
#define GPIO_PORTB_DATA_R   (*((volatile int *) 0x400053FC))
#define GPIO_PORTB_AMSEL_R  (*((volatile int *) 0x40005528))
#define GPIO_PORTB_AFSEL_R  (*((volatile int *) 0x40005420))
#define GPIO_PORTB_PCTL_R   (*((volatile int *) 0x4000552C))
#define GPIO_PORTB_PUR_R    (*((volatile int *) 0x40005510))
#define GPIO_PORTB_LOCK_R   (*((volatile int *) 0x40005520))
#define GPIO_PORTB_CR_R     (*((volatile int *) 0x40005524))
#define GPIO_PORTB_CLK_EN  	0x02
//------------------------------------------------------------------------------------------------------------------

// PortF Registers
//------------------------------------------------------------------------------------------------------------------
#define GPIO_PORTF_DATA_R 	(*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R 		(*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R 	(*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R 		(*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R 		(*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R 	(*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R 		(*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R 	(*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R 	(*((volatile unsigned long *)0x4002552C))
#define GPIO_PORTF_DR2R_R 	(*((volatile unsigned long *)0x40025500))
#define GPIO_PORTF_DR4R_R 	(*((volatile unsigned long *)0x40025504))
#define GPIO_PORTF_DR8R_R 	(*((volatile unsigned long *)0x40025508))
#define SYSCTL_RCGC2_R 			(*((volatile unsigned long *)0x400FE108))
#define PF4                 (*((volatile unsigned long *)0x40025040))
#define PF3                 (*((volatile unsigned long *)0x40025020))
#define PF2                 (*((volatile unsigned long *)0x40025010))
#define PF1                 (*((volatile unsigned long *)0x40025008))
#define PF0                 (*((volatile unsigned long *)0x40025004))
#define GPIO_LOCK_KEY 			0x4C4F434B   // Unlocks the GPIO_CR register
//------------------------------------------------------------------------------------------------------------------

// System Control Registers
//------------------------------------------------------------------------------------------------------------------
#define SYS_CTRL_RCGC2  		(*((volatile unsigned long *)0x400FE608))   	// Offset of RCGC2 register is 0x608 
#define CLK_GPIOF   				0x20
//------------------------------------------------------------------------------------------------------------------

// GPIO PortF Registers
//------------------------------------------------------------------------------------------------------------------
#define PORTF_DATA  			(*((volatile unsigned long *)0x40025038))   		// Offset of DATA register for PF1, PF2, PF3 is  0x38
#define PORTF_DIR   			(*((volatile unsigned long *)0x40025400))   		// Offset of DIR register is 0x400
#define PORTF_DEN  				(*((volatile unsigned long *)0x4002551C))   		// Offset of DEN register is 0x51C
//------------------------------------------------------------------------------------------------------------------

// Delay Function
//------------------------------------------------------------------------------------------------------------------
void Delay(unsigned int delay);

void Delay(unsigned int delay) {

	volatile unsigned int i, counter;
	counter = delay * 4000;  										// 1 second (1000 msec) needs 40000000 counter so 4000000/1000 = 4000
	for (i = 0; i < counter; i++);

}
//------------------------------------------------------------------------------------------------------------------

// PortF Initialization
//------------------------------------------------------------------------------------------------------------------
void PortF_Init(void);

void PortF_Init(void) {

	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000020; 							// Activate clock for Port F
	delay = SYSCTL_RCGC2_R; 										// Allow time for clock to start
	GPIO_PORTF_LOCK_R = 0x4C4F434B; 						// Unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x1F; 										// Allow changes to PF4-0
	GPIO_PORTF_AMSEL_R = 0x00; 									// Disable analog on PF
	GPIO_PORTF_PCTL_R = 0x00000000; 						// PCTL GPIO on PF4-0
	GPIO_PORTF_DIR_R = 0x0E; 										// PF4,PF0 in, PF3-1 out
	GPIO_PORTF_AFSEL_R = 0x00; 									// Disable alt funct on PF7-0
	GPIO_PORTF_PUR_R = 0x11; 										// Enable pull-up on PF0 and PF4
	GPIO_PORTF_DEN_R = 0x1F; 										// Enable digital I/O on PF4-0

}
//------------------------------------------------------------------------------------------------------------------

// PortB Initialization
//------------------------------------------------------------------------------------------------------------------
void PortB_Init(void);

void PortB_Init(void) {

	SYSCTL_RCGCGPIO_R |= GPIO_PORTB_CLK_EN;					// Activate clock for Port B
	Delay(10);																			// Delay 10 msec to allow clock to start on PORTB  
	GPIO_PORTB_DEN_R = 0xFF;												// Enable all pins of PORTB 
	GPIO_PORTB_DIR_R = 0xFF;												// Make all pins of PORTB as ouptut pin
	GPIO_PORTB_PCTL_R &= ~(0xFF);										// Regular GPIO of PORTB
	GPIO_PORTB_AMSEL_R &= ~(0xFF);									// Disable analog function on all pins of PORTB
	GPIO_PORTB_AFSEL_R &= ~(0xFF);									// Regular port function

}
//------------------------------------------------------------------------------------------------------------------

// Handles
//------------------------------------------------------------------------------------------------------------------
TaskHandle_t landing_handle = NULL;
TaskHandle_t take_off_handle = NULL;
TaskHandle_t stable_handle = NULL;
TaskHandle_t idle_handle = NULL;
//------------------------------------------------------------------------------------------------------------------

// Semaphores
//------------------------------------------------------------------------------------------------------------------
SemaphoreHandle_t landing_semaphore = NULL;
SemaphoreHandle_t take_off_semaphore = NULL;
SemaphoreHandle_t stable_semaphore = NULL;
//------------------------------------------------------------------------------------------------------------------

// Global Variables
//------------------------------------------------------------------------------------------------------------------
static int k = 0; 											// Variable to control when the idle state will work
//------------------------------------------------------------------------------------------------------------------

// Landing Task
//------------------------------------------------------------------------------------------------------------------
void landing(void* pvParameters);

void landing(void* pvParameters) {

	xSemaphoreGive(landing_semaphore); 							// Give the semaphore for landing
	for (;;) {
		if (xSemaphoreTake(landing_semaphore, portMAX_DELAY) == pdTRUE) {
			if ((GPIO_PORTF_DATA_R & 0x01) == 0) {  		// On when switch 2 (PF0) is pressed (0)
				GPIO_PORTB_DATA_R = 0xF9;									// Display "1" on 7-segment
				GPIO_PORTF_DATA_R = 0x02; 								// LED Red
				k++; 																			// Increment the k to avoid getting to the idle state if the switch is pressed before the run
				xSemaphoreGive(landing_semaphore); 				// Give the semaphore again
			}
			else { 																			// This part is for the idle to work only the first time and then stop when any switch is clicked
					if (k > 0) {
						xSemaphoreGive(stable_semaphore); 		// If k > 1 give semaphore to the stable (idle is finished)
					}
				}
		}
	}

}
//------------------------------------------------------------------------------------------------------------------

// Take-Off Task
//------------------------------------------------------------------------------------------------------------------
void take_off(void* pvParameters);

void take_off(void* pvParameters) {

	xSemaphoreGive(take_off_semaphore); 						// Give the semaphore for take-off
	for (;;) {
		if ((GPIO_PORTF_DATA_R & 0x01) == 0) { 				// If switch 2 is pressed, give the semaphore to landing
			xSemaphoreGive(landing_semaphore);
		}
		else {
			if (xSemaphoreTake(take_off_semaphore, portMAX_DELAY) == pdTRUE) {
				if ((GPIO_PORTF_DATA_R & 0x10) == 0) {		// On when switch 1 (PF4) is pressed (0)
					GPIO_PORTB_DATA_R = 0xA4;								// Display "2" on 7-segment
					GPIO_PORTF_DATA_R = 0x08; 							// LED Green
					k++; 																		// Increment the k to avoid getting to the idle state if the switch is pressed before the run
					xSemaphoreGive(take_off_semaphore); 		// Give the semaphore again for take-off
				}
				else { 																		// This part is for the idle to work only the first time and then stop when any switch is clicked
					if (k > 0) {
						xSemaphoreGive(stable_semaphore); 		// If k > 1 give semaphore to the stable (idle is finished)
					}
				}
			}
		}
	}

}
//------------------------------------------------------------------------------------------------------------------

// Stable Task
//------------------------------------------------------------------------------------------------------------------
void stable(void* pvParameters);

void stable(void* pvParameters) {

	for (;;) {
		if ((GPIO_PORTF_DATA_R & 0x01) == 0) { 					// Check if switch 2 is pressed
			xSemaphoreGive(landing_semaphore);
		}
		else if ((GPIO_PORTF_DATA_R & 0x10) == 0) {			// Check if switch 1 is pressed
			xSemaphoreGive(take_off_semaphore);
		}
		else
			if (xSemaphoreTake(stable_semaphore, portMAX_DELAY) == pdTRUE) {
				GPIO_PORTB_DATA_R = 0xB0;										// Display "3" on 7-segment
				GPIO_PORTF_DATA_R = 0x00; 									// Leds OFF
				xSemaphoreGive(stable_semaphore);
			}
	}

}
//------------------------------------------------------------------------------------------------------------------

// Idle Task
//------------------------------------------------------------------------------------------------------------------
void idle(void* pvParameters);

void idle(void* pvParameters) {

	for (;;) {
		if ((GPIO_PORTF_DATA_R & 0x01) == 0) { 					// Check if switch 2 is pressed
			xSemaphoreGive(landing_semaphore);
		}
		else if ((GPIO_PORTF_DATA_R & 0x10) == 0) {			// Check if switch 1 is pressed
			xSemaphoreGive(take_off_semaphore);
		}
		else { 																					// If no button is pressed at the begining, go to idle state
				GPIO_PORTB_DATA_R = 0xFF;											// 7-Segment OFF
				GPIO_PORTF_DATA_R = 0x0A; 									// LED Yellow
				Delay(100);
				GPIO_PORTF_DATA_R = 0x06; 									// LED Pink
				Delay(100);
				GPIO_PORTF_DATA_R = 0x0C; 									// LED Sky blue
				Delay(100);
		}
	}

}
//------------------------------------------------------------------------------------------------------------------

// Main
//------------------------------------------------------------------------------------------------------------------
int main(void) {

	PortF_Init();
	PortB_Init();

	landing_semaphore		=		xSemaphoreCreateBinary();
	take_off_semaphore	=		xSemaphoreCreateBinary();
	stable_semaphore		=		xSemaphoreCreateBinary();

	xTaskCreate(landing,	"landing_red_1",		100, NULL, 4, &landing_handle);
	xTaskCreate(take_off,	"take_off_green_2", 100, NULL, 3, &take_off_handle);
	xTaskCreate(stable,		"stable_3",					100, NULL, 2, &stable_handle);
	xTaskCreate(idle,			"idle",							100, NULL, 1, &idle_handle);

	vTaskStartScheduler();

	for (;;);

}
//------------------------------------------------------------------------------------------------------------------

/* Colors
Color				LED(s)		PortF
dark				---				0
red					R-- 			0x02
blue				--B 			0x04
green				-G- 			0x08
yellow			RG- 			0x0A
sky blue		-GB 			0x0C
white				RGB 			0x0E
pink				R-B 			0x06
*/
