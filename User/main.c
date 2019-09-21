#include "common.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_dcmi.h"
#include "camera.h"
#include "lcd.h"
#include "debug.h"

void Init_Clock()
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}
void Init_IO()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  //PC4-5 for sccb Out PP Pu
  GPIOC->MODER |= 0b0101 << 4 * 2;
  GPIOC->OTYPER &= ~(0b11 << 4);
  GPIOC->OSPEEDR |= 0b1111 << 4 * 2;
  GPIOC->PUPDR |= 0b0101 << 4 * 2;
  //PE0 for cam rst  PE1 for cam pwdn
  GPIOC->MODER |= 0b0101;
  GPIOC->OTYPER &= ~(0b11);
  GPIOC->OSPEEDR |= 0b1111;
  GPIOC->PUPDR |= 0b0101;
  //PF9-10 for LED
  GPIOF->MODER |= 0b0101 << 9 * 2;
  GPIOF->OTYPER &= ~(0b11 << 9);
  GPIOF->OSPEEDR |= 0b1111 << 9 * 2;
  GPIOF->PUPDR |= 0b0101 << 4 * 2;
  GPIOF->BSRRL = 3 << 9;
  //PA9 for usart1 Tx
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  //PA8 for cam xclk
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
  //PA4 for DCMI HSync 
  //PA6 for DCMI PClk
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);
  //PB6 for DCMI D5
  //PB7 for DCMI VSync
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);
  //PC6-9,11 for DCMI D0-4
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_11;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_DCMI);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_DCMI);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_DCMI);
  //PE3 for key1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIOE->BSRRL = 1 << 3;
}

int main()
{
  u8 tmp = 0;
  COMMON_Init(168);
  Init_Clock();
  Init_IO();
  DEBUG_Init();
  LCD_Init();
  printf("start\n");
  tmp = CAM_Init();
  if (!tmp)
  {
    printf("Camera init ok.\n");
  }
  LCD_SetWindow(0, 320, 0, 240);
  LCD_Cmd(0x2C);
  DMA_Cmd(DMA2_Stream1, ENABLE);
  DCMI_CaptureCmd(ENABLE);
  while (1)
  {
    // if (!(GPIOE->IDR & (1 << 3)))
    // {
    //   while (!(GPIOE->IDR & (1 << 3)));
    //   DCMI_CaptureCmd(ENABLE);
    // }
  }
}
