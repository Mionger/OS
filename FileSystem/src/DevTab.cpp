#include "DevTab.h"

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