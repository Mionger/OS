#include <iostream>
#include "BlockDevice.h"
#include "BufferManager.h"
#include "SuperBlockManager.h"
#include "INodeManager.h"
#include "UserInterface.h"
#include "tool.h"

#define TEXT_DIR "text.txt"
#define REPORT_DIR "report.pdf"
#define PHOTO_DIR "photo.png"

BlockDevice             os_BlockDevice;
BufferManager           os_BufferManager;
SuperBlock              os_SuperBlock;
SuperBlockManager       os_SuperBlockManager;
INodeManager            os_INodeManager;
UserInterface           ui;

void initCreate(UserInterface &ui, string dir_name);
void initFileSys(UserInterface &ui);
void rwTest(UserInterface &ui);

int main()
{
    os_SuperBlockManager.FormatDisk();
    ui.format();

    initFileSys(ui);
    rwTest(ui);

    ui.GetCmd("root");
    return 0;
}

void initFileSys(UserInterface &ui)
{
    ui.mkdir("bin");
    ui.mkdir("etc");
    ui.mkdir("home");
    ui.mkdir("dev");

    ui.cd("home");

    initCreate(ui, "texts");
    cout << "已读入文本文件" << endl;
    initCreate(ui, "photos");
    cout << "已读入图片" << endl;
    initCreate(ui, "reports");   
    cout << "已读入报告" << endl;

    ui.cd("..");
}

void rwTest(UserInterface &ui)
{
    char wbuf[800];
    char rbuf[20];

    // 创建文件
    ui.mkdir("test");
    ui.cd("test");
    ui.touch("Jerry");
    ui.fopen("Jerry");
    cout << "已创建/test/Jerry" << endl;

    //写入测试
    for(int i = 0; i < 800; i++) 
    {
        wbuf[i] = '0' + i % 10;
    }
    ui.fwrite(wbuf, 800);
    cout << "已向/test/Jerry写入800字节" << endl;

    cout << "当前文件指针位置为 : " << ui.curseek() << endl;

    // 移动读写指针
    ui.fseek(500);

    cout << "当前文件指针位置为 : " << ui.curseek() << endl;

    // 读取测试
    ui.fread(rbuf, 20);
    cout << "已从/test/Jerry读取20字节" << endl;
    for (int i = 0; i < 20; i++)
    {
        if(10==i)
            cout << endl;
        cout << rbuf[i];
    }
    cout << endl;
    ui.fclose();

    ui.cd("..");
}

void initCreate(UserInterface &ui, string dir_name)
{
    ifstream fin;
    int size;
    char buf[512];

    string ofile_name;
    string file_name;
    if("texts"==dir_name)
    {
        ofile_name = TEXT_DIR;
        file_name = TEXT_DIR;
    }
    else if("reports"==dir_name)
    {
        ofile_name = TEXT_DIR;
        file_name = REPORT_DIR;
    }
    else
    {
        ofile_name = TEXT_DIR;
        file_name = PHOTO_DIR;
    }

    ui.mkdir(dir_name);
    ui.cd(dir_name);
    fin.open(ofile_name, ios::in | ios::binary);
    if(fin.is_open())
    {
        // 获取文件长度
        fin.seekg(0, ios::end);
        size = fin.tellg();
        fin.seekg(0, ios::beg);

        ui.touch(file_name);
        ui.fopen(file_name);

        const int blk_cnt = size / 512;
        const int rest = size % 512;
        for(int i = 0; i < blk_cnt; i++) 
        {
            fin.read(buf, 512);
            ui.fwrite(buf, 512);
        }
        if(rest) 
        {
            fin.read(buf, rest);
            ui.fwrite(buf, rest);
        }

        ui.fclose();
        fin.close();
    }
    else
    {
        if("texts"==dir_name)
        {
            cout << "ERROR CODE 009" << endl;
        }
        else if("reports"==dir_name)
        {
            cout << "ERROR CODE 010" << endl;
        }
        else
        {
            cout << "ERROR CODE 011" << endl;
        }
    }

    ui.cd("..");
}