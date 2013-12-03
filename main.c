/*!
	@file main.c
	Main file of the receiver project. 
*/
#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_init.h"
#include "project_servo.h"
#include "project_magnet.h"
#include "project_robot.h"
#include "CC2500.h"
#include "project_receiver.h"
#include "project_pushbutton.h"

#include <stdlib.h>


/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void armThread(void const *argument);
void ledThread(void const *argument);

static struct Servo boomServo, crowdServo, swingServo;
static struct Robot robot;
static struct Magnet magnet;
static struct Receiver receiver;
static struct Pushbutton pushbutton;

static uint32_t mode = 0;

static osThreadId *tid_thread_pushbutton;
static osThreadId *tid_thread_receiver;

//! Thread structure for above thread
osThreadDef(armThread, osPriorityNormal, 1, 0);
osThreadDef(ledThread, osPriorityNormal, 1, 0);

/*!
 @brief Program entry point
 */
int main (void) {
	struct MagnetInit magnetInit;
	struct ServoInit boomServoInit, crowdServoInit, swingServoInit;
	struct RobotInit robotInit;
	
	magnetInit.periph = RCC_AHB1Periph_GPIOD;
	magnetInit.GPIOx = GPIOD;
	magnetInit.pin = GPIO_Pin_15;
	
	boomServoInit.CCR = 1;
	boomServoInit.maxPosition = BOOM_ANGLE_MAX;
	boomServoInit.minPosition = BOOM_ANGLE_MIN;
	boomServoInit.startingPosition = BOOM_ANGLE_STARTING;
	
	crowdServoInit.CCR = 2;
	crowdServoInit.maxPosition = CROWD_ANGLE_MAX;
	crowdServoInit.minPosition = CROWD_ANGLE_MIN;
	crowdServoInit.startingPosition = CROWD_ANGLE_STARTING;
	
	swingServoInit.CCR = 3;
	swingServoInit.maxPosition = SWING_ANGLE_MAX;
	swingServoInit.minPosition = SWING_ANGLE_MIN;
	swingServoInit.startingPosition = SWING_ANGLE_STARTING;
	
	robotInit.boomServo = &boomServo;
	robotInit.crowdServo = &crowdServo;
	robotInit.swingServo = &swingServo;
	
	osThreadId tid_armThread;
	osThreadId tid_ledThread;

	init_TIM4(1 / SERVO_DUTY_CYCLE_STEP, SERVO_FREQUENCY);
	init_LEDS_PWM();
	init_LEDS();
	init_userButton();
	init_TIM3();
	
	init_servo(&boomServo, &boomServoInit);
	init_servo(&crowdServo, &crowdServoInit);
	init_servo(&swingServo, &swingServoInit);
	init_robot(&robot, &robotInit);
	init_magnet(&magnet, &magnetInit);
	init_receiver(&receiver, &tid_thread_receiver);
	init_pushbutton(&pushbutton, &tid_thread_pushbutton);
	
	tid_armThread = osThreadCreate(osThread(armThread), NULL);
	tid_ledThread = osThreadCreate(osThread(ledThread), NULL);
	
	osDelay(osWaitForever);
}


/*!
 @brief Thread that controls movement of arm and magnet state.
 @param argument Unused
 @retval None.
 */
void armThread (void const *argument) {	
	uint8_t data[RECEIVE_DATA_SIZE];
	uint32_t buttonPressed;

	parkRobot(&robot);
	osDelay(1000);

	uint32_t prevY1=0, prevAngle1=0, nextY1=rand()%(Y_MAX+1), nextAngle1=rand()%(ANGLE_MAX+1);
	uint32_t prevY2=0, prevAngle2=0, nextY2=rand()%(Y_MAX+1), nextAngle2=rand()%(ANGLE_MAX+1);
	while(1) {
		
		osMutexWait(pushbutton.mutexID, osWaitForever);
		if (pushbutton.buttonPressed) {
			buttonPressed = 1;
			pushbutton.buttonPressed = 0;
		}
		osMutexRelease(pushbutton.mutexID);

		if (buttonPressed) {
			if (mode) {
				mode = 0;
			} else {
				mode = 1;
				prevY1 = 0;
				prevAngle1 = 0;
				prevY2 = 0;
				prevAngle2 = 0;
			}
			buttonPressed = 0;
		}
		
		if (!mode) {
			currentData(&receiver, data);
			moveRobot(&robot, data[0], data[1], data[2]);
			if (data[3]) {
				turnMagnetOn(&magnet);
			} else {
				turnMagnetOff(&magnet);
			}
		} else {
			moveRobot(&robot, prevY1, 0, prevAngle1);
			turnMagnetOn(&magnet);
			osDelay(RANDOM_MODE_DELAY);
			moveRobot(&robot, nextY1, 0, nextAngle1);
			turnMagnetOff(&magnet);
			osDelay(RANDOM_MODE_DELAY);
			
			moveRobot(&robot, prevY2, 0, prevAngle2);
			turnMagnetOn(&magnet);
			osDelay(RANDOM_MODE_DELAY);
			moveRobot(&robot, nextY2, 0, nextAngle2);
			turnMagnetOff(&magnet);
			osDelay(RANDOM_MODE_DELAY);
			
			prevY1=nextY1;
			prevAngle1=nextAngle1;
			prevY2=nextY2;
			prevAngle2=nextAngle2;
			
			nextY1=rand()%(Y_MAX+1);
			nextAngle1=rand()%(ANGLE_MAX+1);
			nextY2=rand()%(Y_MAX+1);;
			nextAngle2=rand()%(ANGLE_MAX+1);
		}
		osDelay(ARMTHREAD_LOOP_DELAY);
	}
}

