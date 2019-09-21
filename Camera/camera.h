#include "common.h"

#ifndef __CAMERA_H
#define __CAMERA_H

#define CAM_RST PEout(0)
#define CAM_PWDN PEout(1)

#define CAM_ID 0x7FA27673

u8 CAM_Init();

#endif
