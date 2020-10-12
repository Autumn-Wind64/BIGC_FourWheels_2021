//3号程序_摄像头欧姆_先避障_无冲坡
#define ON  1
#define OFF	0
#include "common.h"
#include "include.h"
#include "Statement.h"
#include "FunctionDefinition.h"
#include "FunctionDefinition2.h"
#include "InitFunctionDefine.h"


//-----------------主函数--------------------

//3号程序_摄像头欧姆_先避障_无冲坡
void main()
{  
	/*********声明变量**********/

	s32 ygtemp;
	u8 sccb_ack;

    VariableInitialization();

    /***************各种初始化*********/
	DisableInterrupts;               //关闭中断开始初始化数据

    FunctionInitialization(&sccb_ack);       //各种初始化

    EnableInterrupts;
	//
	//ygtemp=0;
	//for(ii=0;ii<100;ii++){ 
//		ygtemp += MPU6050_GetDoubleData(MPU6050_GYRO_YOUT);//读Y轴角速度，转向
//		Pause();
//	}
//	GYROSCOPE_OFFSET_Y = ygtemp/100;        //静态X轴角速度值， 理论上为0，但实际可能不为0
//GYROSCOPE_OFFSET_Y = 46;
     //while(1);
	gpio_set(PORTE,9,0);
	delayms(40);
	gpio_set(PORTE,9,1);
	delayms(150);
	gpio_set(PORTE,9,0);
	delayms(40);
	gpio_set(PORTE,9,1);
	LCD_P6x8Str(0,7,  "              ");
	/*************初始化结束************************/
	while(1)
    {  
        if (VSYN_Flag == 1)    //完成一幅图像采集
        {
            //ii = ad_once(ADC1, AD15, ADC_12bit);
            //ii++;
            ImageAcquisition();
        }

        if (discnt > 25)          //每50ms刷新
        {
            discnt = 0;
            if (workmode < 2) 
            {
                RepeatTheOperation1();
            }
            PTA17_OUT = ~PTA17_OUT;
            PTC5_OUT = ~PTC5_OUT;
            PTD15_OUT = ~PTD15_OUT;
            PTE24_OUT = ~PTE24_OUT;
        }
        if ((gpio_get(PORTA, 9) == 0) && (workmode == 1))  //Key1  上传图像 
        {
            RepeatTheOperation2();
        }
        if (gpio_get(PORTA, 25) == 0)  //Key1  E24 
        {
            RepeatTheOperation3();
        }
        if (workmode == 0)  //舵机调试模式下
        {
            RepeatTheOperation4();
        }
        else if (workmode == 2)  //图像显示模式下，可以调节曝光
        {
            RepeatTheOperation5(&sccb_ack);
        }
    }
}