/*!
 @brief Thread that controls the LEDs along the arm. 
 @param *argument Unused
 @retval None
 */
void ledThread (void const *argument) {
	uint32_t led = 0;
	uint32_t delay = 0;
	uint8_t data[RECEIVE_DATA_SIZE];
	while(1) {
		currentData(&receiver, data);
		if (!mode) {
			delay = data[4];
		} else {
			delay = 0;
		}
		
		if (delay==0) {
			GPIO_SetBits(GPIOD, GPIO_Pin_0);
			GPIO_SetBits(GPIOD, GPIO_Pin_1);
			GPIO_SetBits(GPIOD, GPIO_Pin_2);
			GPIO_SetBits(GPIOD, GPIO_Pin_3);
			GPIO_SetBits(GPIOD, GPIO_Pin_4);
			GPIO_SetBits(GPIOD, GPIO_Pin_5);
			GPIO_SetBits(GPIOD, GPIO_Pin_6);
		} else {
			GPIO_ResetBits(GPIOD, GPIO_Pin_0);
			GPIO_ResetBits(GPIOD, GPIO_Pin_1);
			GPIO_ResetBits(GPIOD, GPIO_Pin_2);
			GPIO_ResetBits(GPIOD, GPIO_Pin_3);
			GPIO_ResetBits(GPIOD, GPIO_Pin_4);
			GPIO_ResetBits(GPIOD, GPIO_Pin_5);
			GPIO_ResetBits(GPIOD, GPIO_Pin_6);
			switch (led) {
				case 0:
					GPIO_SetBits(GPIOD, GPIO_Pin_0);
					break;
				case 1:
					GPIO_SetBits(GPIOD, GPIO_Pin_1);
					break;
				case 2:
					GPIO_SetBits(GPIOD, GPIO_Pin_2);
					break;
				case 3:
					GPIO_SetBits(GPIOD, GPIO_Pin_3);
					break;
				case 4:
					GPIO_SetBits(GPIOD, GPIO_Pin_4);
					break;
				case 5:
					GPIO_SetBits(GPIOD, GPIO_Pin_5);
					break;
				case 7:
					GPIO_SetBits(GPIOD, GPIO_Pin_5);
					break;
				case 8:
					GPIO_SetBits(GPIOD, GPIO_Pin_4);
					break;
				case 9:
					GPIO_SetBits(GPIOD, GPIO_Pin_3);
					break;
				case 10:
					GPIO_SetBits(GPIOD, GPIO_Pin_2);
					break;
				case 11:
					GPIO_SetBits(GPIOD, GPIO_Pin_1);
					break;
				case 12:
					GPIO_SetBits(GPIOD, GPIO_Pin_0);
					break;
				default:
					GPIO_SetBits(GPIOD, GPIO_Pin_6);
					break;
			}
			led++;
			if (led == 12) {
				led = 0;
			}
		}
		if (delay==0) {
			osDelay(100);
		} else {
			osDelay(LED_FLASH_DELAY_SCALING*delay);
		}
	}
}

void TIM3_IRQHandler(void) {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)			//Checks interrupt status register to ensure an interrupt is pending
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);						//Reset interrupt pending bit
		osSignalSet(*tid_thread_receiver, 0x0001);								//send signal to keypad thread
  }
}

/**
  * @brief  IRQ handler for EXTI0
	* @param  None
  * @retval None
  */

void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)								//Checks interrupt status register to ensure an interrupt is pending
  {
    EXTI_ClearITPendingBit(EXTI_Line0);										//Reset interrupt pending bit
		osSignalSet(*tid_thread_pushbutton, 0x0001);
  }
}
