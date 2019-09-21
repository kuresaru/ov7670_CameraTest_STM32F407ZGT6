#include "common.h"

#ifndef __SCCB_H
#define __SCCB_H

#define SCCB_SCL PCout(5)
#define SCCB_SDA PCout(4)
#define SCCB_SDA_IN()                    \
    {                                    \
        GPIOC->MODER &= ~(3 << (4 * 2)); \
        GPIOC->MODER |= 0 << 4 * 2;      \
    }
#define SCCB_SDA_OUT()                   \
    {                                    \
        GPIOC->MODER &= ~(3 << (4 * 2)); \
        GPIOC->MODER |= 1 << 4 * 2;      \
    }
#define SCCB_READ_SDA PCin(4)
#define SCCB_ADDRESS 0X42

void SCCB_Init();
u8 SCCB_RD_Reg(u8 reg);
u8 SCCB_WR_Reg(u8 reg, u8 data);

#endif