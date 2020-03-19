#include "BlockDevice.h"

const char* BlockDevice::d_name = "./a.img";

BlockDevice::BlockDevice()
{
    this->fp = fopen(this->d_name, "rb+");
    if(IfExist())
    {
        cout << "挂载磁盘"<<d_name<<"成功！" << endl;
    }
}

BlockDevice::~BlockDevice()
{
    fclose(this->fp);
}

/* 判断对应的磁盘文件是否存在 */
bool BlockDevice::IfExist()
{
    return (NULL != this->fp);
}

/* 向块设备写入 */
void BlockDevice::Bwrite(char *buf, int buf_size, int offset)
{
    fseek(this->fp, long(offset), SEEK_SET);
    fwrite((void *)buf, buf_size, sizeof(char), this->fp);
}

/* 从块设备读取 */
void BlockDevice::Bread(char *buf, int buf_size, int offset)
{
    fseek(this->fp, long(offset), SEEK_SET);
    fread((void *)buf, buf_size, sizeof(char), this->fp);
}