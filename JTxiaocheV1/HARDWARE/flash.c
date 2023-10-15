#include "main.h"
#include "flash.h"

#include "stdio.h"

// Flash最后一页的起始地址
#define FLASH_LAST_PAGE_ADDRESS ((uint32_t)0x0807B800)
// Flash页大小（STM32F103中每页为1KB）
//#define FLASH_PAGE_SIZE ((uint32_t)0x400)

// 自定义函数，保存数据到Flash的最后一页
void SaveDataToLastPage(uint32_t* data, uint32_t dataSize) {
     HAL_FLASH_Unlock();
    
    uint32_t address = FLASH_LAST_PAGE_ADDRESS;
    uint32_t pageError = 0; // 用于存储页错误

    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef eraseInit;
    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = FLASH_LAST_PAGE_ADDRESS;
    eraseInit.NbPages = 1;

    // 擦除Flash页
    status = HAL_FLASHEx_Erase(&eraseInit, &pageError);
    if (status != HAL_OK) {
        HAL_FLASH_Lock();
        // 失败处理
        return;
    }
    
    for (uint32_t i = 0; i < dataSize; i++) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data[i]);
        address += 4; // 每个字是4字节
        if (status != HAL_OK) {
            HAL_FLASH_Lock();
            // 失败处理
          printf("flash write error\r\n");
            return;
        }
    }
    
    HAL_FLASH_Lock();
    
    HAL_FLASH_Lock();
    printf("flash write finish\r\n");
    // 成功处理
}

// 自定义函数，从Flash中读取数据
void ReadDataFromLastPage(uint32_t* data, uint32_t dataSize) {
    uint32_t address = FLASH_LAST_PAGE_ADDRESS;
    
    for (uint32_t i = 0; i < dataSize; i++) {
        data[i] = *(__IO uint32_t*)address;
        address += 4; // 每个字是4字节
    }
}

uint32_t testData[100] = {0};
uint32_t readData[100] = {0};
int isEqual = 1;
void TestFlash(void)
{
    for (int i = 0; i < 100; i++) {
        testData[i] = i * 15;
    }

    // 保存数据到Flash的最后一页
    SaveDataToLastPage(testData, sizeof(testData) / sizeof(testData[0]));

    // 从Flash中读取数据
    
    ReadDataFromLastPage(readData, sizeof(readData) / sizeof(readData[0]));

    // 比较读取的数据与原始数据
    
    for (int i = 0; i < 100; i++) {
        if (readData[i] != testData[i]) {
            isEqual = 0;
            break;
        }
    }
    
    if (isEqual) {
        printf("读取数据与原始数据相等\n");
    } else {
        printf("读取数据与原始数据不相等\n");
    }
}
