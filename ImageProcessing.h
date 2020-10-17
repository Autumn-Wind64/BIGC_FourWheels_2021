
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
        if (i >= 35 && i < 40)  //LMR是读取图像的二维数组，LMR[0][i]代表左列，LMR[1][i]代表中列，LMR[2][i]代表右列
        {                            //其中i代表的是行数，现有值35-40行。如果将这个范围调整，进入if的次数也就越多，会影响到Middle_Err_Sum的值，间接影响到row_mid的值（小车依靠这个值判断何时转向，以及转向幅度）
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
