#ifndef _TIMER_H
#define _TIMER_H

#include <Windows.h>
#include <iostream>

typedef struct TTimerNano{
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;
    int mill;
    int micro;
    int nano;
}TimerNano;

void InitTimer();

//获取当前时间，精确到纳秒，本函数自身耗时0.4微秒
void GetTimeOnNano(TimerNano *timer);

//打印时间
void PrintTimeNano(TimerNano& obj);

double GetCurrentTimerFlag();

class TimerStarter{
public:
    TimerStarter(){
        InitTimer();
    };
private:
    static TimerStarter instance;
};

void TestTimer();

#endif