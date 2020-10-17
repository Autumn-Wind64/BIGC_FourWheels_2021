
void Image_Init()
{
    u32 i/*,j*/;
    for (i = 0; i < CAMERA_SIZE; i++)
    {
        Buffer1[i] = 0;
    }
}

void ImageAcquisition()
{
	Dis_num(90, 7, ii);//����ֵ1100
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
        if (i >= 35 && i < 40)  //LMR�Ƕ�ȡͼ��Ķ�ά���飬LMR[0][i]�������У�LMR[1][i]�������У�LMR[2][i]��������
        {                            //����i�����������������ֵ35-40�С�����������Χ����������if�Ĵ���Ҳ��Խ�࣬��Ӱ�쵽Middle_Err_Sum��ֵ�����Ӱ�쵽row_mid��ֵ��С���������ֵ�жϺ�ʱת���Լ�ת����ȣ�
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
