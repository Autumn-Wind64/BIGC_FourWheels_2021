//����





// 50HZ �������
#define servMotorCenture 5360 //835 //620 //  //   6450  //620  //650  //1300  //1435  //775  //1550 
#define servMotorLeft    4860//4560  //750  // 5440 //540  //570  //1155  //1270  //700  //1400  //1060  
#define servMotorRight   5860//6160  //920// // 7440  //700  //730  //1445  //1600  //850  //1700  //1640
//����ͷ����
#define ENSHESHATOU ON
//���ϲ���
#define HONGWAIZHANGAI  1300 //����ֵ���������룬��������

#define JIEJINJULI
#define BIZHANGFANGXIANG 0//0 ��ת 1��ת
#define BZMOTORLEFT 4860//���϶������Ƕ�  :��ȷ������
#define BZMOTORRIGHT 5860//���϶������Ƕ�  :��ȷ������
#define BZDelay 110//�����ӳ٣������ϰ�ת�򱣳�BZDelayʱ��ֱ��Խ���ϰ����ٹջ�����  :��ȷ������
#define BZOUTDELAY		10   //���ϻ���
u8 outBZFlag = 0;//�����ϱ�־
u8 biZhangFlag = 0;//������ϱ�־
u16 julicnt;//���������
//  ��ѡ�ֱ���                   ��  80 * 60    160 * 120   240*180   320*240
//  һ�н��յ��ֽ��� = ʵ������/8    10 * 60    20  * 120   30 *180   40 *240
//  ROW��COL�Ķ���� include.h

u8 Buffer1[CAMERA_SIZE] = { 0 };   //  //ͨ��DMA�ɼ�(��isr.c�����ж��д���DMA�ɼ�)��άͼ��洢��Buffer1�����У�ROW,COL��Include.h�ж���
u8 cmos[cmosrow][cmoscol] = { 0 };  //���д��������

/*img_edg��һ��һά���� ��¼������ͷÿ�е������ص�����ֵ  
ÿ�������� �������ؿ�ʼ���ɺڱ�ף�Ȼ����½��أ��ɰױ�ڣ�
   0xff����ָʾ���е����������ˣ���ʼ��¼��һ��
   ���ÿ��ͼ���԰�ɫ���ֿ�ʼ����ô������������ʼλ��Ϊ0
   �������ȫ�� ��ô���м�¼Ϊ0xff
   �������Ϊȫ�� ��¼Ϊ 0 0xff


  oxff����������ֵ������������һ��
 //edgposition[i]�����i�е� ������ �� img_edg ���������

 */
//��ʱ���������ͼ���йأ�CAMERA_SIZEΪͼ��ֱ���
//��ͼ���ʼ�������У�Bufferl���鱻��ʼ��
// imgtrans�����У� imgbuff_process[m] = Buffer1[m];
//�����洢ͼ��������ɫ�����ںͰ�
u8 imgbuff_process[CAMERA_SIZE] = { 0 };

//img_edg��һ��һά���� ��¼������ͷÿ�е������ص�����ֵ  
//ÿ�������� �������ؿ�ʼ���ɺڱ�ף�Ȼ����½��أ��ɰױ�ڣ� 
u8 img_edg[CAMERA_SIZE * 2];

u8 oledcmos[8][80] = { 0 };
//edgposition[i]�����i�е� ������ �� img_edg ���������
u16 edgposition[ROW];
//����
u16 cont;
u8  LMR[3][ROW];//�����
float Middle_Err;//�����


int   AD_1 = 0;
int   AD_2 = 0;

u8  ov7725auto;  //ov7725�ع����ã� 0���Զ��ع�  1���ֶ��ع�



u16 servPram;  //���ת�Ǳ���ϵ�� �Ŵ�10��
u16 dPram;     //���ת�ǻ���ϵ��  �Ŵ�100��

///  50HZ��� ����ƫ��Լ160
//u16 servPram0 =  6; 
//u16 dPram0 =  34;         //ֱ�� 
//
//u16 servPram1 =  11;    // 
//u16 dPram1 =  27;       //С�� 
//
//u16 servPram2 =  14;       
//u16 dPram2 =  20;  //����ʱ  
//
//u16 servPram3 =  22; 
//u16 dPram3 =  10;      //����ʱ   

