/**
 * @file ring.c
 * @author Kacper Brzostowski (kapibrv97@gmail.com)
 * @link https://github.com/magiczny-kacper
 * @brief Ring buffer library source file.
 * @version 1.1.0
 * @date 2020-10-04
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/**
 * @copyright GNU General Public License v3.0
 * @addtogroup Ring_Buffer
 * @{
 */
#include <stdint.h>
#include <string.h>
#include "ring.h"

/**< Modulo for operations on array indexes. */
#define MODULO_BUF(value, max) ((value) % (max))

uint32_t RingGetMaxSize (RingBuffer_t* buffer){
	return buffer -> size;
}

uint32_t RingGetSpace (RingBuffer_t* buffer){
	return buffer -> place;
}

uint32_t RingGetDataCnt (RingBuffer_t* buffer){
	return buffer -> size - RingGetSpace(buffer);
}

/* TODO: Add null pointer exceptions. */
RingStatus_t RingInit (RingBuffer_t* buffer
#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
		, uint8_t* arrayBuffer, uint32_t bufferSize
#endif
){
	if(buffer == NULL) return NO_PTR;
	memset(buffer, 0, sizeof(RingBuffer_t));
#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
	if(arrayBuffer == NULL) return NO_PTR;
	if(bufferSize <= 0) return NO_DATA;
	buffer -> buffer = arrayBuffer;
	buffer -> size = bufferSize;
	memset(buffer -> buffer, 0, buffer -> size);
#elif RING_BUFFER_MODE == RING_BUFFER_ARRAYS
	buffer -> size = BUFFER_SIZE;
#endif
	buffer -> place = buffer -> size - 1;
	buffer -> writePtr = 0;
	buffer -> readPtr = 0;
	return OK;
}

RingStatus_t RingWriteByte (RingBuffer_t* buffer, uint8_t data){
	RingStatus_t retval = OK;
	if(buffer == NULL) return NO_PTR;
	uint32_t tempHead = buffer -> writePtr;
	uint32_t tempTail = buffer -> readPtr;

#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
	uint8_t* wrPtr;
	if(buffer -> buffer == NULL) return NO_PTR;
	wrPtr = buffer -> buffer + tempHead;
#endif

	tempHead = MODULO_BUF(tempHead + 1, buffer -> size);
	if(tempHead != tempTail){

#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
		*wrPtr = data;
#elif RING_BUFFER_MODE == RING_BUFFER_ARRAYS
		buffer -> buffer[tempHead - 1] = data;
#endif

		buffer -> writePtr = tempHead;
		buffer -> place --;
	}else{
		retval = NO_PLACE;
	}
	return retval;
}

RingStatus_t RingWriteMultipleBytes (RingBuffer_t* buffer, uint8_t* data, uint32_t len){
	RingStatus_t retval = OK;
	if(buffer == NULL) return NO_PTR;
	if(data == NULL) return NO_PTR;
	if(len <= 0) return NO_DATA;
	uint32_t tempHead = buffer -> writePtr;
	uint32_t tempPlace = buffer -> place;

#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
	uint8_t* wrPtr;
	if(buffer -> buffer == NULL) return NO_PTR;
	wrPtr = buffer -> buffer + tempHead;
#endif

	if(tempPlace >= len){
		if(tempHead + len >= buffer -> size){
			uint32_t temp1, temp2;
#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
			temp1 = buffer -> size - tempHead;
			memcpy(wrPtr, data, temp1);
			data += temp1;
			temp2 = len - temp1;
			wrPtr = buffer -> buffer;
			memcpy(wrPtr, data, temp2);
#elif RING_BUFFER_MODE == RING_BUFFER_ARRAYS
			temp1 = buffer -> size - tempHead;
			memcpy(&buffer -> buffer[tempHead], data, temp1);
			data += temp1;
			temp2 = len - temp1;
			memcpy(&buffer -> buffer, data, temp2);
			tempHead = temp2;
#endif	
		}else{

#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
			memcpy(wrPtr, data, len);
#elif RING_BUFFER_MODE == RING_BUFFER_ARRAYS	
			memcpy(&buffer -> buffer[tempHead], data, len);
#endif
			tempHead = MODULO_BUF(tempHead + len, buffer -> size);
		}
		tempPlace -= len;
		buffer -> writePtr = tempHead;
		buffer -> place = tempPlace;
	}else{
		retval = NO_PLACE;
	}

	return retval;
}

RingStatus_t RingReadByte (RingBuffer_t* buffer, uint8_t* data){
	RingStatus_t retval = OK;
	uint32_t tempHead = buffer -> writePtr;
	uint32_t tempTail = buffer -> readPtr;
	uint32_t tempPlace = buffer -> place;
#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
	uint8_t* wrPtr;
	wrPtr = buffer -> buffer + tempTail;
#endif

	if(tempHead != tempTail){
#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
		*data = *wrPtr;
#elif RING_BUFFER_MODE == RING_BUFFER_ARRAYS
		*data = buffer -> buffer[tempTail];
#endif
		tempTail = MODULO_BUF(tempTail + 1, buffer -> size);
		tempPlace++;
		buffer -> readPtr = tempTail;
		buffer -> place = tempPlace;
	}else{
		retval = NO_DATA;
	}
	return retval;
}

RingStatus_t RingReadMultipleBytes (RingBuffer_t* buffer, uint8_t* data, uint32_t len){
	RingStatus_t retval = OK;
	uint32_t tempHead = buffer -> writePtr;
	uint32_t tempTail = buffer -> readPtr;
	uint32_t tempPlace = buffer -> place;
#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
	uint8_t* wrPtr;
	wrPtr = buffer -> buffer + tempHead;
#endif
	if(tempHead != tempTail){
		if(tempTail + len > buffer -> size - 1){
			uint32_t temp1, temp2;
#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
			temp1 = buffer -> size - tempTail;
			memcpy(data, wrPtr, temp1);
			data += temp1;
			temp2 = len - temp1;
			wrPtr = buffer -> buffer;
			memcpy(data, wrPtr, temp2);
			tempTail = temp2;
#elif RING_BUFFER_MODE == RING_BUFFER_ARRAYS
			temp1 = buffer -> size - tempTail;
			memcpy(data, &buffer -> buffer[tempTail], temp1);
			data += temp1;
			temp2 = len - temp1;
			memcpy(data, buffer -> buffer, temp2);
			tempTail = temp2;
#endif
		}else{
#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
			memcpy(data, wrPtr, len);
#elif RING_BUFFER_MODE == RING_BUFFER_ARRAYS
			memcpy(data, &buffer -> buffer[tempTail], len);
#endif
			tempTail += len;
		}
		tempPlace += len;
		buffer -> readPtr = tempTail;
		buffer -> place = tempPlace;
	}else{
		retval = NO_DATA;
	}
	return retval;
}

uint32_t RingGetHead (RingBuffer_t* buffer){
	return buffer -> writePtr;
}

uint32_t RingGetTail (RingBuffer_t* buffer){
	return buffer -> readPtr;
}

uint32_t RingGetLastElement(RingBuffer_t* buffer){
#if RING_BUFFER_MODE == RING_BUFFER_POINTERS
	uint8_t data;
	data = *(buffer -> buffer + buffer -> writePtr);
	return data;
#elif RING_BUFFER_MODE == RING_BUFFER_ARRAYS
	return (buffer -> buffer [buffer -> writePtr]);
#endif
}

/**
 * @}
 * 
 */
