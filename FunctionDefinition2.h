





void onebeep()
{
    gpio_set(PORTB, 20, 0);
    delayms(40);
    gpio_set(PORTB, 20, 1);
}





void run()  // 5msִ��һ�� ��ÿ30ms��һ���ٶ� ��ÿ5msִ�е�ż��
{
    unsigned int temp1, temp2;
    beepcnt++;
    if (beepcnt > 70) //1S
    {
        beepcnt = 0;
        if (endianci == 1)
            gpio_turn(PORTB, 20);
        else
            gpio_set(PORTB, 20, 1);
    }
    discnt++;
    mycnt++;
    AD_1 = 0;
    AD_2 = 0;
    //int   AD_3=0;

    //AD_1 = ad_once(ADC1, AD4a, ADC_12bit);      //����ߵ�е�ѹ   PE0
    //AD_2 = ad_once(ADC1, AD9, ADC_12bit);      //���ұߵ�е�ѹ   PB1
    //AD_3 = ad_once(ADC1, AD5a, ADC_12bit);      //�м��е�ѹ     PE1

    AD_1 = ad_once(ADC1, AD13, ADC_12bit);      //����ߵ�е�ѹ   PB0
    AD_2 = ad_once(ADC1, AD8, ADC_12bit);      //���ұߵ�е�ѹ   PB7



    dis_AD_val_1 = AD_1;
    dis_AD_val_2 = AD_2;
    //dis_AD_val_3=AD_3;

    //�޷�
    if (AD_1 > AD_val_1_max)		AD_1 = AD_val_1_max;
    if (AD_2 > AD_val_2_max)		AD_2 = AD_val_2_max;
    //if(AD_3>AD_val_3_max)		AD_3=AD_val_3_max;

    if (AD_1 < AD_val_1_min)		AD_1 = AD_val_1_min;
    if (AD_2 < AD_val_2_min)		AD_2 = AD_val_2_min;
    //if(AD_3<AD_val_3_min)		AD_3=AD_val_3_min;

    //��һ��
    AD_val_1 = 100 * (AD_1 - AD_val_1_min) / (AD_val_1_max - AD_val_1_min);
    AD_val_2 = 100 * (AD_2 - AD_val_2_min) / (AD_val_2_max - AD_val_2_min);
    ////////////////////////////�뻷

    if (endianci == 1) //����ͷ������ͼ��ʱ�����õ�ŵ���
    {
        setspeed = setspeeddianci; //һ�����٣����λ��̫����
        newroadcal();
        if (servPWMDuty <= servMotorLeft)  servPWMDuty = servMotorLeft;
        if (servPWMDuty >= servMotorRight)  servPWMDuty = servMotorRight;
        if (workmode == 0) { // ����״̬
            if (biZhangFlag == 1) {
                biZhangDuo();
            }
            else if (omuFlag == 1) {
                OMuDuo();
            }
            else if (omuOutFlag == 1) {
                OMuOutDuo();
            }
            else
                FTM_PWM_Duty(FTM2, CH0, DuoCenter);
        } //DuoCenter
        else { //����״̬
            if (biZhangFlag == 1) {
                biZhangDuo();
            }
            else if (omuFlag == 1) {
                OMuDuo();
            }
            else if (omuOutFlag == 1) {
                OMuOutDuo();
            }
            else
                FTM_PWM_Duty(FTM2, CH0, servPWMDuty);
        }
    }

    if (mycnt >= 4)  //ÿ20ms����
    {
        nowrightspeed = rightspeed;
        nowleftspeed = leftspeed;
        rightspeed = 0;
        leftspeed = 0;
        mycnt = 0;
        nowspeed = (nowleftspeed + nowrightspeed) / 2;  //˫���ʱȡ��·��ƽ��ֵ
        speeddif = 0;
        if (endianci == 0) {  //����ͷ����ʱ���ٴ���
            if (row_mid < 35)
                speeddif = 400;//180 ;

            else if (row_mid > 45)  //���
                speeddif = -400; //180;

        }
        else { //��Ź���ʱ���ٴ���
            if ((AD_val_2 > 90) && (AD_val_2 - AD_val_1 >= 60))
                speeddif = -400;//180;      
            else if ((AD_val_1 > 90) && (AD_val_1 - AD_val_2 >= 20))
                speeddif = 400;//-180;       
        }

        //////  ����˫����ͨ��  ////////////////////////////////////////////////////



        if (nowspeed > setspeed + 6) //�ٶȹ��죬��Ҫ����
        {
            newspeedpwm = dianjispeed + speedkp * (setspeed - nowspeed);  //100//speedErr
            if (newspeedpwm < 450) newspeedpwm = 450;  //��Ҫ��֤pwmֵ����С��0
            FTM_PWM_Duty(FTM0, CH1, newspeedpwm - speeddif);  //PTC1 С��ǰ��
            FTM_PWM_Duty(FTM0, CH3, newspeedpwm + speeddif);  //PTC3 С��ǰ��
        }
        else if (nowspeed < setspeed - 30) //�ٶȹ���ʱ����,��С������ʱ�Ʋ�������Ҫ��PWM
        {
            newspeedpwm = dianjispeed + speedkp * (setspeed - nowspeed) / 4;// - speedkd*(dspeed - dspeedold);
            if (nowspeed < 70) newspeedpwm = newspeedpwm + 400;
            if (newspeedpwm > 3000)
                newspeedpwm = 3000;
            FTM_PWM_Duty(FTM0, CH1, newspeedpwm - speeddif);  //PTC1 С��ǰ��
            FTM_PWM_Duty(FTM0, CH3, newspeedpwm + speeddif);  //PTC3 С��ǰ��
        }
        else //�����ٶ�
        {
            newspeedpwm = dianjispeed;
            FTM_PWM_Duty(FTM0, CH1, newspeedpwm - speeddif);  //PTC1 С��ǰ��
            FTM_PWM_Duty(FTM0, CH3, newspeedpwm + speeddif);  //PTC3 С��ǰ��
        }
        //	 speedErr = setspeed - nowspeed;
        //	 if(speedErr >= 30 || speedErr <= -30)
        //		 speedErrPre = speedErrPre + speedErr;
        //	 
        //	 newspeedpwm = dianjispeed + speedkp * speedErr + speedki * speedErrPre;
        //	 if (newspeedpwm < 450)  newspeedpwm = 450;  //��Ҫ��֤pwmֵ����С��0
        //	 if (newspeedpwm > 4000) newspeedpwm = 4000;
        //	 FTM_PWM_Duty(FTM0, CH1, newspeedpwm - speeddif);  //PTC1 С��ǰ��
        //	 FTM_PWM_Duty(FTM0, CH3, newspeedpwm + speeddif);  //PTC3 С��ǰ��
        //	 speedErrPre2 = speedErrPre;
            // speedErrPre = speedErr;
    }
}










