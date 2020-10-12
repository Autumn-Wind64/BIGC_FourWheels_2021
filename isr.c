//////////////////////////////////////////
////         ���ݺ���ΰҵ      ///////////
///   http://shop36538723.taobao.com /////
///           2015-4-10               ///
//////////////////////////////////////////


#include "common.h"
#include "include.h" 

extern void run();

extern u32 rowCnt ;   //�м���
extern u8 Buffer1[ROW][COL];
u8 SampleFlag = 0;
extern u8 VSYN_Flag;
extern uint8 imgbuff_process[CAMERA_SIZE];
extern s16 leftspeed,rightspeed;

u8 TIME1flag_100ms = 0 ;
/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�USART0_IRQHandler
*  ����˵��������1 �ж� ���� ������
*  ����˵������
*  �������أ���
*
*************************************************************************/
void USART0_IRQHandler(void)
{
    uint8 ch;
    //DisableInterrupts;		    //�����ж�
    //����һ���ֽ����ݲ��ط�
    ch = uart_getchar (UART0);      //���յ�һ������
    //EnableInterrupts;		    //�����ж�
    //uart_putchar(UART0,'A'); 
}

/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�PIT0_IRQHandler
*  ����˵����PIT0 ��ʱ�жϷ�����
*  ����˵������
*  �������أ���
*
*************************************************************************/
void PIT0_IRQHandler(void) //5ms
{
    PIT_Flag_Clear(PIT0);       //���жϱ�־λ
    run();   //���ٺ���
}

void PIT1_IRQHandler()           //PIT1 ��ʱ�жϷ�����
{  
	PIT_Flag_Clear(PIT1);       //���жϱ�־λ
	biZhang();                 //���ϳ���
}

/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�SysTick_Handler
*  ����˵����ϵͳ�δ�ʱ���жϷ�����
*  ����˵������
*  �������أ���
*
*************************************************************************/
void SysTick_Handler(void)
{
    //    OSIntEnter();
    //    OSTimeTick();
    //    OSIntExit();
}


/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�HardFault_Handler
*  ����˵����Ӳ���Ϸ��жϷ�����
*  ����˵������
*  �������أ���
*
*************************************************************************/
void HardFault_Handler(void)
{
    while (1)
    {
        printf("\n****Ӳ���Ϸô���!!!*****\r\n\n");
    }
}

/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�PendSV_Handler
*  ����˵����PendSV��������ϵͳ���ã��жϷ�����
*  ����˵������
*  �������أ���
*
*************************************************************************/
void PendSV_Handler(void)
{
}



/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�PORTA_IRQHandler
*  ����˵����PORTA�˿��жϷ�����
*  ����˵������
*  �������أ���
*  ��    ע�����ź���Ҫ�Լ���ʼ�������
*
*************************************************************************/


void PORTA_IRQHandler()  
{   
    //---VSYN���жϴ���
    if(PORTA_ISFR & (1 << 27))         //PTA27�����ж�  ���ж�
    { 
        PORTA_ISFR  |= (1 << 27);       //д1���жϱ�־λ
        //rowCnt = 0;
        //SampleFlag = 1;
//        DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTD_BYTE0_IN, Buffer1, PTC19, DMA_BYTE1, CAMERA_SIZE, DMA_rising_down );//DMA_falling_down  DMA_rising_down
//        DMA_EN(DMA_CH4); 
          
//        for(lie=0;lie<10;lie++)   //130  ��ʱ�޸ģ����Ե���ͼ�������
//        {
//           asm("nop");
//           asm("nop");
//        }
//        //DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTD_BYTE0_IN, Buffer1, PTC19, DMA_BYTE1, CAMERA_SIZE, DMA_falling_down_keepon);
//        DMA_DADDR(DMA_CH0) = (uint32)Buffer1;    //�ָ���ַ
//        DMA_EN(DMA_CH4);   
    }  
}

