#include "stm32f4xx.h"
#include "cmsis_os.h"

#include "project_receiver.h"	
#include "CC2500.h"

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

void currentData(struct Receiver *receiver, uint8_t data[]) {
	osMutexWait(receiver->mutexID, osWaitForever);
	for(uint32_t i=0; i<sizeof(receiver->data)/sizeof(receiver->data[0]); i++) {
		data[i] = receiver->data[i];
	}
	osMutexRelease(receiver->mutexID);
}

void receiverThread(void const *argument) {
	struct Receiver *receiver;
	receiver = (struct Receiver *)argument;
	
	while(1) {
		osSignalWait(0x0001, osWaitForever);
		wireless_RX(receiver);
	}
}
