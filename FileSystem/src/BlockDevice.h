#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

#include <fstream>
#include <iostream>
using namespace std;

class BlockDevice
{
private:
    static const char   *d_name;  /* 虚拟磁盘文件名 */
    FILE                *fp;      /* 文件指针 */

public:
    BlockDevice();
    ~BlockDevice();

    bool IfExist();                                     /* 磁盘文件是否存在 */
    void Bwrite(char *buf, int buf_size, int offset);   /* 向块设备写入 */
    void Bread(char *buf, int buf_size, int offset);    /* 从块设备读取 */
};

#endif