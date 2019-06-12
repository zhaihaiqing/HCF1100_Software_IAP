
#ifndef __IAP_H
#define __IAP_H



typedef  void (*iapfun)(void);				//定义一个函数类型的参数.

void iap_load_app(unsigned int appxaddr);			//执行flash里面的app程序
unsigned char Update_Firmware(void);
void iap_goto_app(unsigned int appxaddr);
uint8_t Wait_Commend(void);


#endif


