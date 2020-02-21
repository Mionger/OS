#include "BlockDevice.h"

BlockDevice::BlockDevice(string device_name)
{
    this->d_name = device_name;
    this->fs.open(d_name, ios::binary | ios::in | ios::out);
}

BlockDevice::~BlockDevice()
{
    this->fs.close();
}

/* 判断对应的磁盘文件是否存在 */
bool BlockDevice::IfExist()
{
    return this->fs.is_open();
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
    this->fs.seekp(offset, ios::beg);
    this->fs.write(buf, buf_size);
}

/* 从块设备读取 */
void BlockDevice::read(char *buf, int buf_size, int offset)
{
    this->fs.seekg(offset, ios::beg);
    this->fs.read(buf, buf_size);
}