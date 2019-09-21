#include "debug.h"

#include <stdlib.h>
#include "stm32f4xx_usart.h"

void DEBUG_Init()
{
    USART_InitTypeDef USART_InitStructure;
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = 115200;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE); //使能串口1
}

void DEBUG_SendHex(u8 hex)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, hex);
}

int _write(int fd, char *ptr, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        DEBUG_SendHex(*(ptr + i));
    }
    return i;
}