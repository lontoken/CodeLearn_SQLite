#include <stdlib.h>
#include <cstdio>
#include <string>
#include "sqlite3.h"
#include "testbasecase.h"
#include "testthread.h"

int main()
{
    //TestBaseCase();
    TestThread();

    int a;
    scanf("%d",&a);//没有此行代码，运行窗口出现后会马山消失。
    return 0;
}