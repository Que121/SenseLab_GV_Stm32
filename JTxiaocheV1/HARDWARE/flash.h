#ifndef __FLASH_OPERATIONS_H
#define __FLASH_OPERATIONS_H

#include "stm32f1xx_hal.h"

// 保存数据到Flash的最后一页
void SaveDataToLastPage(uint32_t* data, uint32_t dataSize);

// 从Flash中读取数据
void ReadDataFromLastPage(uint32_t* data, uint32_t dataSize);

void TestFlash(void);

#endif // __FLASH_OPERATIONS_H


