/*!
	@file project_init.h
	Header file that supports project_init.c source file.
*/
void init_LEDS_PWM(void);
void init_LEDS(void);

#define LED_FLASH_DELAY_SCALING 20

#define TIM3_FREQUENCY 5

#define Y_MIN				0
#define Y_MAX				6
#define Z_MIN				0
#define Z_MAX				1
#define	ANGLE_MIN		0
#define ANGLE_MAX		13

#define PUSHBUTTON_GPIO GPIOA
#define PUSHBUTTON_PIN GPIO_Pin_0

#define ARMTHREAD_LOOP_DELAY 100
#define RANDOM_MODE_DELAY 500

void init_userButton(void);
void init_TIM3(void);
