#ifndef BLOCK_DEVICE_H
#define BLOCK_DEVICE_H

/*
 * 块设备（Block Device）定义
 */
class BlockDevice
{
private:
    /* 方法 */
public:
    /* 构造函数 */
    BlockDevice();
    /* 析构函数 */
    ~BlockDevice();

    void write(char *buf, int buf_size, int offset);    /* 向块设备写入 */
    void read(char *buf, int buf_size, int offset);     /* 从块设备读取 */
};

#endif