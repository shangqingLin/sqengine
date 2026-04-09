#ifndef _COMMON_DATE_H_
#define _COMMON_DATE_H_

//返回当前时间的毫秒时间轴
//类似与Js中的Date.now();
long getDateNow();

class Date
{
 private:
    long start;
 public:
    Date();
    long reset();
    long get();
};

#endif