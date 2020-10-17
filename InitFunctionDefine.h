


void GPIO_Init()
{
    //K60���İ���LED
    gpio_init(PORTA, 17, GPO, 1u);    //��ʼ��Ϊ�ߵ�ƽ���---LED0
    gpio_init(PORTC, 5, GPO, 1u);    //��ʼ��Ϊ�ߵ�ƽ���---LED1
    gpio_init(PORTD, 15, GPO, 1u);    //��ʼ��Ϊ�ߵ�ƽ���---LED2
    gpio_init(PORTE, 24, GPO, 1u);    //��ʼ��Ϊ�ߵ�ƽ���---LED2

    gpio_init(PORTA, 24, GPI, 1);     //��������  ��
    gpio_init(PORTA, 28, GPI, 1);     //��������  ��
    gpio_init(PORTA, 27, GPI, 1);     //��������  ��
    gpio_init(PORTA, 26, GPI, 1);     //�������� ��
    gpio_init(PORTA, 25, GPI, 1);     //�������� 

    gpio_init(PORTA, 9, GPI, 1);     //�������� 

    gpio_init(PORTE, 9, GPO, 1);    //������ �͵�ƽ���
}

void AD_INIT()
{
    adc_init(ADC1, AD6a);  //��ص�ѹ E2

    adc_init(ADC1, AD8);  // 
    adc_init(ADC1, AD13);  // 

}

void PWM_Init()
{
    FTM_PWM_init(FTM2, CH0, 60, DuoCenter);     //��ʼ��FTM2_CH0���Ƶ��Ϊ50HZ,ռ�ձ�ΪDuoCenter ����� FTM1_CH0��ӦPTA8��
    FTM_PWM_init(FTM0, CH0, 10000, 0); //��ʼ��FTM0_CH0���Ƶ��Ϊ10KHZ,ռ�ձ�dianjispeed ��FTM0_CH0��ӦPTC1��
    FTM_PWM_init(FTM0, CH1, 10000, dianjispeed); //��ʼ��FTM0_CH0���Ƶ��Ϊ10KHZ,ռ�ձ�dianjispeed ��FTM0_CH0��ӦPTC1��
    FTM_PWM_init(FTM0, CH2, 10000, 0); //��ʼ��FTM0_CH0���Ƶ��Ϊ10KHZ,ռ�ձ�dianjispeed ��FTM0_CH0��ӦPTC1��
    FTM_PWM_init(FTM0, CH3, 10000, dianjispeed); //��ʼ��FTM0_CH0���Ƶ��Ϊ10KHZ,ռ�ձ�dianjispeed ��FTM0_CH0��ӦPTC1��
}

void UART_Init()
{
    uart_init(UART0, 115200);//115200);//115200);      //��ʼ������1-�������� ������Ϊ115200
}

void FTM_IN_Init()
{
    SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;

    PORTA_PCR12 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;   //PA12
    FTM1_C0SC |= (FTM_CnSC_ELSA_MASK | FTM_CnSC_CHIE_MASK);  //�����ش���
    FTM1_C0SC &= ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK);

    PORTA_PCR13 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK;   //PA13
    FTM1_C1SC |= (FTM_CnSC_ELSA_MASK | FTM_CnSC_CHIE_MASK);  //�����ش���
    FTM1_C1SC &= ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK);

    FTM1_SC = FTM_SC_CLKS(1);
    FTM1_MODE |= FTM_MODE_WPDIS_MASK;
    FTM1_COMBINE = 0;
    FTM1_MODE &= ~FTM_MODE_FTMEN_MASK;
    FTM1_CNTIN = 0;
    FTM1_STATUS = 0x00; //���жϱ�־λ
    enable_irq(63);  //FTM1�ж�
}

void EXTI_Init()
{
    //----��ʼ���ⲿ�ж�---//
    exti_init(PORTC, 15, falling_down);   //HREF----PORTB22 �˿��ⲿ�жϳ�ʼ�� �������ش����жϣ��ڲ�����
    endiscnt = 0;
}
