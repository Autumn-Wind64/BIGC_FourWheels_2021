//////////////////////////////////////////
////         杭州红树伟业      ///////////
///   http://shop36538723.taobao.com /////
///           2015-4-10               ///
//////////////////////////////////////////


#include "common.h"
#include "include.h" 

extern void run();

extern u32 rowCnt ;   //行计数
extern u8 Buffer1[ROW][COL];
u8 SampleFlag = 0;
extern u8 VSYN_Flag;
extern uint8 imgbuff_process[CAMERA_SIZE];
extern s16 leftspeed,rightspeed;

u8 TIME1flag_100ms = 0 ;
/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：USART0_IRQHandler
*  功能说明：串口1 中断 接收 服务函数
*  参数说明：无
*  函数返回：无
*
*************************************************************************/
void USART0_IRQHandler(void)
{
    uint8 ch;
    //DisableInterrupts;		    //关总中断
    //接收一个字节数据并回发
    ch = uart_getchar (UART0);      //接收到一个数据
    //EnableInterrupts;		    //开总中断
    //uart_putchar(UART0,'A'); 
}

/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：PIT0_IRQHandler
*  功能说明：PIT0 定时中断服务函数
*  参数说明：无
*  函数返回：无
*
*************************************************************************/
void PIT0_IRQHandler(void) //5ms
{
    PIT_Flag_Clear(PIT0);       //清中断标志位
    run();   //测速函数
}

void PIT1_IRQHandler()           //PIT1 定时中断服务函数
{  
	PIT_Flag_Clear(PIT1);       //清中断标志位
	biZhang();                 //避障程序
}

/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：SysTick_Handler
*  功能说明：系统滴答定时器中断服务函数
*  参数说明：无
*  函数返回：无
*
*************************************************************************/
void SysTick_Handler(void)
{
    //    OSIntEnter();
    //    OSTimeTick();
    //    OSIntExit();
}


/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：HardFault_Handler
*  功能说明：硬件上访中断服务函数
*  参数说明：无
*  函数返回：无
*
*************************************************************************/
void HardFault_Handler(void)
{
    while (1)
    {
        printf("\n****硬件上访错误!!!*****\r\n\n");
    }
}

/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：PendSV_Handler
*  功能说明：PendSV（可悬起系统调用）中断服务函数
*  参数说明：无
*  函数返回：无
*
*************************************************************************/
void PendSV_Handler(void)
{
}



/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：PORTA_IRQHandler
*  功能说明：PORTA端口中断服务函数
*  参数说明：无
*  函数返回：无
*  备    注：引脚号需要自己初始化来清除
*
*************************************************************************/


void PORTA_IRQHandler()  
{   
    //---VSYN场中断处理
    if(PORTA_ISFR & (1 << 27))         //PTA27触发中断  场中断
    { 
        PORTA_ISFR  |= (1 << 27);       //写1清中断标志位
        //rowCnt = 0;
        //SampleFlag = 1;
//        DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTD_BYTE0_IN, Buffer1, PTC19, DMA_BYTE1, CAMERA_SIZE, DMA_rising_down );//DMA_falling_down  DMA_rising_down
//        DMA_EN(DMA_CH4); 
          
//        for(lie=0;lie<10;lie++)   //130  延时修改，可以调节图像的左右
//        {
//           asm("nop");
//           asm("nop");
//        }
//        //DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTD_BYTE0_IN, Buffer1, PTC19, DMA_BYTE1, CAMERA_SIZE, DMA_falling_down_keepon);
//        DMA_DADDR(DMA_CH0) = (uint32)Buffer1;    //恢复地址
//        DMA_EN(DMA_CH4);   
    }  
}

/*************************************************************************
*              
*
*  函数名称：PORTB_IRQHandler
*  功能说明：PORTB端口中断服务函数
*  参数说明：无
*  函数返回：无
*  备    注：引脚号需要根据自己初始化来修改
*
*************************************************************************/
void PORTB_IRQHandler()
{   unsigned int lie; 
    if( PORTB_ISFR & (1 << 22))         //PTB23触发中断  
    {
        PORTB_ISFR  |= (1 << 22);        //写1清中断标志位
//        DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTD_BYTE0_IN, Buffer1, PTC19, DMA_BYTE1, CAMERA_SIZE, DMA_falling_down );//DMA_falling_down  DMA_rising_down
//        DMA_EN(DMA_CH4); 
    }
}

/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：PORTD_IRQHandler
*  功能说明：PORTA端口中断服务函数
*  参数说明：无
*  函数返回：无
*  备    注：引脚号需要自己初始化来清除
*
*************************************************************************/


void PORTC_IRQHandler()  
{   unsigned int lie; 
    //---VSYN场中断处理
    if(PORTC_ISFR & (1 << 15))         //PTD8触发中断  场中断
    { 
        PORTC_ISFR  |= (1 << 15);       //写1清中断标志位
        DMA_PORTx2BUFF_Init (DMA_CH4, (void *)&PTD_BYTE0_IN, Buffer1, PTC13, DMA_BYTE1, CAMERA_SIZE, DMA_falling_down );//DMA_falling_down  DMA_rising_down
        DMA_EN(DMA_CH4); 
    }  
}

/*************************************************************************
*                            岱默科技DEMOK Kinetis开发小组
*
*  函数名称：DMA_CH4_Handler
*  功能说明：DMA通道4的中断服务函数
*  参数说明：无
*  函数返回：无
*
*************************************************************************/
void DMA_CH4_Handler(void)
{
    DMA_IRQ_CLEAN(DMA_CH4);                             //清除通道传输中断标志位    (这样才能再次进入中断)
    DMA_IRQ_DIS(DMA_CH4);                               //禁止DMA   等待下一次行中断来临开启DMA
    DisableInterrupts;
    //rowCnt = 0;
    DMA_DIS(DMA_CH4);
    //disable_irq(90); //关闭PORTD中断，关闭场中断响应
    VSYN_Flag = 1;
}

/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：FTM0_IRQHandler
*  功能说明：FTM0输入捕捉中断服务函数
*  参数说明：无
*  函数返回：无
*  备    注：引脚号需要根据自己初始化来修改，参考现有的代码添加自己的功能
*
*************************************************************************/
void FTM0_IRQHandler()
{


}
/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：FTM2_IRQHandler
*  功能说明：FTM2输入捕捉中断服务函数
*  参数说明：无
*  函数返回：无
*  备    注：引脚号需要根据自己初始化来修改，参考现有的代码添加自己的功能
*
*************************************************************************/
void FTM2_IN_IRQHandler()
{  unsigned char curstatus = FTM2_STATUS;
   FTM2_STATUS = 0x00;  //清中断
//   if(curstatus&(1<<0))  //CH0中断,左测速
//   { leftspeed++;
//   }
//   if(curstatus&(1<<1))  //CH1中断，右测速
//   { rightspeed++;
//   }
}
/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*
*  函数名称：FTM1_IRQHandler
*  功能说明：FTM1输入捕捉中断服务函数
*  参数说明：无
*  函数返回：无
*  备    注：引脚号需要根据自己初始化来修改，参考现有的代码添加自己的功能
*
*************************************************************************/
void FTM1_IRQHandler()
{
   unsigned char curstatus = FTM1_STATUS;
   FTM1_STATUS = 0x00;  //清中断
   if(curstatus&(1<<0))  //CH0中断,左测速
   { leftspeed++;
   }
   if(curstatus&(1<<1))  //CH1中断，右测速
   { rightspeed++;
   }
}