//  adc_init(ADC1,AD4a);  //��ص�ѹ E0
//  adc_init(ADC1,AD5a);  //��ص�ѹ E1
//  adc_init(ADC1,AD6a);  //��ص�ѹ E2

void newroadcal()  //�¶�����Ƴ���
{
    //AD_val_3=100*(AD_3 - AD_val_3_min)/(AD_val_3_max-AD_val_3_min);

   // disgy_AD_val_1 = AD_val_1;
   // disgy_AD_val_2 = AD_val_2;
    //disgy_AD_val_3 = AD_val_3;

   // if (AD_val_2 >= 99) {  /////////////����omu
  //if(omuLtState == 0)
  //  omuLtState = 1;
  //if (omuLtState == 2)
  //	omuLtState = 3;
   // }
   // else {
  //  if (omuLtState == 1)
  //	  omuLtState = 2;
  //  if (omuLtState == 3)
  //	  omuLtState = 4;
   // }
   // if (AD_val_1 >= 99) {  /////////////����omu
  //  if (omuRtState == 0)
  //	  omuRtState = 1;
  //  if (omuRtState == 2)
  //	  omuRtState = 3;
   // }
   // else {
  //  if (omuRtState == 1)
  //	  omuRtState = 2;
  //  if (omuRtState == 3)
  //	  omuRtState = 4;
   // }
   // if (omuLtState == 4 && omuRtState == 3)////////////////����omu
  //  omuLeftFlag = 1;
   // else omuLeftFlag = 0;
   // if (omuLtState == 3 && omuRtState == 4)////////////////����omu
  //  omuRightFlag = 1;
   // else omuRightFlag = 0;

    if (AD_val_2 - AD_val_1 >= 20)  Dir_last = 1;   //��һ״̬Ϊ��ת
    if (AD_val_1 - AD_val_2 >= 20)  Dir_last = 0;   //��һ״̬Ϊ��ת

    if (Dir_last == 2)//֮ǰ״̬��������
    {
        if ((AD_val_1 < 20 && (AD_val_1 > AD_val_2)) || AD_val_2 == 0 || (AD_val_1 - AD_val_2) > 30)//Right Miss;
            Dir_last = 1;
        if ((AD_val_2 < 20 && (AD_val_2 > AD_val_1)) || AD_val_1 == 0 || (AD_val_2 - AD_val_1) > 30)//Left Miss
            Dir_last = 0;
    }
    //��������
    if (AD_val_1 > 20 || AD_val_2 > 20)    //
    {
        Dir_last = 2;
        dir_error_pre = dir_error;                                            //�ϸ�ƫ��
        dir_error = (AD_val_2 - AD_val_1);          //ƫ����
        dir_error = dir_error * (dir_error * dir_error / 1250.0 + 2) / 10;
        dis_error = dir_error;
        DuoP = rkp3;
        DuoD = rkd3;
        servPWMDuty = DuoCenter - DuoP * dir_error - DuoD * (dir_error - dir_error_pre) / 10;
        setspeed = setspeeddianci;
    }
    if (Dir_last == 1)  // Right Miss
    {
        servPWMDuty = servMotorLeft;   //������ת
        setspeed = setspeeddiancilow;
        DuoP = rkp3;
        DuoD = rkd3;
    }
    if (Dir_last == 0)  //Left Miss
    {
        servPWMDuty = servMotorRight;		//������ת
        setspeed = setspeeddiancilow;
        DuoP = rkp3;
        DuoD = rkd3;
    }

}

