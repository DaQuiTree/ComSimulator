#ifndef YIELDDATA_H
#define YIELDDATA_H

class YieldData{
    unsigned short milk;
    unsigned short cond;
    unsigned short motion;
    unsigned short code;
    bool bClear;
public:
    YieldData();
    ~YieldData();
    void GenericData();
    void GetMilk(unsigned short& mv, unsigned short& cv, bool& bc);//获取奶量
    void GetData(unsigned short& cd, unsigned short& mo);//获取活动量，牛号
};
#endif // YIELDDATA_H
