#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <fstream>
#include <string>
#include "DevTab.h"
using namespace std;

/*
 * 块设备（Block Device）类定义
 */
class BlockDevice
{
private:
    string  d_name;     /* 虚拟磁盘文件名 */
    fstream fs;         /* 文件操作流 */
    

public:
    BlockDevice(string device_name);
    ~BlockDevice();

    DevTab *d_tab;      /* 文件控制表项 */

    bool IfExist();                                     /* 判断对应的磁盘文件是否存在 */
    string GetName();                                   /* 获取块设备名称 */
    void SetDevTab(DevTab *d_tab);                      /* 设置设备对应的文件控制表项 */
    void InitDevTab();                                  /* 初始化文件控制表项 */
    void write(char *buf, int buf_size, int offset);    /* 向块设备写入 */
    void read(char *buf, int buf_size, int offset);     /* 从块设备读取 */
};

#endif

