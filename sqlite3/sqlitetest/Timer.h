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

//��ȡ��ǰʱ�䣬��ȷ�����룬�����������ʱ0.4΢��
void GetTimeOnNano(TimerNano *timer);

//��ӡʱ��
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