///  60HZ��� %70000 ����ƫ��Լ800
u16 servPram0 = 55;// 6; 
u16 dPram0 = 330;//34;         //ֱ�� 

u16 servPram1 = 95;// 11;    // 
u16 dPram1 = 260;// 27;       //С�� 

u16 servPram2 = 130;//  14;       
u16 dPram2 = 200;// 20;  //����ʱ  

u16 servPram3 = 210;// 22; 
u16 dPram3 = 90;// 10;      //����ʱ  
///  100HZ��� ����ƫ��Լ300
//u16 servPram0 =  12; 
//u16 dPram0 =  50;         //ֱ�� 
//
//u16 servPram1 =  24;    // 
//u16 dPram1 =  40;       //С�� 
//
//u16 servPram2 =  34;       
//u16 dPram2 =  30;  //����ʱ  
//
//u16 servPram3 =  45; 
//u16 dPram3 =  15;      //����ʱ 


u8  Threshold[40];

s32 zuoPos = 0;          //��߽�λ��[�з�����]
s32 youPos = 0;          //�ұ߽�λ��[�з�����]

s32 PreErr = 0, LastErr = 0, dErr = 0;
u32 centure = 0;         //������ĵ�ǰ���������λ��[�޷�����]
s32 centureErr = 0;      //������ĵ�ǰ����λ��ƫ�� [�з�����]
s32 turn1 = 0;            //�߽�����  ��Ϊ����ƫת  ��Ϊ����ƫת
s32 turn2 = 0;
u8 zuoEdgeFlag = 0;      //��߽�ȷ�ϱ�־���ҵ���߽���λ
u8 youEdgeFlag = 0;      //�ұ߽�ȷ�ϱ�־���ҵ���߽���λ

u8 VSYN_Flag = 0;        //���һ����ʶλ  0Ϊδ��ɣ�1Ϊ���
u8 time;
u8 ensend;  //������
u8 enpwm;
u8 speedmodi;  //�ٶ��޸ı�־

u8 DuoDir;//ת��ʱ����
u8 workmode;//����ģʽ
u8 lockduoji;//����û���ҵ�ʹ��,�����

u16 leftline, rightline, leftlineold, rightlineold;//����û���ҵ�ʹ��,�����
u8  st_left, st_right;//����û���ҵ�ʹ��,�����
u16 wideline, wideline2;//����û���ҵ�ʹ��,�����
//s16 row_middle[rowNum];
s16 row_mid;  //����ֵ
s16 row_midold; //ǰһ������ֵ
s16 inomu = 0;//����û���ҵ�ʹ��,�����
s16 shiziflag = 0;//����û���ҵ�ʹ��,�����
s16 leftshizi, rightshizi;//����û���ҵ�ʹ��,�����
s16 zhidaoflag = 0;//����û���ҵ�ʹ��,�����
u8 lasterror;//����û���ҵ�ʹ��,�����

//����û���ҵ�ʹ��,�����
u16 left_lost = 0, right_lost = 0, black_left, black_right, lost_flag, left_black, right_black, flag1 = 0;
s16 sum_m = 0, part2_sum = 0, field_lost = 0, break_sum = 0;
u8  qian2 = 0, shiziwan_left = 0, shiziwan_right = 0;


u16 DuoCenter;  //����м�ֵ 120HZ
u16 dianjispeed;//����ٶȣ������ٶȲ���ʱ��������pwm�����ٶ�  :����
u16 nowspeed;  //С��ʵ���ٶ�
s32 TimeCount = 0;//1ms�жϱ�־
s16 leftspeed, rightspeed;//���ҵ���ٶ�
u16 nowleftspeed, nowrightspeed;  //��ǰ�����ٶ�
s16 speedErr = 0;//����û���ҵ�ʹ��,�����
s16 speedErrPre = 0;//����û���ҵ�ʹ��,�����
s16 speedErrPre2 = 0;//����û���ҵ�ʹ��,�����
extern u8 TIME1flag_100ms;//�����


