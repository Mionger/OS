#include "DeviceManager.h"

DeviceManager::DeviceManager()
{
    Initialize();
}

DeviceManager::~DeviceManager()
{
    
}

void DeviceManager::Initialize()
{
    /* 挂载硬盘文件 */
    this->dev_tab[0] = new DevTab;
    this->bdevsw[0] = new BlockDevice("a.img");
    this->bdevsw[0]->SetDevTab(this->dev_tab[0]);
    this->bdevsw[0]->InitDevTab();
    this->nblkdev = 1;
}

/* 获取系统中实际块设备数量nblkdev */
int DeviceManager::GetNBlkDev()
{
    return this->nblkdev;
}

/* 根据主设备号major获取相应块设备对象实例 */
BlockDevice &DeviceManager::GetBlockDevice(short major)
{
    // if (major >= 0 && major < this->nblkdev)
    return *(this->bdevsw[major]);
}

/* 根据主设备号获取设备控制表对应项 */
DevTab &DeviceManager::GetDevTabl(short mahor)
{
    return *(this->dev_tab[mahor]);
}