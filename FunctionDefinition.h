//函数定义



/***************************************2018级自定义函数******************************************/
void FunctionInitialization(u8* sccb_ack)   //各种初始化
{
	FTM_IN_Init();           //初始化FTM1为捕获
	GPIO_Init();             //------GPIO初始化  用于指示SCCB状态
	sccb_init();             //-----SCCB初始化，用于配置摄像头状态 
	*sccb_ack = sccb_refresh();
	PWM_Init();              //-----PWM初始化，用于初始化舵机电机的输出 
	EXTI_Init();             //-----外部中断初始化，用于摄像头中断触发
	UART_Init();             //-----UART初始化，用于初始化串口   
	AD_INIT();
	pit_init_ms(PIT0, 5);    //初始化PIT0，定时时间为： 5ms 
	pit_init_ms(PIT1, 5);    //初始化PIT1，定时时间为： 5ms 
	oled_init();             //oled初始化
	MPU6050_Init();          //MPU6050初始化  , PD8  , PD9
	Image_Init();            //----图像数组初始化




	disable_irq(84);//关闭PIT0中断
	disable_irq(85);//关闭PIT1中断
//	uart_irq_EN(UART0);
//	uart_putchar(UART0,'O');  
//	uart_putchar(UART0,'K');  
	enable_irq(84);//开启PIT0中断 	 
	LCD_CLS();  //清屏
	LCD_P6x8Str(10, 7, "Key ...");// Press 
	getdcinit();  //检测电磁最大最小值

	enable_irq(85);//开启PIT1中断
	LCD_P6x8Str(0, 0, "DuoCen:");
	Dis_num(60, 0, DuoCenter);
	LCD_P6x8Str(0, 1, "Speed:");
	Dis_num(40, 1, dianjispeed);
	LCD_P6x8Str(0, 2, "Duo_P D:");
	Dis_num(60, 2, servPram);
	Dis_num(92, 2, dPram);
	LCD_P6x8Str(0, 3, "RoadBd:");
	LCD_P6x8Str(0, 4, "Row_mid:");
	Dis_num(70, 4, row_mid);
	LCD_P6x8Str(0, 5, "L:         R:");
	if (*sccb_ack == 0)  //OV7640初始化成功
		LCD_P6x8Str(0, 7, "OV7725 SCCB OK");
	else
		LCD_P6x8Str(0, 7, "OV7725 SCCB ER");
}

void VariableInitialization()
{
	ii = 0;

	FTFL_BASE_PTR->FOPT = FTFL_BASE_PTR->FOPT & 0XF7;
	brightval = 40;
	pwmdiff = 0;
#if ENSHESHATOU
	endianci = 0;
#else
	endicanci = 1;  //???????????变量名写错了？
#endif
	DuoCenter = servMotorCenture;//  1630;  //   1210  ~   1610
	dianjispeed = 1500;
	curdistance = 2000;
	ov7725auto = 0;  //自动曝光
	julicnt = 0;
	setspeed = setspeedhigh;
	DuoDir = 1;  //如果小车转向和实际要求相反，把该值修改成 1
	workmode = 2;  //0 舵机调模式  >0 正常工作模式
	ensend = 0;
	servPram = servPram1;
	dPram = dPram1;
	row_mid = 40;
}

void RepeatTheOperation1()
{
	if (workmode == 1)
		LCD_P6x8Str(0, 0, "DuoCen:             ");
	else
		LCD_P6x8Str(0, 0, "Duo ADJ:            ");

	LCD_P6x8Str(0, 1, "Speed:");
	LCD_P6x8Str(0, 2, "Duo_P D:");
	LCD_P6x8Str(0, 3, "RoadBd:");
	LCD_P6x8Str(0, 4, "Row_mid:");
	LCD_P6x8Str(0, 5, "L:         R:");
	LCD_P6x8Str(0, 6, "Turn:");
	LCD_P6x8Str(110, 7, "mV");
	Dis_num(96, 0, servPWMDuty);
	Dis_num(90, 1, newspeedpwm);
	Dis_num(60, 2, servPram);
	Dis_num(92, 2, dPram);
	Dis_num(60, 3, leftlineold);
	Dis_num(96, 3, rightlineold);
	Dis_num(70, 4, row_mid);
	Dis_num(20, 5, nowleftspeed);
	Dis_num(84, 5, nowrightspeed);
	Dis_num(36, 6, GYROSCOPE_OFFSET_Y);
	Dis_num(86, 6, disTurn_Speed);
	Dis_num(0, 7, servPram3);
	Dis_num(40, 7, dPram3);

	battval = ad_ave(ADC1, AD6a, ADC_10bit, 5);
	batvoltage = battval * 10.3125;  //mV
	Dis_num(76, 7, batvoltage);
	if (endianci == 0)
	{
		LCD_P6x8Str(110, 4, "  ");
		LCD_P6x8Str(110, 4, "C ");
	}
	else
	{
		LCD_P6x8Str(110, 4, "  ");
		LCD_P6x8Str(110, 4, "D ");
	}
}

