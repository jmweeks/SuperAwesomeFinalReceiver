/*!
	@file project_magnet.c
	Source file for the magnet initialization and functionality.
*/
#include "stm32f4xx.h"

#include "project_magnet.h"

/**  
  * @brief  Function that initializes the electromagnet.
  * @param  *magnet: pointer to Magnet structure.  
	* @param  *magnetInit: pointer to MagnetInit structure.
  * @retval None  
  */
void init_magnet(struct Magnet *magnet, struct MagnetInit *magnetInit) {
	magnet->GPIO = magnetInit->GPIOx;
	magnet->periph = magnetInit->periph;
	magnet->pin = magnetInit->pin;
	magnet->isOn = 0;
	
	RCC_AHB1PeriphClockCmd(magnet->periph, ENABLE);																			//Enable clock to GPIOD
	
	GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = magnet->pin;			//Specify which LEDs, on which pins
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																							//Aternate function mode to work with TIM4
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;																					//Set speed
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																						//Push-pull
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;																					//No pull - output
  GPIO_Init(magnet->GPIO, &GPIO_InitStructure);																										//Pass struct and initialize
}

/**  
  * @brief  Function that turn the magnet on.
  * @param  *magnet: pointer to Magnet structure.  
  * @retval None  
  */
void turnMagnetOn(struct Magnet *magnet) {
	GPIO_SetBits(magnet->GPIO, magnet->pin);
	magnet->isOn = 1;
}

/**  
  * @brief  Function that turn the magnet off.
  * @param  *magnet: pointer to Magnet structure.  
  * @retval None  
  */
void turnMagnetOff(struct Magnet *magnet) {
	GPIO_ResetBits(magnet->GPIO, magnet->pin);
	magnet->isOn = 0;
}

/**  
  * @brief  Function that toggles the magnet between on/off states.
  * @param  *magnet: pointer to Magnet structure.  
  * @retval None  
  */
void toggleMagnet(struct Magnet *magnet) {
	if (magnet->isOn) {
		turnMagnetOff(magnet);
	} else {
		turnMagnetOn(magnet);
	}
}
