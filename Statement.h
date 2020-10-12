//声明





// 50HZ 舵机参数
#define servMotorCenture 5360 //835 //620 //  //   6450  //620  //650  //1300  //1435  //775  //1550 
#define servMotorLeft    4860//4560  //750  // 5440 //540  //570  //1155  //1270  //700  //1400  //1060  
#define servMotorRight   5860//6160  //920// // 7440  //700  //730  //1445  //1600  //850  //1700  //1640
//摄像头开关
#define ENSHESHATOU ON
//避障参数
#define HONGWAIZHANGAI  1300 //红外值，测量距离，用来避障

#define JIEJINJULI
#define BIZHANGFANGXIANG 0//0 左转 1右转
#define BZMOTORLEFT 4860//避障舵机左向角度  :不确定猜想
#define BZMOTORRIGHT 5860//避障舵机右向角度  :不确定猜想
#define BZDelay 110//避障延迟，遇到障碍转向保持BZDelay时间直行越过障碍后，再拐回赛道  :不确定猜想
#define BZOUTDELAY		10   //避障回正
u8 outBZFlag = 0;//出避障标志
u8 biZhangFlag = 0;//进入避障标志
u16 julicnt;//距离计数器
//  可选分辨率                   ：  80 * 60    160 * 120   240*180   320*240
//  一行接收的字节数 = 实际列数/8    10 * 60    20  * 120   30 *180   40 *240
//  ROW和COL的定义见 include.h

u8 Buffer1[CAMERA_SIZE] = { 0 };   //  //通过DMA采集(在isr.c中行中断中触发DMA采集)二维图像存储于Buffer1数组中，ROW,COL在Include.h中定义
u8 cmos[cmosrow][cmoscol] = { 0 };  //进行处理的数据

/*img_edg是一个一维数组 记录了摄像头每行的跳变沿的坐标值  
每行跳变沿 由上升沿开始（由黑变白）然后接下降沿（由白变黑）
   0xff用于指示该行的跳变沿完了，开始记录下一行
   如果每行图像以白色部分开始，那么该行跳变沿起始位置为0
   如果该行全黑 那么该行记录为0xff
   如果该行为全白 记录为 0 0xff


  oxff代表本行坐标值结束，进入下一行
 //edgposition[i]代表第i行的 跳变沿 在 img_edg 中坐标起点

 */
//暂时不清楚，与图像有关，CAMERA_SIZE为图像分辨率
//在图像初始化函数中，Bufferl数组被初始化
// imgtrans函数中， imgbuff_process[m] = Buffer1[m];
//用来存储图像处理后的颜色，即黑和白
u8 imgbuff_process[CAMERA_SIZE] = { 0 };

//img_edg是一个一维数组 记录了摄像头每行的跳变沿的坐标值  
//每行跳变沿 由上升沿开始（由黑变白）然后接下降沿（由白变黑） 
u8 img_edg[CAMERA_SIZE * 2];

u8 oledcmos[8][80] = { 0 };
//edgposition[i]代表第i行的 跳变沿 在 img_edg 中坐标起点
u16 edgposition[ROW];
//计数
u16 cont;
u8  LMR[3][ROW];//不理解
float Middle_Err;//不理解


int   AD_1 = 0;
int   AD_2 = 0;

u8  ov7725auto;  //ov7725曝光设置： 0：自动曝光  1：手动曝光



u16 servPram;  //舵机转角比例系数 放大10倍
u16 dPram;     //舵机转角积分系数  放大100倍

///  50HZ舵机 左右偏差约160
//u16 servPram0 =  6; 
//u16 dPram0 =  34;         //直线 
//
//u16 servPram1 =  11;    // 
//u16 dPram1 =  27;       //小弯 
//
//u16 servPram2 =  14;       
//u16 dPram2 =  20;  //中弯时  
//
//u16 servPram3 =  22; 
//u16 dPram3 =  10;      //大弯时   

///  60HZ舵机 %70000 左右偏差约800
u16 servPram0 = 55;// 6; 
u16 dPram0 = 330;//34;         //直线 

u16 servPram1 = 95;// 11;    // 
u16 dPram1 = 260;// 27;       //小弯 

