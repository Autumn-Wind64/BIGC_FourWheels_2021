


void GPIO_Init()
{
    //K60核心板载LED
    gpio_init(PORTA, 17, GPO, 1u);    //初始化为高电平输出---LED0
    gpio_init(PORTC, 5, GPO, 1u);    //初始化为高电平输出---LED1
    gpio_init(PORTD, 15, GPO, 1u);    //初始化为高电平输出---LED2
    gpio_init(PORTE, 24, GPO, 1u);    //初始化为高电平输出---LED2

    gpio_init(PORTA, 24, GPI, 1);     //按键输入  下
    gpio_init(PORTA, 28, GPI, 1);     //按键输入  左
    gpio_init(PORTA, 27, GPI, 1);     //按键输入  上
    gpio_init(PORTA, 26, GPI, 1);     //按键输入 右
    gpio_init(PORTA, 25, GPI, 1);     //按键输入 

    gpio_init(PORTA, 9, GPI, 1);     //按键输入 

    gpio_init(PORTE, 9, GPO, 1);    //蜂鸣器 低电平输出
}

void AD_INIT()
{
    adc_init(ADC1, AD6a);  //电池电压 E2

    adc_init(ADC1, AD8);  // 
    adc_init(ADC1, AD13);  // 

}

void PWM_Init()
{
    FTM_PWM_init(FTM2, CH0, 60, DuoCenter);     //初始化FTM2_CH0输出频率为50HZ,占空比为DuoCenter ：舵机 FTM1_CH0对应PTA8口
    FTM_PWM_init(FTM0, CH0, 10000, 0); //初始化FTM0_CH0输出频率为10KHZ,占空比dianjispeed ：FTM0_CH0对应PTC1口
    FTM_PWM_init(FTM0, CH1, 10000, dianjispeed); //初始化FTM0_CH0输出频率为10KHZ,占空比dianjispeed ：FTM0_CH0对应PTC1口
    FTM_PWM_init(FTM0, CH2, 10000, 0); //初始化FTM0_CH0输出频率为10KHZ,占空比dianjispeed ：FTM0_CH0对应PTC1口
    FTM_PWM_init(FTM0, CH3, 10000, dianjispeed); //初始化FTM0_CH0输出频率为10KHZ,占空比dianjispeed ：FTM0_CH0对应PTC1口
}

void UART_Init()
{
    uart_init(UART0, 115200);//115200);//115200);      //初始化串口1-蓝牙串口 波特率为115200
}

void FTM_IN_Init()
{
    SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;

    PORTA_PCR12 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;   //PA12
    FTM1_C0SC |= (FTM_CnSC_ELSA_MASK | FTM_CnSC_CHIE_MASK);  //上升沿触发
    FTM1_C0SC &= ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK);

    PORTA_PCR13 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;   //PA13
    FTM1_C1SC |= (FTM_CnSC_ELSA_MASK | FTM_CnSC_CHIE_MASK);  //上升沿触发
    FTM1_C1SC &= ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK);

    FTM1_SC = FTM_SC_CLKS(1);
    FTM1_MODE |= FTM_MODE_WPDIS_MASK;
    FTM1_COMBINE = 0;
    FTM1_MODE &= ~FTM_MODE_FTMEN_MASK;
    FTM1_CNTIN = 0;
    FTM1_STATUS = 0x00; //清中断标志位
    enable_irq(63);  //FTM1中断
}

void EXTI_Init()
{
    //----初始化外部中断---//
    exti_init(PORTC, 15, falling_down);   //HREF----PORTB22 端口外部中断初始化 ，上升沿触发中断，内部下拉
    endiscnt = 0;
}
