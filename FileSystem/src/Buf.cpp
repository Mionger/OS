#include <iostream>
#include "Buf.h"

Buf::Buf()
{
    this->b_flags = 0;

    this->b_forw = NULL;
    this->b_back = NULL;
    this->av_forw = NULL;
    this->av_back = NULL;

    this->b_dev = -1;
    this->b_wcount = 0;
    this->b_blkno = 0;
}

Buf::~Buf()
{

}