void getdcinit()  //����������Сֵ
{
    unsigned int i, j, sum;
    //������е���Сֵ
    j = 0;
    /*LCD_P6x8Str(20,3,"Get Min Val");
    for(i=0;i<50;i++)   //Left-Min
    {
      AD_val_1 = ad_once(ADC1, AD4a, ADC_10bit);
      sum+=AD_val_1;
      delayms(2);
    }
    AD_val_1_min=sum/50;
    sum=0;
    for(i=0;i<50;i++)   //Right-Min
    {
      AD_val_2 = ad_once(ADC1, AD6a, ADC_10bit);
      sum+=AD_val_2;
      delayms(2);
    }
    AD_val_2_min=sum/50;
    sum=0;
    for(i=0;i<50;i++)   //Middle-Min
    {
      AD_val_3 = ad_once(ADC1, AD5a, ADC_10bit);
      sum+=AD_val_3;
      delayms(2);
    }
    AD_val_3_min=sum/50;
    sum=0;


    //������е����ֵ
    LCD_P6x8Str(20,3,"Get Max Val");
    for(i=0;i<1500;i++)
    {
      AD_val_1 = ad_once(ADC1, AD4a, ADC_10bit);
      if(AD_val_1>=AD_val_1_max)
        AD_val_1_max=AD_val_1;
      delayms(1);
      AD_val_2 = ad_once(ADC1, AD6a, ADC_10bit);
      if(AD_val_2>=AD_val_2_max)
        AD_val_2_max=AD_val_2;
      delayms(1);
      AD_val_3 =  ad_once(ADC1, AD5a, ADC_10bit);
      if(AD_val_3>=AD_val_3_max)
        AD_val_3_max=AD_val_3;
      delayms(1);
    }
    */
    //
    LCD_P6x8Str(0, 0, "LMax:      LMin:");
    LCD_P6x8Str(0, 1, "RMax:      RMin:");
    LCD_P6x8Str(0, 2, "CMax:      CMin:");
    Dis_num(32, 0, AD_val_1_max);
    Dis_num(96, 0, AD_val_1_min);
    Dis_num(32, 1, AD_val_2_max);
    Dis_num(96, 1, AD_val_2_min);
    Dis_num(32, 2, AD_val_3_max);
    Dis_num(96, 2, AD_val_3_min);

    while (1)  //��ʾ��ǰ��й�һ��ֵ���鿴��һ��ֵ�Ƿ���ȷ���ȴ����IPE6���º�С������
    {
        adtmp1 = 0;
        adtmp2 = 0;
        adtmp3 = 0;
        for (i = 0; i < 40; i++) {
            AD_val_1 = ad_ave(ADC1, AD13, ADC_12bit, 30);
            AD_val_2 = ad_ave(ADC1, AD8, ADC_12bit, 30);
            //AD_val_3 = ad_ave(ADC1, AD5a, ADC_12bit,30);
            adtmp1 = adtmp1 + AD_val_1;
            adtmp2 = adtmp2 + AD_val_2;
            //adtmp3= adtmp3+ AD_val_3;
        }
        AD_val_1 = adtmp1 / 40;
        AD_val_2 = adtmp2 / 40;
        //AD_val_3 = adtmp3/40;

        if (AD_val_1 > AD_val_1_max)
            AD_val_1 = AD_val_1_max;
        if (AD_val_2 > AD_val_2_max)
            AD_val_2 = AD_val_2_max;
        //if(AD_val_3>AD_val_3_max)		AD_val_3=AD_val_3_max;

        if (AD_val_1 < AD_val_1_min)
            AD_val_1 = AD_val_1_min;
        if (AD_val_2 < AD_val_2_min)
            AD_val_2 = AD_val_2_min;
        //if(AD_val_3<AD_val_3_min)		AD_val_3=AD_val_3_min;

         //��һ��
        dis_AD_val_1 = 100 * (AD_val_1 - AD_val_1_min) / (AD_val_1_max - AD_val_1_min);
        dis_AD_val_2 = 100 * (AD_val_2 - AD_val_2_min) / (AD_val_2_max - AD_val_2_min);
        //dis_AD_val_3=100*(AD_val_3 - AD_val_3_min)/(AD_val_3_max-AD_val_3_min);

        LCD_P6x8Str(0, 3, "Val:");
        Dis_num(0, 4, AD_val_1);
        //Dis_num(40,4,AD_val_3);
        Dis_num(80, 4, AD_val_2);
        LCD_P6x8Str(0, 5, "GuiYi:");
        Dis_num(0, 6, dis_AD_val_1);
        //Dis_num(40,6,dis_AD_val_3);
        Dis_num(80, 6, dis_AD_val_2);
        delayms(300);
        if (gpio_get(PORTA, 9) == 0) {
            delayms(5);
            if (gpio_get(PORTA, 9) == 0) {
                while (gpio_get(PORTA, 9) == 0)
                    continue;
                break;
            }
        }
        j++;
        //Dis_num(80,7,j);
    }
    LCD_CLS();  //����
}