/*************************************************************************
*              
*
*  �������ƣ�PORTB_IRQHandler
*  ����˵����PORTB�˿��жϷ�����
*  ����˵������
*  �������أ���
*  ��    ע�����ź���Ҫ�����Լ���ʼ�����޸�
*
*************************************************************************/
void PORTB_IRQHandler()
{   unsigned int lie; 
    if( PORTB_ISFR & (1 << 22))         //PTB23�����ж�  
    {
        PORTB_ISFR  |= (1 << 22);        //д1���жϱ�־λ
//        DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTD_BYTE0_IN, Buffer1, PTC19, DMA_BYTE1, CAMERA_SIZE, DMA_falling_down );//DMA_falling_down  DMA_rising_down
//        DMA_EN(DMA_CH4); 
    }
}

/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�PORTD_IRQHandler
*  ����˵����PORTA�˿��жϷ�����
*  ����˵������
*  �������أ���
*  ��    ע�����ź���Ҫ�Լ���ʼ�������
*
*************************************************************************/


void PORTC_IRQHandler()  
{   unsigned int lie; 
    //---VSYN���жϴ���
    if(PORTC_ISFR & (1 << 15))         //PTD8�����ж�  ���ж�
    { 
        PORTC_ISFR  |= (1 << 15);       //д1���жϱ�־λ
        DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTD_BYTE0_IN, Buffer1, PTC13, DMA_BYTE1, CAMERA_SIZE, DMA_falling_down );//DMA_falling_down  DMA_rising_down
        DMA_EN(DMA_CH4); 
    }  
}

/*************************************************************************
*                            �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�DMA_CH4_Handler
*  ����˵����DMAͨ��4���жϷ�����
*  ����˵������
*  �������أ���
*
*************************************************************************/
void DMA_CH4_Handler(void)
{
    DMA_IRQ_CLEAN(DMA_CH4);                             //���ͨ�������жϱ�־λ    (���������ٴν����ж�)
    DMA_IRQ_DIS(DMA_CH4);                               //��ֹDMA   �ȴ���һ�����ж����ٿ���DMA
    DisableInterrupts;
    //rowCnt = 0;
    DMA_DIS(DMA_CH4);
    //disable_irq(90); //�ر�PORTD�жϣ��رճ��ж���Ӧ
    VSYN_Flag = 1;
}

/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�FTM0_IRQHandler
*  ����˵����FTM0���벶׽�жϷ�����
*  ����˵������
*  �������أ���
*  ��    ע�����ź���Ҫ�����Լ���ʼ�����޸ģ��ο����еĴ�������Լ��Ĺ���
*
*************************************************************************/
void FTM0_IRQHandler()
{


}
/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�FTM2_IRQHandler
*  ����˵����FTM2���벶׽�жϷ�����
*  ����˵������
*  �������أ���
*  ��    ע�����ź���Ҫ�����Լ���ʼ�����޸ģ��ο����еĴ�������Լ��Ĺ���
*
*************************************************************************/
void FTM2_IN_IRQHandler()
{  unsigned char curstatus = FTM2_STATUS;
   FTM2_STATUS = 0x00;  //���ж�
//   if(curstatus&(1<<0))  //CH0�ж�,�����
//   { leftspeed++;
//   }
//   if(curstatus&(1<<1))  //CH1�жϣ��Ҳ���
//   { rightspeed++;
//   }
}
/*************************************************************************
*                             �Ĭ�Ƽ�DEMOK Kinetis����С��
*
*  �������ƣ�FTM1_IRQHandler
*  ����˵����FTM1���벶׽�жϷ�����
*  ����˵������
*  �������أ���
*  ��    ע�����ź���Ҫ�����Լ���ʼ�����޸ģ��ο����еĴ�������Լ��Ĺ���
*
*************************************************************************/
void FTM1_IRQHandler()
{
   unsigned char curstatus = FTM1_STATUS;
   FTM1_STATUS = 0x00;  //���ж�
   if(curstatus&(1<<0))  //CH0�ж�,�����
   { leftspeed++;
   }
   if(curstatus&(1<<1))  //CH1�жϣ��Ҳ���
   { rightspeed++;
   }
}
