/*!
	@file project_receiver.c
	Source file for receiver initialization.
*/
#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_receiver.h"	
#include "CC2500.h"

/**  
  * @brief  Function that initializes receiver. 
  * @param  *receiver: pointer to Receiver structure.
  * @retval None  
  */
void init_receiver(struct Receiver *receiver, osThreadId **tid_thread_receiver) {
	for (uint32_t i=0; i<sizeof(receiver->data)/sizeof(receiver->data[0]); i++) {
		receiver->data[i] = 0;
	}
	receiver->state = 0;
	receiver->buffer_space = 0;
	
	CC2500_Init();
	CC2500_config_transmitter();
	goToTX(&(receiver->state), &(receiver->buffer_space));
	
	osThreadDef(receiverThread, osPriorityNormal, 1, 0);
	receiver->threadID = osThreadCreate(osThread(receiverThread), receiver);
	
	osMutexDef(receiverMutex);
	receiver->mutexID=osMutexCreate(osMutex(receiverMutex));
	
	*tid_thread_receiver = &(receiver->threadID);
}

/**  
  * @brief  Function that fetches the newest data received from transmitter.
  * @param  *receiver: pointer to Receiver structure.
  * @param  *data: array that stores newest data.
  * @retval None  
  */
void currentData(struct Receiver *receiver, uint8_t data[]) {
	osMutexWait(receiver->mutexID, osWaitForever);
	for(uint32_t i=0; i<sizeof(receiver->data)/sizeof(receiver->data[0]); i++) {
		data[i] = receiver->data[i];
	}
	osMutexRelease(receiver->mutexID);
}

/**  
  * @brief  Thread that waits periodically for transmitted data.
  * @param  *argument: pointer to a Receiver structure.
  * @retval None  
  */
void receiverThread(void const *argument) {
	struct Receiver *receiver;
	receiver = (struct Receiver *)argument;
	
	while(1) {
		osSignalWait(0x0001, osWaitForever);
		wireless_RX(receiver);
	}
}
