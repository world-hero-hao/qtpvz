#include "wallnut.h"

#include "wallnut.h"

wallnut::wallnut() : plant()  // 显式调用父类构造函数
{
    hp = 3200;
    setMovie(":/new/prefix1/WallNut.gif");
}


void wallnut::advance(int phase)
{
    if(!phase)
        return;

    if(hp<=0)
    {
        delete this;
        return;
    }
    else if(hp<1200)
    {
        setMovie(":/new/prefix1/WallNut2.gif");
        return;
    }
    else if(hp<2200)
    {
        setMovie(":/new/prefix1/WallNut1.gif");
        return;
    }
}