u16 servPram2 = 130;//  14;       
u16 dPram2 = 200;// 20;  //中弯时  

u16 servPram3 = 210;// 22; 
u16 dPram3 = 90;// 10;      //大弯时  
///  100HZ舵机 左右偏差约300
//u16 servPram0 =  12; 
//u16 dPram0 =  50;         //直线 
//
//u16 servPram1 =  24;    // 
//u16 dPram1 =  40;       //小弯 
//
//u16 servPram2 =  34;       
//u16 dPram2 =  30;  //中弯时  
//
//u16 servPram3 =  45; 
//u16 dPram3 =  15;      //大弯时 


u8  Threshold[40];

s32 zuoPos = 0;          //左边界位置[有符号数]
s32 youPos = 0;          //右边界位置[有符号数]

s32 PreErr = 0, LastErr = 0, dErr = 0;
u32 centure = 0;         //计算出的当前车体的中心位置[无符号数]
s32 centureErr = 0;      //计算出的当前车体位置偏差 [有符号数]
s32 turn1 = 0;            //边界走势  正为向左偏转  负为向右偏转
s32 turn2 = 0;
u8 zuoEdgeFlag = 0;      //左边界确认标志，找到后边界置位
u8 youEdgeFlag = 0;      //右边界确认标志，找到后边界置位

u8 VSYN_Flag = 0;        //完成一场标识位  0为未完成；1为完成
u8 time;
u8 ensend;  //允许发送
u8 enpwm;
u8 speedmodi;  //速度修改标志

u8 DuoDir;//转向时方向
u8 workmode;//工作模式
u8 lockduoji;//引用没有找到使用,不清楚

u16 leftline, rightline, leftlineold, rightlineold;//引用没有找到使用,不清楚
u8  st_left, st_right;//引用没有找到使用,不清楚
u16 wideline, wideline2;//引用没有找到使用,不清楚
//s16 row_middle[rowNum];
s16 row_mid;  //中线值
s16 row_midold; //前一次中线值
s16 inomu = 0;//引用没有找到使用,不清楚
s16 shiziflag = 0;//引用没有找到使用,不清楚
s16 leftshizi, rightshizi;//引用没有找到使用,不清楚
s16 zhidaoflag = 0;//引用没有找到使用,不清楚
u8 lasterror;//引用没有找到使用,不清楚

//引用没有找到使用,不清楚
u16 left_lost = 0, right_lost = 0, black_left, black_right, lost_flag, left_black, right_black, flag1 = 0;
s16 sum_m = 0, part2_sum = 0, field_lost = 0, break_sum = 0;
u8  qian2 = 0, shiziwan_left = 0, shiziwan_right = 0;


u16 DuoCenter;  //舵机中间值 120HZ
u16 dianjispeed;//电机速度，用于速度不当时，根据与pwm调节速度  :猜想
u16 nowspeed;  //小车实际速度
s32 TimeCount = 0;//1ms中断标志
s16 leftspeed, rightspeed;//左右电机速度
u16 nowleftspeed, nowrightspeed;  //当前左右速度
s16 speedErr = 0;//引用没有找到使用,不清楚
s16 speedErrPre = 0;//引用没有找到使用,不清楚
s16 speedErrPre2 = 0;//引用没有找到使用,不清楚
extern u8 TIME1flag_100ms;//不清楚


u16 setspeed;  //设定速度
u16 setspeedhigh = 300;//400; //直道时速度
u16 setspeedlow = 100;  //大弯时速度
u16 setspeeddiancilow = 100;//电磁转向速度
u16 setspeeddianci = 150;//150//电磁速度
u16 speedkp = 60;//调节速度
u16 speedkd = 10;  //速度调节PD值
s16 speeddif = 0;     //大弯时左右轮速度差
s32 newspeedpwm;   //速度PWM
s16 servPWMDuty;   //舵机PWM占空比
u16 discnt, mycnt, beepcnt;//discnt避障相关，beepcnt小车上的按钮，可能欧姆环相关
u8  oledcnt;
u8  brightval; //曝光值，越大图像越亮



////////////////////////////////////////