void ImageAcquisition()
{
	Dis_num(90, 7, ii);//红外值1100
	ImageProc();
	if (workmode == 2) {
		oledcnt++;
		if (oledcnt > 2) {
			LCD_CLS();
			oledcnt = 0;
			oledimg();
			Dis_num(90, 0, omuCount);
			Dis_num(90, 1, omuOutCount);
			Dis_num(90, 2, speedErrPre);
			Dis_num(90, 3, AD_val_1);            //row_mid
			Dis_num(90, 4, AD_val_2);          //brightval
			Dis_num(90, 5, julicnt);
			Dis_num(90, 6, biZhangFlag);
		}
	}
	VSYN_Flag = 0;
	EnableInterrupts;
}

void RepeatTheOperation2()
{
	delayms(3);
	if (gpio_get(PORTA, 9) == 0)  //Key1   
	{
		ensend++;
		if (ensend > 1) ensend = 0;
		while (gpio_get(PORTA, 9) == 0);
	}
}

void RepeatTheOperation3()
{
	delayms(3);
	if (gpio_get(PORTA, 25) == 0)  //Key1   
	{
		LCD_CLS();
		workmode++;
		if (workmode > 2) workmode = 0;
		if (workmode == 0)  //舵机调试模式 
		{
			LCD_P6x8Str(0, 0, "Duo ADJ:            ");  //ADJ：调整？  舵机调整？
			FTM_PWM_Duty(FTM2, CH0, DuoCenter);
			Dis_num(60, 0, DuoCenter);
		}
		else if (workmode == 1)
		{
			DuoCenter = servMotorCenture;  //还原正常值
			LCD_P6x8Str(0, 0, "DuoCen:");
		}
		else if (workmode == 2) //液晶显示图像
		{
			//LCD_CLS();          
		}
		while (gpio_get(PORTA, 25) == 0)
			continue;
	}
}

void RepeatTheOperation4()
{
	if (gpio_get(PORTA, 27) == 0)  // E26 Duocenter++
	{
		delayms(3);
		if (gpio_get(PORTA, 27) == 0)
		{
			DuoCenter = DuoCenter + 20;
			Dis_num(60, 0, DuoCenter);
			FTM_PWM_Duty(FTM2, CH0, DuoCenter);
			while (gpio_get(PORTA, 27) == 0)
				continue;
		}
	}
	if (gpio_get(PORTA, 24) == 0)   // E27  Duocenter--
	{
		delayms(3);
		if (gpio_get(PORTA, 124) == 0)
		{
			DuoCenter = DuoCenter - 20;
			Dis_num(60, 0, DuoCenter);
			FTM_PWM_Duty(FTM2, CH0, DuoCenter);
			while (gpio_get(PORTA, 24) == 0);
		}
	}
}

void RepeatTheOperation5(u8* sccb_ack)
{
	if (gpio_get(PORTA, 27) == 0)  //  E26
	{
		delayms(3);
		if (gpio_get(PORTA, 27) == 0)
		{
			brightval = brightval + 2;
			*sccb_ack = sccb_regWrite(0x42, OV7725_BRIGHT, brightval);
			if (*sccb_ack == 0) //成功
				LCD_P6x8Str(90, 5, "SUCC");
			else
			{
				brightval = brightval - 2; //恢复原值
				LCD_P6x8Str(90, 5, "Fail");
			}
			Dis_num(90, 4, brightval);
			while (gpio_get(PORTA, 27) == 0)
				continue;
		}
	}
	if (gpio_get(PORTA, 24) == 0)   // E27 Duocenter--
	{
		delayms(3);
		if (gpio_get(PORTA, 24) == 0)
		{
			brightval = brightval - 2;
			*sccb_ack = sccb_regWrite(0x42, OV7725_BRIGHT, brightval);
			if (*sccb_ack == 0) //成功
				LCD_P6x8Str(90, 5, "SUCC");
			else
			{
				brightval = brightval + 2; //恢复原值
				LCD_P6x8Str(90, 5, "Fail");
			}
			Dis_num(90, 4, brightval);
			while (gpio_get(PORTA, 24) == 0)
				continue;
		}
	}
}
/***************************************2018级自定义函数******************************************/
