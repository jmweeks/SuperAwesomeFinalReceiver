/*!
	@file project_receiver.h
	Header file that supports @ref project_receiver.c
*/
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "CC2500.h"

#define RECEIVE_DATA_SIZE 5

#ifndef RECEIVER_STRUCT
#define RECEIVER_STRUCT

/**  
  * @brief  Structure representing receiver.   
  */
struct Receiver {
	uint8_t state;
	uint8_t buffer_space;
	uint8_t data[RECEIVE_DATA_SIZE];
	osMutexId mutexID;
	osThreadId threadID;
};

#endif

void init_receiver(struct Receiver *receiver, osThreadId **threadID);
void currentData(struct Receiver *receiver, uint8_t data[]);
void receiverThread(void const *argument);
