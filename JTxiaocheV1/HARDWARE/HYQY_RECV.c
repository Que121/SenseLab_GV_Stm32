#include "HYQY_RECV.h"
#include "main.h"
#include "usart.h"
#include "stdio.h"                                      //ʹ�����еĵ��Թ���

HYQY_RECV_Typedef HYQY_recvInfo;
uint8_t UartRecvBuff = 0;

void HYQY_Recv_Init(HYQY_RECV_Typedef* recv_Type, uint8_t UART_id)
{
    uint16_t i = 0;
    for(; i < RECV_BUFFER_MAX; i++)
    {
        recv_Type->recvBuff[i] = 0;                     //��ջ�����  
    }
    recv_Type->UART_id = UART_id;
    recv_Type->recvNum = 0;                             //��ս��յ��ֽڸ���
    recv_Type->recvPin_Now = 0;
    recv_Type->findBegin = 0;
    recv_Type->findEnd = 0;
    recv_Type->DataIsOK = 0;
    recv_Type->LastRecv = 0;
    HAL_UART_Receive_IT(&huart1,&UartRecvBuff,1);
}
void HYQY_Recv_Handle(HYQY_RECV_Typedef* recv_Type,uint8_t chr)
{
    //�ҵ�ͷ
    if((recv_Type->LastRecv == RECV_BEG1) && (chr == RECV_BEG2))   
    {
        recv_Type->findBegin = 1;                       //��־λ��1
        recv_Type->LastRecv = 0;                        //����ϴμ�¼
        return;                                         //ֱ�ӽ�������
    }
    if(recv_Type->findBegin)                            //�Ѿ�����ͷ
    {                                                   //������浽������
        recv_Type->recvBuff[recv_Type->recvPin_Now++] = chr;    
        if((recv_Type->LastRecv == RECV_END1) && (chr == RECV_END2))
        {
            recv_Type->findBegin = 0;                   //�ҵ�β����0
            recv_Type->findEnd   = 1;                   //�ҵ�β����1
            recv_Type->LastRecv  = 0;                   //����ϴεļ�¼
        }
        else recv_Type->LastRecv = chr;                 //��¼�ϸ��ַ�
    }
    if(recv_Type->findEnd)                              //�Ѿ�����β�ͣ�֤��������
    {
        //���ݽ���
        HYQY_DeBuf(recv_Type, recv_Type->recvBuff, recv_Type->recvPin_Now - 2);
        //��ձ�־λ��
        recv_Type->findEnd = 0;
        recv_Type->recvPin_Now = 0;
    }
    else recv_Type->LastRecv = chr;
}

void HYQY_DeBuf(HYQY_RECV_Typedef* recv_Type,uint8_t* buf,uint16_t num)
{
    uint8_t NumPin = 0;                                 //�洢�ڼ������ݣ�
    uint8_t i = 0;
    for(i = 0;i < recv_Type->recvNum + 1; i++)
        recv_Type->recvRes[i] = 0;
    for(i = 0;i < num; i++)
    {
        if(buf[i] != ',')
        {
            recv_Type->recvRes[NumPin] *= 10;
            recv_Type->recvRes[NumPin] += buf[i] - '0';
        }
        else NumPin ++;
    }
    recv_Type->recvNum = NumPin;                        //һ���ҵ�����ô�������
    recv_Type->DataIsOK = 1;
    //printf("-----Decode--finish-----\r\n");
}

uint8_t IsReceiveDataOK(HYQY_RECV_Typedef* recv_Type)   //�жϽ����Ƿ����
{
    if(recv_Type->DataIsOK)
    {
        recv_Type->DataIsOK = 0;
        return 1;
    }
    return 0;
}

void ShowDecodeResult(HYQY_RECV_Typedef* recv_Type)     //չʾ������Ϣ
{
    uint8_t i = 0;
    printf("\n UART %d decode %d datas:\r\n", recv_Type->UART_id, recv_Type->recvNum);
    for(i = 0; i < recv_Type->recvNum; i++)
    {
        printf("recvRes[%d]:%d\r\n", i, recv_Type->recvRes[i]);
    }
    //printf("-----finish-----\r\n");
}

//void undefinedReceiveShowDemo_Handle(void)
//{
//    YQYlib_Init();

//    UART0_Init();                           //����0 �����շ�����
//    UART1_Init();                           //����1 ���ڵ���
//    HYQY_Recv_Init(&HYQY_recvInfo[0],0);    //��ʼ�����������չ���
//    HYQY_Recv_Init(&HYQY_recvInfo[1],1);    //��ʼ�����������չ���

//    printf("from YQY :start!\n");           //��ӡ��ӭ��Ϣ
//    while(1)                                //��ѭ����������յ����ݣ������ӡ
//    {
//        if(IsReceiveDataOK(&HYQY_recvInfo[0]))
//            ShowDecodeResult(&HYQY_recvInfo[0]);
//        if(IsReceiveDataOK(&HYQY_recvInfo[1]))
//            ShowDecodeResult(&HYQY_recvInfo[1]);
//    }
//}

//void undefinedReceive_LED_Demo_Handle(void)
//{
//    YQYlib_Init();

//    UART1_Init();                           //����1 ���ڵ���
//    HYQY_Recv_Init(&HYQY_recvInfo[1],1);    //��ʼ�����������չ���
//    ledInit();                              //��ʼ��LED

//    printf("from YQY :start!\n");           //��ӡ��ӭ��Ϣ
//    while(1)                                //��ѭ����������յ����ݣ������ӡ
//    {
//        if(HYQY_recvInfo[1].recvRes[0] == 10)
//            ledOut(HIGH,HIGH);
//        if(HYQY_recvInfo[1].recvRes[0] == 11)
//            ledOut(LOW,LOW);
//    }
//}
