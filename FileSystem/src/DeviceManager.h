#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "BlockDevice.h"

/* 
 * 设备管理器（Device Manager）定义
 */
class DeviceManager
{
public:
	static const int MAX_DEVICE_NUM = 10;		/* 系统允许最大块设备数量 */
	
	static const int NODEV = -1;				/* NODEV设备号 */
	static const short ROOTDEV = (0 << 8) | 0;	/* 磁盘的主、从设备号都为0 */

	DeviceManager();
	~DeviceManager();
	
	int GetNBlkDev();							/* 获取系统中实际块设备数量nblkdev */
	BlockDevice& GetBlockDevice(short major);	/* 根据主设备号major获取相应块设备对象实例 */
	DevTab &GetDevTabl(short dev);				/* 根据主设备号获取设备控制表对应项 */

private:
	int nblkdev;							/* 系统中块设备的数量 */
	BlockDevice *bdevsw[MAX_DEVICE_NUM];	/* 指向块设备基类的指针数组，相当于Unix V6中块设备开关表 */
	DevTab *dev_tab[MAX_DEVICE_NUM]; 		/* 设备控制表 */

	void Initialize();							/* 初始化块设备基类指针数组。相当于是对块设备开关表bdevsw的初始化。*/
};

#endif