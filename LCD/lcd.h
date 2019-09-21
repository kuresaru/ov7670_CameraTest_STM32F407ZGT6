#ifndef __LCD_H
#define __LCD_H

#include "stm32f4xx.h"

// ===================================
// 显示设置
#define USE_HORIZONTAL		//横屏
#define LCD_16BIT			//16位总线
#define LCD_BGCOLOR clBlack //默认背景颜色

// ===================================
// 其它
#define ScreenX0 0
#define ScreenY0 0
#ifdef USE_HORIZONTAL
#define ScreenXsize 320
#define ScreenYsize 240
#else
#define ScreenXsize 240
#define ScreenYsize 320
#endif

#define LCD_BL PBout(15)

#define LCD_BASE ((u32)(0x6C000000))

//颜色定义
#define Rbits 0  // Red bits position
#define Gbits 5  // Green bits position
#define Bbits 11 // Blue bits position
#define RGB(R, G, B) (((R >> 3) << Rbits) | ((G >> 2) << Gbits) | ((B >> 3) << Bbits))
enum COLOR
{
	clBlack = RGB(0, 0, 0),
	clWhite = RGB(255, 255, 255),
	clRed = RGB(255, 0, 0),
	clGreen = RGB(0, 255, 0),
	clBlue = RGB(0, 0, 255),
	clYellow = RGB(255, 255, 0),
	clGainsboro = RGB(220, 220, 220),
	clNavy = RGB(0, 0, 128),
	clAqua = RGB(0, 255, 255),
	clHotpink = RGB(255, 105, 180),
	clOrange = RGB(255, 165, 0),
	clDeepskyblue = RGB(0, 191, 255),
	clDimgray = RGB(105, 105, 105),
	cllightsalmon = RGB(255, 160, 122),
	cllightcoral = RGB(240, 128, 128),
	clpaleturquoise = RGB(175, 238, 238),
	clturquoise = RGB(64, 224, 208),
	clViolet = RGB(238, 130, 238),
	clGray1 = RGB(90, 90, 90),
	clGray2 = RGB(220, 220, 220),
	clGray3 = RGB(240, 240, 240),
	clDarkgray = RGB(169, 169, 169),
	clSkyblue = RGB(135, 206, 235),
	clChocolate = RGB(210, 105, 30),
	clMediumseagreen = RGB(60, 179, 113),
	clPeachpuff = RGB(255, 218, 185),
	clSeagreen = RGB(46, 139, 87),
};

void LCD_InitIO();
void LCD_Init();
void LCD_Cmd(u16 cmd);
void LCD_Data(u16 data);
void LCD_Data16(vu16 data);
void LCD_SetWindow(u16 x, u16 xsize, u16 y, u16 ysize);
void LCD_Start();
void LCD_Cls(void);
void LCD_Fill(s16 x, s16 y, s16 xsize, s16 ysize, u16 color);

#endif