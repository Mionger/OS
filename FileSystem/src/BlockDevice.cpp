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

DevTab::DevTab()
{
    this->d_active = 0;
    this->d_errcnt = 0;
    
    this->b_forw = NULL;
    this->b_back = NULL;
    this->d_actf = NULL;
    this->d_actl = NULL;
}

DevTab::~DevTab()
{

}