u16 setspeed;  //�趨�ٶ�
u16 setspeedhigh = 300;//400; //ֱ��ʱ�ٶ�
u16 setspeedlow = 100;  //����ʱ�ٶ�
u16 setspeeddiancilow = 100;//���ת���ٶ�
u16 setspeeddianci = 150;//150//����ٶ�
u16 speedkp = 60;//�����ٶ�
u16 speedkd = 10;  //�ٶȵ���PDֵ
s16 speeddif = 0;     //����ʱ�������ٶȲ�
s32 newspeedpwm;   //�ٶ�PWM
s16 servPWMDuty;   //���PWMռ�ձ�
u16 discnt, mycnt, beepcnt;//discnt������أ�beepcntС���ϵİ�ť������ŷķ�����
u8  oledcnt;
u8  brightval; //�ع�ֵ��Խ��ͼ��Խ��



////////////////////////////////////////

s8	omuFlag = 0,  //0 δ��⵽ 1 ������⵽ 2 ����ж� 3 δ����
//������ŷķ�й�
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


volatile u32 rowCnt = 0;//�м���


u8 endiscnt;//�����
int battval;//�����
int batvoltage;//�����
s16 GYROSCOPE_OFFSET_Y;//�����
s16 ygout = 1000;//����û���ҵ�ʹ��,�����
float Turn_Speed;//ת����ٶ�
s16 disTurn_Speed;//�����
float TurnD = 0;//5.6--(6.2)


// ��ſ��Ʋ���
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

//�����
unsigned char  Dir_last = 0;
int  dir_error_pre = 0;
int  dir_error = 0;
int dis_error;
unsigned char zz;


//u32 sum=0;
////////////////���Ʋ��� ////////////////
s16 rkp3 = 22;  //   5  ת��PD���� ֱ�� �Ŵ�10��
s16 rkd3 = 6; // 160

s16 rkp2 = 24;  //16  //9  ת��PD���� С���
s16 rkd2 = 4;  //100; //130

s16 rkp1 = 25; //16   //6  ת��PD���� �����
s16 rkd1 = 4;  //45

int  DuoP;   //
int  DuoD;

u16 pwmdiff;  //���ֵĲ���

u8 endianci;  //��Ż�������ͷ�����л���־



/////////////////////////////////////////////////////
//-----------------��������------------------
//ͼ����
void ImageProc();
//K60���İ���LED����ʼ��
void GPIO_Init();
//��ʼ��FTM0_CH0
void PWM_Init();
//��ʼ������1-�������� ������Ϊ115200
void UART_Init();

void EXTI_Init();
//ͼ���ʼ��
void Image_Init();
//��ʻ����
void run();
//void CountThreshole(void);
//��ʻ�����е����ݴ���״̬�жϣ�ͼ�񣬵�ŵ��л���
void datatrans();
//�����
void FTM_IN_Init();
//void simplecal();  //�򵥵�·��ʶ�� 
void get_edge();//����·���߽�
void Search();//����С��λ��
void imgtrans();//ͼ���ʼ�� :��ȷ��
//�����
void oledtrans();
void oledimg();
void AD_INIT();
void getdcinit();  //����������Сֵ
void newroadcal();  //�¶�����Ƴ���
//�����
void onebeep();
void biZhang();     //���ϳ���
void biZhangDuo();  //���϶������
//ŷķ
void OMuDuo();
void OMuOutDuo();

//2018���Զ��庯��
/********************************************************/
void FunctionInitialization(u8* sccb_ack);			//������ʼ��
void VariableInitialization();                      //������ʼ��
void RepeatTheOperation1();							//
void ImageAcquisition();							//���һ��ͼ��ɼ�
void RepeatTheOperation2();							//Key1  �ϴ�ͼ�� 
void RepeatTheOperation3();							//Key1  E24
void RepeatTheOperation4();							//�������ģʽ��
void RepeatTheOperation5(u8* sccb_ack);				//ͼ����ʾģʽ



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


