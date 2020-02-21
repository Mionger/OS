#ifndef DEV_TAB_H
#define DEV_TAB_H

#include "Buf.h"
#include <iostream>
using namespace std;

/* 
 * 块设备表(DevTab)类定义
 */
class DevTab
{
public:
	DevTab();
	~DevTab();
	
public:
	int	d_active;
	int	d_errcnt;

    /* 缓存控制块队列勾连指针 */
	Buf* b_forw;        /* 设备队列 */
	Buf* b_back;        /* 设备队列 */
	Buf* d_actf;        /* i/o请求队列 */
	Buf* d_actl;        /* i/o请求队列 */
};

#endif