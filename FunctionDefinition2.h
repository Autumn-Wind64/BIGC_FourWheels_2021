





void onebeep()
{
    gpio_set(PORTB, 20, 0);
    delayms(40);
    gpio_set(PORTB, 20, 1);
}



void datatrans()  //�������ݷ��뺯��
{
    //����ͷ�������ݷ���
    uint16 m, n;
    uint16 sum[5];  //�ж��м�5���Ƿ�Ϊȫ�� 
    uint16 sum1[5]; //�ж����5���Ƿ�Ϊȫ�� 
    uint8 colour[2] = { 0, 240 }; //0 �� 1 �ֱ��Ӧ����ɫ
//  uint8 cmostmp[60];
    for (m = 0; m < cmosrow; m++)  //
    {
        for (n = 0; n < cmoscolbyte; n++) //  
        {
            cmos[m][8 * n] = colour[(Buffer1[m * COL + n] >> 7) & 0x01];
            cmos[m][8 * n + 1] = colour[(Buffer1[m * COL + n] >> 6) & 0x01];
            cmos[m][8 * n + 2] = colour[(Buffer1[m * COL + n] >> 5) & 0x01];
            cmos[m][8 * n + 3] = colour[(Buffer1[m * COL + n] >> 4) & 0x01];
            cmos[m][8 * n + 4] = colour[(Buffer1[m * COL + n] >> 3) & 0x01];
            cmos[m][8 * n + 5] = colour[(Buffer1[m * COL + n] >> 2) & 0x01];
            cmos[m][8 * n + 6] = colour[(Buffer1[m * COL + n] >> 1) & 0x01];
            cmos[m][8 * n + 7] = colour[(Buffer1[m * COL + n] >> 0) & 0x01];
        }
    }

    //   ����ͷ �� ����л� ������Ǵ�����ͷ��ע����γ��� ////////////////////////////////////////////
    //      �ж��м�һЩ���Ƿ�ȫ�ڣ����ȫ���л������    ////////////////////////////////////////

    for (n = 0; n < 5; n++) sum[n] = 0;
    for (n = 20; n < 25; n++) //ͼ��ǰ���ж�
    {
        for (m = 5; m < 75; m++)
        {
            if (cmos[n][m] > 10)
                sum[n - 20] = sum[n - 20] + 1;  //ÿ�а׵����
        }
    }

    //  for(n=0;n<5;n++) sum1[n]=0;
    //  for(n=45;n<50;n++) //ͼ�񿿺��ж�
    //  {  for(m=5;m<75;m++) 
    //     {  if(cmos[n][m]>10) 
    //          sum1[n-45] = sum1[n-45] + 1;  //ÿ�а׵����
    //     }
    //  }
#if ENSHESHATOU
    if (endianci == 0)
    {
        if ((sum[0] < 4) && (sum[1] < 4) && (sum[2] < 4) && (sum[3] < 4) && (sum[4] < 4))  //5��ȫ�ڣ�ÿ�����4���׵㣩
            endianci = 1;  //�л������
    }

    else //
    {
        if ((sum1[0] > 60) && (sum1[1] > 60) && (sum1[2] > 60) && (sum1[3] > 60) && (sum1[4] > 60))  //5������
            endianci = 0;   //����ͷ����
    }
#else
#endif
//////////////  �ж�����  //////////////////////////////////////////  

}

void imgtrans()     //
{
    uint16 m, n;
    for (m = 0; m < CAMERA_SIZE; m++)  //600
    {
        imgbuff_process[m] = Buffer1[m];
    }
}

void oledtrans()
{
    uint16 m, n, k;
    u8 cval;
    u8 midval;
    cval = 0;
    for (m = 0; m < 7; m++)  //row 7*8 =56 ��
    {
        for (n = 0; n < 80; n++)  //col
        {
            for (k = 0; k < 8; k++)
            {
                if (cmos[m * 8 + k][n] > 0)
                {
                    if (k == 0) cval = cval + 1;
                    else if (k == 1) cval = cval + 2;
                    else if (k == 2) cval = cval + 4;
                    else if (k == 3) cval = cval + 8;
                    else if (k == 4) cval = cval + 16;
                    else if (k == 5) cval = cval + 32;
                    else if (k == 6) cval = cval + 64;
                    else if (k == 7) cval = cval + 128;
                }
            }
            oledcmos[m][n] = cval;
            cval = 0;
        }
    }
    cval = 0;
    for (n = 0; n < 80; n++)  //���һ��
    {
        for (k = 0; k < 4; k++)
        {
            if (cmos[56 + k][n] > 0)
            {
                if (k == 0) cval = cval + 1;
                else if (k == 1) cval = cval + 2;
                else if (k == 2) cval = cval + 4;
                else if (k == 3) cval = cval + 8;
            }
        }
        oledcmos[7][n] = cval;
        cval = 0;
    }
    for (m = 0; m < ROW; m++) //��������ʾ����
    {
        midval = LMR[1][m]; //ĳ�е�����ֵ
        n = m / 8;  //
        k = m % 8;
        oledcmos[n][midval] = oledcmos[n][midval] & ~(1 << k);
    }
}

