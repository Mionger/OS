#include "BlockDevice.h"

BlockDevice::BlockDevice(char* device_name)
{
    this->d_name = device_name;
    this->fp = fopen(this->d_name, "rb+");
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

/* 获取块设备名称 */
string BlockDevice::GetName()
{
    return this->d_name;
}

/* 设置设备对应的文件控制表项 */
void BlockDevice::SetDevTab(DevTab *d_tab)
{
    this->d_tab = d_tab;
    return;
}

/* 初始化文件控制表项 */
void BlockDevice::InitDevTab()
{
    if (NULL != this->d_tab)
    {
        this->d_tab->b_forw = (Buf *)this->d_tab;
        this->d_tab->b_back = (Buf *)this->d_tab;
    }
    return;
}

/* 向块设备写入 */
void BlockDevice::write(char *buf, int buf_size, int offset)
{
    fseek(this->fp, long(offset), SEEK_SET);
    fwrite((void *)buf, buf_size, sizeof(char), this->fp);
}

/* 从块设备读取 */
void BlockDevice::read(char *buf, int buf_size, int offset)
{
    fseek(this->fp, long(offset), SEEK_SET);
    fread((void *)buf, buf_size, sizeof(char), this->fp);
}