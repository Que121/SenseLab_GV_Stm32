#include "main.h"
#include "flash.h"

#include "stdio.h"

// Flash���һҳ����ʼ��ַ
#define FLASH_LAST_PAGE_ADDRESS ((uint32_t)0x0807B800)
// Flashҳ��С��STM32F103��ÿҳΪ1KB��
//#define FLASH_PAGE_SIZE ((uint32_t)0x400)

// �Զ��庯�����������ݵ�Flash�����һҳ
void SaveDataToLastPage(uint32_t* data, uint32_t dataSize) {
     HAL_FLASH_Unlock();
    
    uint32_t address = FLASH_LAST_PAGE_ADDRESS;
    uint32_t pageError = 0; // ���ڴ洢ҳ����

    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef eraseInit;
    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = FLASH_LAST_PAGE_ADDRESS;
    eraseInit.NbPages = 1;

    // ����Flashҳ
    status = HAL_FLASHEx_Erase(&eraseInit, &pageError);
    if (status != HAL_OK) {
        HAL_FLASH_Lock();
        // ʧ�ܴ���
        return;
    }
    
    for (uint32_t i = 0; i < dataSize; i++) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data[i]);
        address += 4; // ÿ������4�ֽ�
        if (status != HAL_OK) {
            HAL_FLASH_Lock();
            // ʧ�ܴ���
          printf("flash write error\r\n");
            return;
        }
    }
    
    HAL_FLASH_Lock();
    
    HAL_FLASH_Lock();
    printf("flash write finish\r\n");
    // �ɹ�����
}

// �Զ��庯������Flash�ж�ȡ����
void ReadDataFromLastPage(uint32_t* data, uint32_t dataSize) {
    uint32_t address = FLASH_LAST_PAGE_ADDRESS;
    
    for (uint32_t i = 0; i < dataSize; i++) {
        data[i] = *(__IO uint32_t*)address;
        address += 4; // ÿ������4�ֽ�
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

    // �������ݵ�Flash�����һҳ
    SaveDataToLastPage(testData, sizeof(testData) / sizeof(testData[0]));

    // ��Flash�ж�ȡ����
    
    ReadDataFromLastPage(readData, sizeof(readData) / sizeof(readData[0]));

    // �Ƚ϶�ȡ��������ԭʼ����
    
    for (int i = 0; i < 100; i++) {
        if (readData[i] != testData[i]) {
            isEqual = 0;
            break;
        }
    }
    
    if (isEqual) {
        printf("��ȡ������ԭʼ�������\n");
    } else {
        printf("��ȡ������ԭʼ���ݲ����\n");
    }
}
