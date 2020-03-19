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

//TODO:进入..和.出现段错误

int main()
{

    ui.format();

    // ui.mkdir("home");
    // cout << "创建路径home成功" << endl;

    ui.GetCmd("root");
    return 0;
}