s8	omuFlag = 0,  //0 未检测到 1 持续检测到 2 检测中断 3 未定义
//猜想与欧姆有关
omuOutFlag = 0,
omuLtState = 0,
omuRtState = 0,
omuCount = 0,
omuOutCount = 0,
omucnt = 0;
//	omuleftend1Point16 = -1,
//	omuleftstart1Point16 = -1,
//	omuleftend2Point16 = -1,
//	omuleftstart2Point16 = -1,
//	omurightend1Point16 = -1,
//	omurightstart1Point16 = -1,
//	omurightend2Point16 = -1,
//	omurightstart2Point16 = -1;
/////////////////////////////////////////

u16 curdistance;


volatile u32 rowCnt = 0;//行计数


u8 endiscnt;//不清楚
int battval;//不清楚
int batvoltage;//不清楚
s16 GYROSCOPE_OFFSET_Y;//不清楚
s16 ygout = 1000;//引用没有找到使用,不清楚
float Turn_Speed;//转向角速度
s16 disTurn_Speed;//不清楚
float TurnD = 0;//5.6--(6.2)


// 电磁控制参数
int   AD_val_1 = 0;
int   AD_val_2 = 0;
int   AD_val_3 = 0;
int   adtmp1, adtmp2, adtmp3;
int   dis_AD_val_1, dis_AD_val_2, dis_AD_val_3;
//int   disgy_AD_val_1,disgy_AD_val_2,disgy_AD_val_3 ;
int   AD_val_1_min = 0;
int   AD_val_2_min = 0;
int   AD_val_3_min = 0;


int   AD_val_1_max = 4096;
int   AD_val_2_max = 4096;
int   AD_val_3_max = 2400;


u16 ii = 0;

//不清楚
unsigned char  Dir_last = 0;
int  dir_error_pre = 0;
int  dir_error = 0;
int dis_error;
unsigned char zz;


//u32 sum=0;
////////////////控制参数 ////////////////
s16 rkp3 = 22;  //   5  转向PD参数 直道 放大10倍
s16 rkd3 = 6; // 160

s16 rkp2 = 24;  //16  //9  转向PD参数 小弯道
s16 rkd2 = 4;  //100; //130

s16 rkp1 = 25; //16   //6  转向PD参数 大弯道
s16 rkd1 = 4;  //45

int  DuoP;   //
int  DuoD;

u16 pwmdiff;  //两轮的差速

u8 endianci;  //电磁还是摄像头工作切换标志



/////////////////////////////////////////////////////
//-----------------函数声明------------------
//图像处理
void ImageProc();
//K60核心板载LED，初始化
void GPIO_Init();
//初始化FTM0_CH0
void PWM_Init();
//初始化串口1-蓝牙串口 波特率为115200
void UART_Init();

void EXTI_Init();
//图像初始化
void Image_Init();
//行驶函数
void run();
//void CountThreshole(void);
//行驶过程中的数据处理，状态判断（图像，电磁的切换）
void datatrans();
//不清楚
void FTM_IN_Init();
//void simplecal();  //简单的路径识别 
void get_edge();//查找路径边界
void Search();//计算小车位置
void imgtrans();//图像初始化 :不确定
//不清楚
void oledtrans();
void oledimg();
void AD_INIT();
void getdcinit();  //读电磁最大最小值
void newroadcal();  //新舵机控制程序
//不清楚
void onebeep();
void biZhang();     //避障程序
void biZhangDuo();  //避障舵机控制
//欧姆
void OMuDuo();
void OMuOutDuo();

//2018级自定义函数
/********************************************************/
void FunctionInitialization(u8* sccb_ack);			//函数初始化
void VariableInitialization();                      //变量初始化
void RepeatTheOperation1();							//
void ImageAcquisition();							//完成一幅图像采集
void RepeatTheOperation2();							//Key1  上传图像 
void RepeatTheOperation3();							//Key1  E24
void RepeatTheOperation4();							//舵机调试模式下
void RepeatTheOperation5(u8* sccb_ack);				//图像显示模式



/********************************************************/

#define OMUDIANCI 99
#define OMUDELAY  8
#define OMUFLAGOUTDELAY 3   
#define OMUOUTDUOFLAG 1
#define ZuoOMU 0
#define	ENOUMUDIANCI 1
#define OMUINDELAY  20//100ms
#define OMUOUTDELAY 15//75ms
//A.A.A.A


