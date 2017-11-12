#include <QtGlobal>
#include <QDebug>
#include "yielddata.h"

YieldData::YieldData()
{
    milk = 0;
    cond = 0;
    bClear = false;
}

YieldData::~YieldData()
{

}

void YieldData::GenericData()
{
    milk = qrand();
    cond = qrand();
    motion = qrand();
    code = qrand();
    bClear = false;
}

void YieldData::GetData(unsigned short &cd, unsigned short &mo)
{
    cd = code;
    mo = motion;
}

void YieldData::GetMilk(unsigned short &mv, unsigned short &cv, bool &bc)
{
    mv = milk;
    cv = cond & 0x00FF;
    bc = bClear;
}
