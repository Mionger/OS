#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <fstream>
#include <string>
using namespace std;

/*
 * 块设备（Block Device）类定义
 */
class BlockDevice
{
private:
    string  d_name;    /* 虚拟磁盘文件名 */
    fstream fs;         /* 文件操作流 */

public:
    BlockDevice(string device_name);
    ~BlockDevice();

    bool IfExist();                                     /* 判断对应的磁盘文件是否存在 */
    void write(char *buf, int buf_size, int offset);    /* 向块设备写入 */
    void read(char *buf, int buf_size, int offset);     /* 从块设备读取 */
};

#endif

#ifndef DEV_TAB_H
#define DEV_TAB_H

#include "Buf.h"

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