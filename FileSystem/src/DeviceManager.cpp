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

int DeviceManager::GetNBlkDev()
{
    return this->nblkdev;
}

BlockDevice &DeviceManager::GetBlockDevice(short major)
{
    // if (major >= 0 && major < this->nblkdev)
    return *(this->bdevsw[major]);
}