void oledimg()
{
    uint16 x;
    u8 y;
    for (y = 0; y < 7; y++)
    {
        LCD_Set_Pos(0, y);
        for (x = 0; x < 80; x++)
        {
            LCD_WrDat(oledcmos[y][x]);
        }
    }
}

void ImageProc()            //ͼ�������
{
    u32 i, j;
    DisableInterrupts;            //��ֹ�ж�
    datatrans(); //��Buffer1�������Ƶ�cmos
    oledtrans();
    if (endianci == 0) //����ͷ����
    {
        imgtrans();
        get_edge();  //�ұ߽�
        Search();   //����С��λ��
        if ((row_mid > 48) || (row_mid < 32))  //����
        {
            servPram = servPram3;
            dPram = dPram3;
            setspeed = setspeedlow;  //����
        }
        else if ((row_mid > 45) || (row_mid < 35))  //����
        {
            servPram = servPram2;
            dPram = dPram2;
            setspeed = setspeedlow + 5;
        }
        else if ((row_mid > 43) || (row_mid < 37))  //С��
        {
            servPram = servPram1;
            dPram = dPram1;
            setspeed = setspeedlow + 10;
        }
        else    //ֱ��
        {
            servPram = servPram0;
            dPram = dPram0;
            setspeed = setspeedhigh;  //����
        }
        /// ����ת����ٶ�
         //ygout=MPU6050_GetDoubleData(MPU6050_GYRO_YOUT);//��Y����ٶȣ�ת����ٶ�
         //Turn_Speed =  (ygout - GYROSCOPE_OFFSET_Y)/70;//*0.048;  //X����ٶ����ֵ, 500��--> 65.5 = 65535/1000
        disTurn_Speed = Turn_Speed * 1;
        //---------------������ת��-----
        if (DuoDir == 0)
            servPWMDuty = DuoCenter + (row_mid - 40) * servPram / 2 + (row_mid - row_midold) * dPram / 2 + TurnD * Turn_Speed;
        else
            servPWMDuty = DuoCenter - (row_mid - 40) * servPram / 2 - (row_mid - row_midold) * dPram / 2 - TurnD * Turn_Speed;
        row_midold = row_mid;
        if (servPWMDuty <= servMotorLeft)  servPWMDuty = servMotorLeft;
        if (servPWMDuty >= servMotorRight)  servPWMDuty = servMotorRight;
        if (workmode == 0) {
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
                FTM_PWM_Duty(FTM2, CH0, DuoCenter);  //���Զ����ֵ
        }
        else {
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

void get_edge()   //Ѱ��ͼ���Ե�������������ٳ˷����㣩
{
    static int16 i = 0, j = 0, last = 0, x = 0, y = 0, n = 0;
    uint8 temp = 0, find = 0;
    cont = 0;
    for (i = 0; i < 60; i++)
    {
        last = 0;
        x = i * 10;
        find = 0;
        edgposition[i] = 0;
        for (j = 0; j < 10; j++)
        {
            if (imgbuff_process[x + j] == 0xff)
            {
                if (last == 0)
                {
                    y = j << 3;
                    if (find == 0)
                    {
                        edgposition[i] = cont;
                    }
                    img_edg[cont++] = y;   //���ƶ�5�൱�ڳ���32 ���ƶ�3�൱�ڳ���8 
                    find = 1;
                }
                last = 1;
                continue;
            }
            if (imgbuff_process[x + j] == 0)
            {
                if (last == 1)
                {
                    y = j << 3;
                    if (find == 0)
                    {
                        edgposition[i] = cont;
                    }
                    img_edg[cont++] = y;   //���ƶ�5�൱�ڳ���32 ���ƶ�3�൱�ڳ���8  
                    find = 1;
                }
                last = 0;
                continue;
            }

            for (n = 7; n >= 0; n--)
            {
                temp = (imgbuff_process[x + j] >> n) & 1;// ��ȡ�õ�����ֵ ��0��1��     
                if (temp != last) //����һ��ֵ����ͬ ������������            
                {
                    y = j << 3;
                    if (find == 0)
                    {
                        edgposition[i] = cont;
                    }
                    img_edg[cont++] = y + 7 - n;   //���ƶ�5�൱�ڳ���32 ���ƶ�3�൱�ڳ���8 
                    find = 1;
                }
                last = temp;                //�洢�õ��ֵ
            }
        }
        img_edg[cont++] = 0xff;   //���ƶ�5�൱�ڳ���32 ���ƶ�3�൱�ڳ���8
    }
}
/*img_edg��һ��һά���� ��¼������ͷÿ�е������ص�����ֵ  ÿ�������� �������ؿ�ʼ���ɺڱ�ף�Ȼ����½��أ��ɰױ�ڣ�
   0xff����ָʾ���е����������ˣ���ʼ��¼��һ��
   ���ÿ��ͼ���԰�ɫ���ֿ�ʼ����ô������������ʼλ��Ϊ0
   �������ȫ�� ��ô���м�¼Ϊ0xff
   �������Ϊȫ�� ��¼Ϊ 0 0xff


  oxff����������ֵ������������һ��
 //edgposition[i]�����i�е� ������ �� img_edg ���������

 */







void Search()               //һ��һ�б���ͼ������ͼ���������ʾ���أ�������С��λ��
{
    //�ӵײ���������
    float Middle_Err_Sum = 0;
    float ave_Err;
    static int i, j, n, m, find;
    uint8 left_cont = 0, right_cont = 0;
    uint8* startedge;
    startedge = img_edg;
    static float slopeleft, sloperight, left_Pre, right_Pre;
    uint8 Turning_Line_Left = 0, Turning_Line_Right = 0;
    uint8 break_line_left = 0, break_line_right = 0;
    uint8 left_fix_begin = 0, left_fix_end = 0, right_fix_begin = 0, right_fix_end = 0;
    for (i = 0; i < 60; i++)  //�������
    {

        LMR[0][i] = 0; //���������
        LMR[1][i] = 0;  //��������
        LMR[2][i] = 80; //�ұ�������
    }
    for (i = 59; i > 0; i--) //�ӵ�59�п�ʼ����
    {
        if (edgposition[i] == 0 && (i != 0)) //ȫ���� ��Ϊ����
        {
            break;
        }

        j = edgposition[i];//���������ؿ�ʼ��λ��  j����ڱ�� ������� j+1����ٱ�� ���ұ��� 

        if (i == 59)  //�ײ���ʼ��
        {
            while (img_edg[j] != 255)
            {
                if ((img_edg[j] < 55) && (img_edg[j + 1] > 25))  //�����С��55 �ұ��ش���25
                {
                    if ((img_edg[j + 1] - img_edg[j]) > 25) //�ұ���-����ش���20
                    {
                        LMR[0][i] = img_edg[j];
                        if (img_edg[j + 1] == 255)
                        {
                            LMR[2][i] = 80;
                        }
                        else
                        {
                            LMR[2][i] = img_edg[j + 1];
                        }
                        break;//while
                    }
                }
                if (img_edg[j + 1] == 255)
                {
                    break;//while
                }
                j = j + 2;
            }
        }
        else   //���ǵײ���ʼ��
        {
            find = 0;
            while (img_edg[j] != 255)
            {
                if ((img_edg[j] <= LMR[2][i + 1]) && (img_edg[j + 1] >= LMR[0][i + 1]) && (img_edg[j + 1] - img_edg[j] > 3))     //�����С����һ�е��ұ��� �ұ��ش�����һ�е����������ͨ��
                {
                    find = 1;
                    if (LMR[0][i] == 0)
                    {
                        LMR[0][i] = img_edg[j];
                    }
                    if (img_edg[j + 1] != 255 && (LMR[2][i - 1] == 80))
                    {
                        LMR[2][i] = img_edg[j + 1];
                    }
                }

                if (img_edg[j + 1] == 255) //���е������ؽ�����
                {
                    break;//while
                }
                j = j + 2;
            }
            if (find == 0)//û���ҵ���ͨ����
            {
                i++;
                if ((left_fix_begin > 0) && (left_fix_begin <= n))
                {
                    for (n = left_fix_begin; n > 0; n--)
                    {
                        LMR[0][n] = 0; //���������
                    }
                }
                if ((right_fix_begin > 0) && (right_fix_begin <= n))
                {
                    for (n = right_fix_begin; n > 0; n--)
                    {
                        LMR[2][n] = 0; //���������
                    }
                }
                break;//for
            }
        }
    }
    Middle_Err_Sum = 0;
    for (; i < 59; i++)
    {
        LMR[1][i] = (LMR[0][i] + LMR[2][i]) / 2;
        if (i >= 35 && i < 40)
        {
            Middle_Err_Sum = Middle_Err_Sum + LMR[1][i] - 40;
        }
    }
    ave_Err = Middle_Err_Sum / 5;
    if ((Middle_Err - ave_Err > -2) || (Middle_Err - ave_Err < 2))
    {
        Middle_Err = ave_Err;//  for(i=59;i>0;i--) //�ӵ�59�п�ʼ����
    }
    else
    {
        if (Middle_Err < ave_Err)  Middle_Err = Middle_Err + 2;
        else Middle_Err = Middle_Err - 2;
    }
    row_mid = 40 + Middle_Err;  //С��λ��ֵ   
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