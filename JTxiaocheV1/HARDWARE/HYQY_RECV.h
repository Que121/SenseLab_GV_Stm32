#ifndef _HYQY_RECV_H
#define _HYQY_RECV_H

#include "main.h"

/**
 * ʹ�÷������£�
 * �������ⳤ��С��RECV_BUFFER_MAX�������������ݼ���Ӣ�Ķ��Ÿ���
 * ע�Ᵽ֤��֡Ϊ��
 *          ������1��������2 + ����1, + ����2 , +...+����n, + ����β1 + ����β2
 * ����(char)��pp12,666,266,1234,\r\n
 */

#define RECV_BUFFER_MAX     64              //���ջ�������С
#define RECV_BEG1           0x70            //������1   'p'
#define RECV_BEG2           0x70            //������2   'p'

#define RECV_END1           0x0D            //����β1   '\r'
#define RECV_END2           0x0A            //����β2   '\n'


typedef struct 
{
    uint8_t     UART_id;                    //��¼���ڼ���
    uint8_t     recvBuff[RECV_BUFFER_MAX];
    uint16_t    recvNum;                    //��¼�����˼���������
    uint16_t    recvPin_Now;                //ָ�룬ָ��ǰ����λ
    uint8_t     findBegin;                  //����ҵ�ͷ����1
    uint8_t     findEnd;                    //����ҵ�β����1��ͷ��0
    uint8_t     DataIsOK;                   //���ݽ�����ɣ��������
    uint32_t    recvRes[10];                //���յĳ�Ʒ����
    uint8_t     LastRecv;                   //��¼�ϴν��յ��ַ�
}HYQY_RECV_Typedef;

extern HYQY_RECV_Typedef HYQY_recvInfo;  

/* ��ʼ������ */
void HYQY_Recv_Init(HYQY_RECV_Typedef* recv_Type, uint8_t UART_id);

/* ��������ж��ڣ����ڽ���һ���ַ� */
void HYQY_Recv_Handle(HYQY_RECV_Typedef* recv_Type,uint8_t chr);

/* �Ի��������ݽ��� */
void HYQY_DeBuf(HYQY_RECV_Typedef* recv_Type,uint8_t* buf,uint16_t num);

/* �жϽ����Ƿ���� */
uint8_t IsReceiveDataOK(HYQY_RECV_Typedef* recv_Type);

/* չʾ������ɵ���Ϣ */
void ShowDecodeResult(HYQY_RECV_Typedef* recv_Type);

/* ����demo�� */
void undefinedReceiveShowDemo_Handle(void);
void undefinedReceive_LED_Demo_Handle(void);

#endif      //_HYQY_RECV_H
