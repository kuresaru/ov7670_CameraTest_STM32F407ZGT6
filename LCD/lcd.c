#include "lcd.h"
#include "common.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_fsmc.h"

#define LCD_WR_REG LCD_Cmd
#define LCD_WR_DATA LCD_Data

void LCD_Cmd(u16 cmd)
{
	*((u16 *)LCD_BASE) = cmd;
}
void LCD_Data(u16 data)
{
	*((u16 *)(LCD_BASE | (1 << 7))) = data;
}
void LCD_Data16(vu16 data)
{
#ifdef LCD_16BIT
	LCD_Data(data);
#else
	LCD_Data(data >> 8);
	LCD_Data(data);
#endif
}
void LCD_InitIO()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = 0b1100000000110011;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0b0000011110000000;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0b0001000000000000;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0b0001000000000000;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_FSMC);
#ifdef LCD_16BIT
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
#endif
	GPIOB->BSRRL = 1 << 15;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void LCD_Init()
{
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef readWriteTiming;
	FSMC_NORSRAMTimingInitTypeDef writeTiming;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
	LCD_InitIO();

	readWriteTiming.FSMC_AddressSetupTime = 0XF; //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns
	readWriteTiming.FSMC_AddressHoldTime = 0x00; //地址保持时间（ADDHLD）模式A未用到
	readWriteTiming.FSMC_DataSetupTime = 60;	 //数据保存时间为60个HCLK	=6*60=360ns
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	readWriteTiming.FSMC_CLKDivision = 0x00;
	readWriteTiming.FSMC_DataLatency = 0x00;
	readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A; //模式A

	//9341: 6*3=18ns
	writeTiming.FSMC_AddressSetupTime = 3;   //地址建立时间（ADDSET）为9个HCLK =54ns
	writeTiming.FSMC_AddressHoldTime = 0x00; //地址保持时间（A
	writeTiming.FSMC_DataSetupTime = 3;		 //数据保存时间为6ns*9个HCLK=54ns
	writeTiming.FSMC_BusTurnAroundDuration = 0x00;
	writeTiming.FSMC_CLKDivision = 0x00;
	writeTiming.FSMC_DataLatency = 0x00;
	writeTiming.FSMC_AccessMode = FSMC_AccessMode_A; //模式A

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;					   //  这里我们使用NE4 ，也就对应BTCR[6],[7]。
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;   // 不复用数据地址
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;			   // FSMC_MemoryType_SRAM;  //SRAM
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;	  //存储器数据宽度为16bit
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable; // FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable; //  存储器写使能
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;		 //写时序

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); //初始化FSMC配置
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE); // 使能BANK1


	LCD_Cmd(0xcf);
	LCD_Data(0x00);
	LCD_Data(0xC1);
	LCD_Data(0x30);

	LCD_Cmd(0xed);
	LCD_Data(0x67);
	LCD_Data(0x03);
	LCD_Data(0x12);
	LCD_Data(0x81);

	LCD_Cmd(0xcb);
	LCD_Data(0x39);
	LCD_Data(0x2c);
	LCD_Data(0x00);
	LCD_Data(0x34);
	LCD_Data(0x02);

	LCD_Cmd(0xea);
	LCD_Data(0x00);
	LCD_Data(0x00);

	LCD_Cmd(0xe8);
	LCD_Data(0x85);
	LCD_Data(0x0a);
	LCD_Data(0x78);

	LCD_Cmd(0xF7);
	LCD_Data(0x20);

	LCD_Cmd(0xC0);  //Power control
	LCD_Data(0x26); //VRH[5:0]

	LCD_Cmd(0xC1);  //Power control
	LCD_Data(0x01); //SAP[2:0];BT[3:0]

	LCD_Cmd(0xC5); //VCM control
	LCD_Data(0x2b);
	LCD_Data(0x2F);

	LCD_Cmd(0xc7);
	LCD_Data(0xc7);

	LCD_Cmd(0x3A);
	LCD_Data(0x55);

	LCD_Cmd(0x36); // Memory Access Control
				   //	LCD_Data(0x08);
#ifdef USE_HORIZONTAL
	LCD_Data(0x20);
#else
	LCD_Data(0x80);
#endif

	LCD_Cmd(0xB1); // Frame Rate Control
	LCD_Data(0x00);
	LCD_Data(0x18);

	LCD_Cmd(0xB6); // Display Function Control
	LCD_Data(0x0a);
	LCD_Data(0xE2);

	LCD_Cmd(0xF2); // 3Gamma Function Disable
	LCD_Data(0x00);
	LCD_Cmd(0x26); //Gamma curve selected
	LCD_Data(0x01);
	LCD_Cmd(0xE0); //Set Gamma
	LCD_Data(0x0f);
	LCD_Data(0x1d);
	LCD_Data(0x1a);
	LCD_Data(0x09);
	LCD_Data(0x0f);
	LCD_Data(0x09);
	LCD_Data(0x46);
	LCD_Data(0x88);
	LCD_Data(0x39);
	LCD_Data(0x05);
	LCD_Data(0x0f);
	LCD_Data(0x03);
	LCD_Data(0x07);
	LCD_Data(0x05);
	LCD_Data(0x00);

	LCD_Cmd(0XE1); //Set Gamma
	LCD_Data(0x00);
	LCD_Data(0x22);
	LCD_Data(0x25);
	LCD_Data(0x06);
	LCD_Data(0x10);
	LCD_Data(0x06);
	LCD_Data(0x39);
	LCD_Data(0x22);
	LCD_Data(0x4a);
	LCD_Data(0x0a);
	LCD_Data(0x10);
	LCD_Data(0x0c);
	LCD_Data(0x38);
	LCD_Data(0x3a);
	LCD_Data(0x0F);

	LCD_Cmd(0x11); //Exit Sleep
	Delay_ms(120);
	LCD_Cmd(0x29); //display on
	Delay_ms(30);
	
	LCD_Cls();
	Delay_ms(10);

	LCD_BL = 0;
}
void LCD_SetWindow(u16 x, u16 xsize, u16 y, u16 ysize)
{
	LCD_Cmd(0x2A);
	LCD_Data(x >> 8);
	LCD_Data(0x00FF & x);
	x = x + xsize - 1;
	LCD_Data(x >> 8);
	LCD_Data(0x00FF & x);

	LCD_Cmd(0x2B);
	LCD_Data(y >> 8);
	LCD_Data(0x00FF & y);
	y = y + ysize - 1;
	LCD_Data(y >> 8);
	LCD_Data(0x00FF & y);
}
void LCD_Start()
{
	LCD_Cmd(0x2C);
}
// ==========================================
// Fill rectangle area with given color
void LCD_Fill(s16 x, s16 y, s16 xsize, s16 ysize, u16 color)
{
	u32 tmp;
	LCD_SetWindow(x, xsize, y, ysize);
	LCD_Start();
	tmp = (u32)xsize * (u32)ysize;
	while (tmp--)
	{
		LCD_Data16(color);
	}
}
// Clear screen
void LCD_Cls(void)
{
	LCD_Fill(ScreenX0, ScreenY0, ScreenXsize, ScreenYsize, LCD_BGCOLOR);
}