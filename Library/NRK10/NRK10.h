#ifndef _NRK10_H
#define _NRK10_H

#include "sys.h"

typedef struct 
{
    u8 wake_up[3];
    u8 intensity_up[3];
    u8 intensity_down[3];
    u8 time_up[3];
    u8 time_down[3];
    u8 mode_become[3];
    u8 start[3];
    u8 pause[3];
} _NRK10_type;

#define NRK10_OVER_TIME 10000

extern const _NRK10_type NRK10_command;
extern u8 NRK10_dataBuff[];
extern u16 NRK10_Overtime_Cnt;

void NRK10_ReceiveByte(u8 byte);
void NRK10_Listen(void(*press)(void), u8 i, const u8 *command);
void NRK10_Cancel(void);
void NRK10_Handler(void);

#endif
