#include <iostream>
#include "BlockDevice.h"
#include "BufferManager.h"
#include "SuperBlockManager.h"
#include "INodeManager.h"
#include "UserInterface.h"

BlockDevice             os_BlockDevice;
BufferManager           os_BufferManager;
SuperBlock              os_SuperBlock;
SuperBlockManager       os_SuperBlockManager;
INodeManager            os_INodeManager;
UserInterface           ui;

//TODO:删除文件夹失败
//dirdelete


int main()
{

    ui.format();

    ui.GetCmd("root");
    return 0;
}