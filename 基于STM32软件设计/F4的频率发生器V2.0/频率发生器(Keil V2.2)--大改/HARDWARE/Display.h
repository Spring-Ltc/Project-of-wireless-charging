#ifndef __DISPLAY_H__
#define __DISPLAY_H__


#include "sys.h"


extern	u8 lcd_id[12];	//´æ·ÅLCD ID×Ö·û´®

extern void MenuShow(u8 colur,u8 mode);
extern void DateShow(u32 freq,u32 duty,u32 dead,u16 line);
#endif






