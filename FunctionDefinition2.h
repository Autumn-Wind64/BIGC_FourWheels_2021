





void onebeep()
{
    gpio_set(PORTB, 20, 0);
    delayms(40);
    gpio_set(PORTB, 20, 1);
}



void datatrans()  //传入数据翻译函数
{
    //摄像头传入数据翻译
    uint16 m, n;
    uint16 sum[5];  //判定中间5行是否为全黑 
    uint16 sum1[5]; //判定后端5行是否为全黑 
    uint8 colour[2] = { 0, 240 }; //0 和 1 分别对应的颜色
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

    //   摄像头 和 电磁切换 ，如果是纯摄像头，注释这段程序 ////////////////////////////////////////////
    //      判定中间一些行是否全黑，如果全黑切换到电磁    ////////////////////////////////////////

    for (n = 0; n < 5; n++) sum[n] = 0;
    for (n = 20; n < 25; n++) //图像靠前处判定
    {
        for (m = 5; m < 75; m++)
        {
            if (cmos[n][m] > 10)
                sum[n - 20] = sum[n - 20] + 1;  //每行白点个数
        }
    }

    //  for(n=0;n<5;n++) sum1[n]=0;
    //  for(n=45;n<50;n++) //图像靠后处判定
    //  {  for(m=5;m<75;m++) 
    //     {  if(cmos[n][m]>10) 
    //          sum1[n-45] = sum1[n-45] + 1;  //每行白点个数
    //     }
    //  }
#if ENSHESHATOU
    if (endianci == 0)
    {
        if ((sum[0] < 4) && (sum[1] < 4) && (sum[2] < 4) && (sum[3] < 4) && (sum[4] < 4))  //5行全黑（每行最多4个白点）
            endianci = 1;  //切换到电磁
    }

    else //
    {
        if ((sum1[0] > 60) && (sum1[1] > 60) && (sum1[2] > 60) && (sum1[3] > 60) && (sum1[4] > 60))  //5行正常
            endianci = 0;   //摄像头正常
    }
#else
#endif
//////////////  判定结束  //////////////////////////////////////////  

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
    for (m = 0; m < 7; m++)  //row 7*8 =56 行
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
    for (n = 0; n < 80; n++)  //最后一行
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
    for (m = 0; m < ROW; m++) //把中线显示出来
    {
        midval = LMR[1][m]; //某行的中线值
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

void ImageProc()            //图像处理程序
{
    u32 i, j;
    DisableInterrupts;            //禁止中断
    datatrans(); //把Buffer1的数据移到cmos
    oledtrans();
    if (endianci == 0) //摄像头正常
    {
        imgtrans();
        get_edge();  //找边界
        Search();   //计算小车位置
        if ((row_mid > 48) || (row_mid < 32))  //大弯
        {
            servPram = servPram3;
            dPram = dPram3;
            setspeed = setspeedlow;  //低速
        }
        else if ((row_mid > 45) || (row_mid < 35))  //中弯
        {
            servPram = servPram2;
            dPram = dPram2;
            setspeed = setspeedlow + 5;
        }
        else if ((row_mid > 43) || (row_mid < 37))  //小弯
        {
            servPram = servPram1;
            dPram = dPram1;
            setspeed = setspeedlow + 10;
        }
        else    //直道
        {
            servPram = servPram0;
            dPram = dPram0;
            setspeed = setspeedhigh;  //高速
        }
        /// 计算转向角速度
         //ygout=MPU6050_GetDoubleData(MPU6050_GYRO_YOUT);//读Y轴角速度，转向角速度
         //Turn_Speed =  (ygout - GYROSCOPE_OFFSET_Y)/70;//*0.048;  //X轴角速度输出值, 500度--> 65.5 = 65535/1000
        disTurn_Speed = Turn_Speed * 1;
        //---------------计算舵机转角-----
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
                FTM_PWM_Duty(FTM2, CH0, DuoCenter);  //测试舵机中值
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


void run()  // 5ms执行一次 ，每30ms测一次速度 ，每5ms执行电磁检测
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

    //AD_1 = ad_once(ADC1, AD4a, ADC_12bit);      //最左边电感电压   PE0
    //AD_2 = ad_once(ADC1, AD9, ADC_12bit);      //最右边电感电压   PB1
    //AD_3 = ad_once(ADC1, AD5a, ADC_12bit);      //中间电感电压     PE1

    AD_1 = ad_once(ADC1, AD13, ADC_12bit);      //最左边电感电压   PB0
    AD_2 = ad_once(ADC1, AD8, ADC_12bit);      //最右边电感电压   PB7



    dis_AD_val_1 = AD_1;
    dis_AD_val_2 = AD_2;
    //dis_AD_val_3=AD_3;

    //限幅
    if (AD_1 > AD_val_1_max)		AD_1 = AD_val_1_max;
    if (AD_2 > AD_val_2_max)		AD_2 = AD_val_2_max;
    //if(AD_3>AD_val_3_max)		AD_3=AD_val_3_max;

    if (AD_1 < AD_val_1_min)		AD_1 = AD_val_1_min;
    if (AD_2 < AD_val_2_min)		AD_2 = AD_val_2_min;
    //if(AD_3<AD_val_3_min)		AD_3=AD_val_3_min;

    //归一化
    AD_val_1 = 100 * (AD_1 - AD_val_1_min) / (AD_val_1_max - AD_val_1_min);
    AD_val_2 = 100 * (AD_2 - AD_val_2_min) / (AD_val_2_max - AD_val_2_min);
    ////////////////////////////入环

    if (endianci == 1) //摄像头看不到图像时，采用电磁导向
    {
        setspeed = setspeeddianci; //一定低速，电磁位置太靠后
        newroadcal();
        if (servPWMDuty <= servMotorLeft)  servPWMDuty = servMotorLeft;
        if (servPWMDuty >= servMotorRight)  servPWMDuty = servMotorRight;
        if (workmode == 0) { // 正常状态
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
        else { //调试状态
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

    if (mycnt >= 4)  //每20ms测速
    {
        nowrightspeed = rightspeed;
        nowleftspeed = leftspeed;
        rightspeed = 0;
        leftspeed = 0;
        mycnt = 0;
        nowspeed = (nowleftspeed + nowrightspeed) / 2;  //双电机时取两路的平均值
        speeddif = 0;
        if (endianci == 0) {  //摄像头正常时差速处理
            if (row_mid < 35)
                speeddif = 400;//180 ;

            else if (row_mid > 45)  //弯道
                speeddif = -400; //180;

        }
        else { //电磁工作时差速处理
            if ((AD_val_2 > 90) && (AD_val_2 - AD_val_1 >= 60))
                speeddif = -400;//180;      
            else if ((AD_val_1 > 90) && (AD_val_1 - AD_val_2 >= 20))
                speeddif = 400;//-180;       
        }

        //////  单、双驱动通用  ////////////////////////////////////////////////////



        if (nowspeed > setspeed + 6) //速度过快，需要减速
        {
            newspeedpwm = dianjispeed + speedkp * (setspeed - nowspeed);  //100//speedErr
            if (newspeedpwm < 450) newspeedpwm = 450;  //需要保证pwm值不能小于0
            FTM_PWM_Duty(FTM0, CH1, newspeedpwm - speeddif);  //PTC1 小车前进
            FTM_PWM_Duty(FTM0, CH3, newspeedpwm + speeddif);  //PTC3 小车前进
        }
        else if (nowspeed < setspeed - 30) //速度过慢时加速,在小车急弯时推不动，需要加PWM
        {
            newspeedpwm = dianjispeed + speedkp * (setspeed - nowspeed) / 4;// - speedkd*(dspeed - dspeedold);
            if (nowspeed < 70) newspeedpwm = newspeedpwm + 400;
            if (newspeedpwm > 3000)
                newspeedpwm = 3000;
            FTM_PWM_Duty(FTM0, CH1, newspeedpwm - speeddif);  //PTC1 小车前进
            FTM_PWM_Duty(FTM0, CH3, newspeedpwm + speeddif);  //PTC3 小车前进
        }
        else //正常速度
        {
            newspeedpwm = dianjispeed;
            FTM_PWM_Duty(FTM0, CH1, newspeedpwm - speeddif);  //PTC1 小车前进
            FTM_PWM_Duty(FTM0, CH3, newspeedpwm + speeddif);  //PTC3 小车前进
        }
        //	 speedErr = setspeed - nowspeed;
        //	 if(speedErr >= 30 || speedErr <= -30)
        //		 speedErrPre = speedErrPre + speedErr;
        //	 
        //	 newspeedpwm = dianjispeed + speedkp * speedErr + speedki * speedErrPre;
        //	 if (newspeedpwm < 450)  newspeedpwm = 450;  //需要保证pwm值不能小于0
        //	 if (newspeedpwm > 4000) newspeedpwm = 4000;
        //	 FTM_PWM_Duty(FTM0, CH1, newspeedpwm - speeddif);  //PTC1 小车前进
        //	 FTM_PWM_Duty(FTM0, CH3, newspeedpwm + speeddif);  //PTC3 小车前进
        //	 speedErrPre2 = speedErrPre;
            // speedErrPre = speedErr;
    }
}

void get_edge()   //寻找图像边缘函数（尽量减少乘法运算）
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
                    img_edg[cont++] = y;   //左移动5相当于乘以32 左移动3相当于乘以8 
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
                    img_edg[cont++] = y;   //左移动5相当于乘以32 左移动3相当于乘以8  
                    find = 1;
                }
                last = 0;
                continue;
            }

            for (n = 7; n >= 0; n--)
            {
                temp = (imgbuff_process[x + j] >> n) & 1;// 获取该点像素值 （0或1）     
                if (temp != last) //与上一个值不相同 出现了跳变沿            
                {
                    y = j << 3;
                    if (find == 0)
                    {
                        edgposition[i] = cont;
                    }
                    img_edg[cont++] = y + 7 - n;   //左移动5相当于乘以32 左移动3相当于乘以8 
                    find = 1;
                }
                last = temp;                //存储该点的值
            }
        }
        img_edg[cont++] = 0xff;   //左移动5相当于乘以32 左移动3相当于乘以8
    }
}
/*img_edg是一个一维数组 记录了摄像头每行的跳变沿的坐标值  每行跳变沿 由上升沿开始（由黑变白）然后接下降沿（由白变黑）
   0xff用于指示该行的跳变沿完了，开始记录下一行
   如果每行图像以白色部分开始，那么该行跳变沿起始位置为0
   如果该行全黑 那么该行记录为0xff
   如果该行为全白 记录为 0 0xff


  oxff代表本行坐标值结束，进入下一行
 //edgposition[i]代表第i行的 跳变沿 在 img_edg 中坐标起点

 */







void Search()               //一行一行遍历图像（这里图像用数组表示像素）并分析小车位置
{
    //从底部往上搜线
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
    for (i = 0; i < 60; i++)  //清空数组
    {

        LMR[0][i] = 0; //左边线数列
        LMR[1][i] = 0;  //中线数列
        LMR[2][i] = 80; //右边线数列
    }
    for (i = 59; i > 0; i--) //从第59行开始搜线
    {
        if (edgposition[i] == 0 && (i != 0)) //全黑行 置为丢线
        {
            break;
        }

        j = edgposition[i];//该行跳变沿开始的位置  j代表黑变白 即左边线 j+1代表百变黑 即右边线 

        if (i == 59)  //底部开始行
        {
            while (img_edg[j] != 255)
            {
                if ((img_edg[j] < 55) && (img_edg[j + 1] > 25))  //左边沿小于55 右边沿大于25
                {
                    if ((img_edg[j + 1] - img_edg[j]) > 25) //右边沿-左边沿大于20
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
        else   //不是底部开始行
        {
            find = 0;
            while (img_edg[j] != 255)
            {
                if ((img_edg[j] <= LMR[2][i + 1]) && (img_edg[j + 1] >= LMR[0][i + 1]) && (img_edg[j + 1] - img_edg[j] > 3))     //左边沿小于上一行的右边线 右边沿大于上一行的左边线是连通域；
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

                if (img_edg[j + 1] == 255) //该行的跳变沿结束了
                {
                    break;//while
                }
                j = j + 2;
            }
            if (find == 0)//没有找到连通区域
            {
                i++;
                if ((left_fix_begin > 0) && (left_fix_begin <= n))
                {
                    for (n = left_fix_begin; n > 0; n--)
                    {
                        LMR[0][n] = 0; //左边线数列
                    }
                }
                if ((right_fix_begin > 0) && (right_fix_begin <= n))
                {
                    for (n = right_fix_begin; n > 0; n--)
                    {
                        LMR[2][n] = 0; //左边线数列
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
        Middle_Err = ave_Err;//  for(i=59;i>0;i--) //从第59行开始搜线
    }
    else
    {
        if (Middle_Err < ave_Err)  Middle_Err = Middle_Err + 2;
        else Middle_Err = Middle_Err - 2;
    }
    row_mid = 40 + Middle_Err;  //小车位置值   
}

//  adc_init(ADC1,AD4a);  //电池电压 E0
//  adc_init(ADC1,AD5a);  //电池电压 E1
//  adc_init(ADC1,AD6a);  //电池电压 E2

void newroadcal()  //新舵机控制程序
{
    //AD_val_3=100*(AD_3 - AD_val_3_min)/(AD_val_3_max-AD_val_3_min);

   // disgy_AD_val_1 = AD_val_1;
   // disgy_AD_val_2 = AD_val_2;
    //disgy_AD_val_3 = AD_val_3;

   // if (AD_val_2 >= 99) {  /////////////神奇omu
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
   // if (AD_val_1 >= 99) {  /////////////神奇omu
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
   // if (omuLtState == 4 && omuRtState == 3)////////////////神奇omu
  //  omuLeftFlag = 1;
   // else omuLeftFlag = 0;
   // if (omuLtState == 3 && omuRtState == 4)////////////////神奇omu
  //  omuRightFlag = 1;
   // else omuRightFlag = 0;

    if (AD_val_2 - AD_val_1 >= 20)  Dir_last = 1;   //上一状态为右转
    if (AD_val_1 - AD_val_2 >= 20)  Dir_last = 0;   //上一状态为左转

    if (Dir_last == 2)//之前状态是正常的
    {
        if ((AD_val_1 < 20 && (AD_val_1 > AD_val_2)) || AD_val_2 == 0 || (AD_val_1 - AD_val_2) > 30)//Right Miss;
            Dir_last = 1;
        if ((AD_val_2 < 20 && (AD_val_2 > AD_val_1)) || AD_val_1 == 0 || (AD_val_2 - AD_val_1) > 30)//Left Miss
            Dir_last = 0;
    }
    //正常处理
    if (AD_val_1 > 20 || AD_val_2 > 20)    //
    {
        Dir_last = 2;
        dir_error_pre = dir_error;                                            //上个偏差
        dir_error = (AD_val_2 - AD_val_1);          //偏差量
        dir_error = dir_error * (dir_error * dir_error / 1250.0 + 2) / 10;
        dis_error = dir_error;
        DuoP = rkp3;
        DuoD = rkd3;
        servPWMDuty = DuoCenter - DuoP * dir_error - DuoD * (dir_error - dir_error_pre) / 10;
        setspeed = setspeeddianci;
    }
    if (Dir_last == 1)  // Right Miss
    {
        servPWMDuty = servMotorLeft;   //保持右转
        setspeed = setspeeddiancilow;
        DuoP = rkp3;
        DuoD = rkd3;
    }
    if (Dir_last == 0)  //Left Miss
    {
        servPWMDuty = servMotorRight;		//保持左转
        setspeed = setspeeddiancilow;
        DuoP = rkp3;
        DuoD = rkd3;
    }

}

void getdcinit()  //读电磁最大最小值
{
    unsigned int i, j, sum;
    //检测各电感的最小值
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


    //检测各电感的最大值
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

    while (1)  //显示当前电感归一化值，查看归一化值是否正确，等待按鍵PE6按下后小车出发
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

         //归一化
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
    LCD_CLS();  //清屏
}











void biZhang() {                        //避障程序
    //ii = ad_ave(ADC1, AD15, ADC_12bit, 30);//红外检测
    //if (0/*ii >= HONGWAIZHANGAI && biZhangFlag == 0*/ ) {//进入神奇避障
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
    ///////////////////////////入环end
    //if (omuFlag == 1) {
    //	omucnt++;
    //}
    ///////////////////////////出环
    //if (omuFlag == 2) {
    //	if (AD_val_2 >= OMUDIANCI || AD_val_1 >= OMUDIANCI) {
    //		omuOutCount++;
    //	}
    //	if (omuOutCount > OMUFLAGOUTDELAY) {
    //		omuOutFlag = 1;
    //	}
    //}
    //////////////////////////出环end
    //if (omuOutFlag == 1) {
    //	omucnt++;
    //}
    //	//DisableInterrupts;//关闭所有中断
    //	//disable_irq(85);//关闭PIT1中断
    //	//if(BIZHANGFANGXIANG){//右避障
    //	//	
    //	//	delayms(BZDelayMs);//延时
    //	//	FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//舵机向左
    //	//	delayms(BZDelayMs);//延时
    //	//}
    //	//else {//左避障
    //	//	FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//舵机向左
    //	//	delayms(BZDelayMs);//延时
    //	//	FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);//舵机向右
    //	//	delayms(BZDelayMs);//延时
    //	//}
    //	//biZhangFlag = 2;
    //	//EnableInterrupts;//开中断


}










void biZhangDuo() {                     //避障
    //#if BIZHANGFANGXIANG
    //	if (julicnt < BZDelay - BZOUTDELAY) {
    //		endianci = 1;
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);	//舵机向左
    //	}
    //	else if (julicnt < 2 * (BZDelay - BZOUTDELAY)) {
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//舵机向右
    //	}
    //	else if (julicnt < 2 * (BZDelay - BZOUTDELAY)) {
    //		setspeed = 20;
    //		FTM_PWM_Duty(FTM2, CH0, DuoCenter);
    //	}
    //	else if (julicnt < 2 * (BZDelay + BZOUTDELAY) + 100) {
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//舵机向右
    //		setspeed = 50;
    //	}
    //	else if (julicnt < 2 * (BZDelay + BZOUTDELAY) + 240)
    //	{
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);//舵机向左
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
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);	//舵机向右
    //	}
    //	else if (julicnt < 2 * (BZDelay - BZOUTDELAY)) {
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);//舵机向左
    //	}
    //	else if (julicnt < 2 * (BZDelay - BZOUTDELAY)) {
    //		setspeed = 20;
    //		FTM_PWM_Duty(FTM2, CH0, DuoCenter);
    //	}
    //	else if (julicnt < 2 * (BZDelay + BZOUTDELAY) + 100) {
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORRIGHT);//舵机向左
    //		setspeed = 50;
    //	}
    //	else if (julicnt < 2 * (BZDelay + BZOUTDELAY) + 240)
    //	{
    //		FTM_PWM_Duty(FTM2, CH0, BZMOTORLEFT);//舵机向右
    //		Dir_last = 1;
    //	}
    //	else {
    //		biZhangFlag = 2;
    //		//		endianci = 0;
    //	}
    //#endif
}




void OMuDuo(void) {                 //欧姆环
    //#if ZuoOMU
    //	if(omucnt>0)
    //	FTM_PWM_Duty(FTM2, CH0, servMotorRight);//左
    //
    //
    //#else
    //	if (omucnt > 0) {
    //		FTM_PWM_Duty(FTM2, CH0, servMotorLeft);//右
    //	}
    //	else if (omucnt > OMUINDELAY) {
    //		endianci = ENOUMUDIANCI;
    //		omuFlag = 2;
    //		omucnt = 0;
    //	}
    //
    //#endif
}
void OMuOutDuo() {                     //欧姆出环
    //#if ZuoOMU
    //
    //
    //
    //#else
    //	if (omucnt > 0) {
    //#if OMUOUTDUOFLAG
    //		FTM_PWM_Duty(FTM2, CH0, DuoCenter);//中
    //#else
    //		FTM_PWM_Duty(FTM2, CH0, servMotorCenture);//中
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