#include "stm32f4xx.h"

struct Magnet {
	GPIO_TypeDef *GPIO;
	uint32_t periph;
	uint16_t pin;
	uint32_t isOn;
};

struct MagnetInit {
	GPIO_TypeDef *GPIOx;
	uint32_t periph;
	uint16_t pin;
};

void init_magnet(struct Magnet *magnet, struct MagnetInit *magnetInit);
void turnMagnetOn(struct Magnet *magnet);
void turnMagnetOff(struct Magnet *magnet);
void toggleMagnet(struct Magnet *magnet);
