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
    scanf("%d",&a);//û�д��д��룬���д��ڳ��ֺ����ɽ��ʧ��
    return 0;
}