void biZhang() {                        //���ϳ���
    //ii = ad_ave(ADC1, AD15, ADC_12bit, 30);//������
    //if (0/*ii >= HONGWAIZHANGAI && biZhangFlag == 0*/ ) {//�����������
    //	biZhangFlag = 1;
    //}
    //if (biZhangFlag == 1) {
    //	julicnt++;
    //				outBZFlag = 1;
    //	if(julicnt > 2 * (BZDelay + BZOUTDELAY) + 240)
    //		if (AD_val_1 > 40 || AD_val_2 > 40) {
    //			biZhangFlag = 2;
    //		}
    //}

    //if (omuFlag == 0 && AD_val_1 >= OMUDIANCI && AD_val_2 >= OMUDIANCI) {
    //	omuCount++;
    //}
    //else {
    //	omuCount = 0;//C.C.C.C
    //}
    //if (omuCount > OMUDELAY) {
    //	omuFlag = 1;
    //}
    ///////////////////////////�뻷end
    //if (omuFlag == 1) {
    //	omucnt++;
    //}
    ///////////////////////////����
    //if (omuFlag == 2) {
    //	if (AD_val_2 >= OMUDIANCI || AD_val_1 >= OMUDIANCI) {
    //		omuOutCount++;
    //	}
    //	if (omuOutCount > OMUFLAGOUTDELAY) {
    //		omuOutFlag = 1;
    //	}
    //}
    //////////////////////////����end
    //if (omuOutFlag == 1) {
    //	omucnt++;
    //}
    //	//DisableInterrupts;//�ر������ж�
    //	//disable_irq(85);//�ر�PIT1�ж�
    //	//if(BIZHANGFANGXIANG){//�ұ���
    //	//	
    //	//	delayms(BZDelayMs);//��ʱ
    //	//	FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//�������
    //	//	delayms(BZDelayMs);//��ʱ
    //	//}
    //	//else {//�����
    //	//	FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//�������
    //	//	delayms(BZDelayMs);//��ʱ
    //	//	FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);//�������
    //	//	delayms(BZDelayMs);//��ʱ
    //	//}
    //	//biZhangFlag = 2;
    //	//EnableInterrupts;//���ж�


}










