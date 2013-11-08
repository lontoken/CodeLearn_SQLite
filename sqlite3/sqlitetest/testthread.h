#ifndef _TEST_THREAD_H
#define _TEST_THREAD_H

#include <Windows.h>
#include <process.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "sqlite3.h"
#include "sqlitehelper.h"
#include "Timer.h"

using namespace std;

sqlite3 *g_db = NULL;
TimerNano curTimer;

void InitDataBase_Thread()
{
    int iRes = sqlite3_initialize();
    if(SQLITE_OK != iRes){
        cout << "[InitHbDataBase] sqlite3 initialize faild." << std::endl;
        exit(1);
    }
    
    iRes = sqlite3_open("db_thread.db", &g_db);
    if(iRes){
        cout << "can't open db!" << sqlite3_errmsg(g_db) << std::endl;
        sqlite3_close(g_db);
        exit(1);
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS book(id INT, name TEXT)";

    iRes = ExecSqlNoReturn(g_db, sql);
    if(iRes){
        cout << "[InitTable]CREATE TABLE repent faild." << std::endl;
        exit(1);
    }
}

void UninitDataBase_Thread()
{
    sqlite3_close(g_db);
}

int InsertBook(sqlite3 *db, int id, char* name)
{
    //TransactionControler tranConl(db);

    int iRes = 0;
    const char* sql = "INSERT INTO book(id, name) VALUES(?,?)";

    int lenSql = strlen(sql);
    sqlite3_stmt* stmt = NULL;
    iRes = sqlite3_prepare_v2(db, sql, lenSql, &stmt, NULL);
    if (SQLITE_OK != iRes) {
        if (stmt)sqlite3_finalize(stmt);
        cout << "[InsertReportedEntrust]prepare stmt faild. msg=" << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, name, strlen(name), SQLITE_TRANSIENT);

    iRes = sqlite3_step(stmt);
    if(SQLITE_DONE != iRes){
        sqlite3_finalize(stmt);
        cout << "[InsertReportedEntrust]do step faild. msg=" << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    sqlite3_finalize(stmt);

    //iRes = tranConl.Commit();
    //if(iRes){
    //    cout << "[InsertReportedEntrust]commit transaction faild. msg=" << sqlite3_errmsg(db) << std::endl;
    //    return 1;
    //}

    return 0;
}


int InsertBookList(sqlite3 *db, int* idList, char* name, const int iCount)
{
    TransactionControler tranConl(db);

    int iRes = 0;
    const char* sql = "INSERT INTO book(id, name) VALUES(?,?)";

    int lenSql = strlen(sql);
    sqlite3_stmt* stmt = NULL;
    iRes = sqlite3_prepare_v2(db, sql, lenSql, &stmt, NULL);
    if (SQLITE_OK != iRes) {
        if (stmt)sqlite3_finalize(stmt);
        cout << "[InsertReportedEntrust]prepare stmt faild. msg=" << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    int* pItem = NULL;
    for (int i = 0; i < iCount; ++i) {
        pItem = idList + i;
        sqlite3_bind_int(stmt, 1, *pItem);
        sqlite3_bind_text(stmt, 2, name, strlen(name), SQLITE_TRANSIENT);

        iRes = sqlite3_step(stmt);
        if(SQLITE_DONE != iRes){
            sqlite3_finalize(stmt);

            cout << "[InsertReportedEntrust]do step faild. msg=" << sqlite3_errmsg(db) << std::endl;
            return 1;
        }
        //重新初始化该sqlite3_stmt对象绑定的变量。
        sqlite3_reset(stmt);
    }
    sqlite3_finalize(stmt);

    iRes = tranConl.Commit();
    if(iRes){
        cout << "[InsertReportedEntrust]commit transaction faild. msg=" << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    return 0;
}

double flag_thread_1, flag_thread_2;
unsigned int __stdcall DoInsertBooksAction(void *pPM)
{
    int* param = (int*)pPM;
    int iCount = *param;
    int* pId = param + 1;

    sqlite3 *db = NULL;
    sqlite3_open("db_thread.db", &db);

    InsertBookList(db, pId, "name", iCount);

    return 0;
}

const int THREAD_NUM = 10;

int TestThread()
{
    cout << "[TestThread]begin." << std::endl;
    InitDataBase_Thread();
    memset(&curTimer, 0, sizeof(curTimer));

    double flag_1, flag_2;

    const int iCount = 200;
    cout.precision(12);

    flag_1 = GetCurrentTimerFlag();
    {
        TransactionControler tranConl(g_db);
        for(int idx = 0; idx < iCount; ++idx){
            InsertBook(g_db, idx, "name");
        }

        int iRes = tranConl.Commit();
        if(iRes){
            cout << "[InsertReportedEntrust]commit transaction faild. msg=" << sqlite3_errmsg(g_db) << std::endl;
            return 1;
        }
    }
    flag_2 = GetCurrentTimerFlag();
    cout << "flag_1=" << flag_1 << ",flag_2=" << flag_2 << ",diff=" << (flag_2 - flag_1) << ", pre=" << (flag_2 - flag_1)/iCount << std::endl;


    int idList[iCount] = {};
    for(int idx = 0; idx < iCount; ++idx){
        idList[idx] = idx;
    }
    flag_1 = GetCurrentTimerFlag();
    for(int i = 0; i < THREAD_NUM; i++)
        InsertBookList(g_db, idList, "name", iCount);
    flag_2 = GetCurrentTimerFlag();
    cout << "flag_1=" << flag_1 << ",flag_2=" << flag_2 << ",diff=" << (flag_2 - flag_1) 
        << ", pre=" << (flag_2 - flag_1)/(iCount * THREAD_NUM) << std::endl;

    int doCount = 400;
    
    HANDLE handle[THREAD_NUM];

    flag_thread_1 = GetCurrentTimerFlag();
    for(int i = 0; i < THREAD_NUM; i++)
        handle[i] = (HANDLE)_beginthreadex(NULL, 0, DoInsertBooksAction, &doCount, 0, NULL);
    WaitForMultipleObjects(THREAD_NUM, handle, TRUE, INFINITE);
    flag_thread_2 = GetCurrentTimerFlag();

    cout << "flag_thread_1=" << flag_thread_1 << ",flag_thread_2=" << flag_thread_2 << ",diff=" << (flag_thread_2 - flag_thread_1) 
        << ", pre=" << (flag_thread_2 - flag_thread_1)/(doCount * THREAD_NUM) << std::endl;

    UninitDataBase_Thread();


    cout << "[TestThread]end." << std::endl;
    return 0;
}


#endif