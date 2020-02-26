#include "DiskINode.h"

DiskINode::DiskINode()
{
    this->d_mode = 0;
    this->d_nlink = 0;

    this->d_uid = 0;
    this->d_gid = 0;

    this->d_size = 0;
    for (int i = 0; i < 10; i++)
    {
        this->d_addr[i] = 0;
    }

    this->d_atime = 0;
    this->d_mtime = 0;
}

DiskINode::~DiskINode()
{

}