void biZhangDuo() {                     //����
    //#if BIZHANGFANGXIANG
    //	if (julicnt < BZDelay - BZOUTDELAY) {
    //		endianci = 1;
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);	//�������
    //	}
    //	else if (julicnt < 2 * (BZDelay - BZOUTDELAY)) {
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//�������
    //	}
    //	else if (julicnt < 2 * (BZDelay - BZOUTDELAY)) {
    //		setspeed = 20;
    //		FTM_PWM_Duty(FTM2, CH0, DuoCenter);
    //	}
    //	else if (julicnt < 2 * (BZDelay + BZOUTDELAY) + 100) {
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//�������
    //		setspeed = 50;
    //	}
    //	else if (julicnt < 2 * (BZDelay + BZOUTDELAY) + 240)
    //	{
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);//�������
    //		Dir_last = 0;
    //	}
    //	else {
    //		biZhangFlag = 2;
    //		//		endianci = 0;
    //	}
    //#else
    //
    //	if (julicnt < BZDelay - BZOUTDELAY) {
    //		endianci = 1;
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);	//�������
    //	}
    //	else if (julicnt < 2 * (BZDelay - BZOUTDELAY)) {
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);//�������
    //	}
    //	else if (julicnt < 2 * (BZDelay - BZOUTDELAY)) {
    //		setspeed = 20;
    //		FTM_PWM_Duty(FTM2, CH0, DuoCenter);
    //	}
    //	else if (julicnt < 2 * (BZDelay + BZOUTDELAY) + 100) {
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);//�������
    //		setspeed = 50;
    //	}
    //	else if (julicnt < 2 * (BZDelay + BZOUTDELAY) + 240)
    //	{
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//�������
    //		Dir_last = 1;
    //	}
    //	else {
    //		biZhangFlag = 2;
    //		//		endianci = 0;
    //	}
    //#endif
}




void OMuDuo(void) {                 //ŷķ��
    //#if ZuoOMU
    //	if(omucnt>0)
    //	FTM_PWM_Duty(FTM2, CH0, servMotorRight);//��
    //
    //
    //#else
    //	if (omucnt > 0) {
    //		FTM_PWM_Duty(FTM2, CH0, servMotorLeft);//��
    //	}
    //	else if (omucnt > OMUINDELAY) {
    //		endianci = ENOUMUDIANCI;
    //		omuFlag = 2;
    //		omucnt = 0;
    //	}
    //
    //#endif
}
void OMuOutDuo() {                     //ŷķ����
    //#if ZuoOMU
    //
    //
    //
    //#else
    //	if (omucnt > 0) {
    //#if OMUOUTDUOFLAG
    //		FTM_PWM_Duty(FTM2, CH0, DuoCenter);//��
    //#else
    //		FTM_PWM_Duty(FTM2, CH0, servMotorCenture);//��
    //#endif
    //	}
    //	else if (omucnt > OMUOUTDELAY) {
    //		endianci = ENOUMUDIANCI;
    //		omuFlag = 2;
    //		omucnt = 0;
    //	}
    //
    //
    //#endif
}