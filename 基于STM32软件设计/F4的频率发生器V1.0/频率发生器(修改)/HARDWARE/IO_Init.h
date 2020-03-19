#ifndef __IO_INIT_H__
#define __IO_INIT_H__


#include <sys.h>



void IO_INIT(void);

//设置IO口与中断映射关系，并进行初始化
void SetCorection(void);

//配置中断分组
void ExtiGpoup(void);






#endif







