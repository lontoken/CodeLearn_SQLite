#ifndef _SQLITE_HELPER_H
#define _SQLITE_HELPER_H

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "sqlite3.h"

using namespace std;

static int ExecSqlNoReturn(sqlite3 *db, const char* pSql)
{
    sqlite3_stmt* stmt = NULL;
    int len = strlen(pSql);

    //2. ׼���������ݱ��������ʧ�ܣ���Ҫ��sqlite3_finalize�ͷ�sqlite3_stmt�����Է�ֹ�ڴ�й¶
    int iRes = sqlite3_prepare_v2(db, pSql, len, &stmt, NULL);
    if (SQLITE_OK != iRes) {
        if(stmt) sqlite3_finalize(stmt);
        cout << "[ExecSqlNoReturn]faild. prepare. msg=" << sqlite3_errmsg(db) << ", sql=" << pSql << endl;
        return 1;
    }
    //3. ͨ��sqlite3_step����ִ�д��������䡣����DDL��DML�����ԣ�sqlite3_stepִ����ȷ�ķ���ֵ
    //ֻ��SQLITE_DONE������SELECT��ѯ���ԣ���������ݷ���SQLITE_ROW������������ĩβʱ�򷵻�
    //SQLITE_DONE��
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        cout << "[ExecSqlNoReturn]faild. step. msg=" << sqlite3_errmsg(db) << ", sql=" << pSql << endl;
        return 1;
    }
    //4. �ͷŴ��������������Դ��
    sqlite3_finalize(stmt);
    return 0;
}

static int ExecSqlGetOneIntValue(sqlite3 *db, const char* pSql, int *iOut)
{
    sqlite3_stmt* stmt = NULL;
    int sqlLen = strlen(pSql);
    int iRes = sqlite3_prepare_v2(db, pSql, sqlLen, &stmt, NULL);
    if (SQLITE_OK != iRes) {
        if (stmt)sqlite3_finalize(stmt);
        cout << "[ExecSqlGetOneIntValue]faild. step. msg=" << sqlite3_errmsg(db) << ", sql=" << pSql << endl;
        return 1;
    }

    iRes = sqlite3_step(stmt);

    int iVal = 0;
    if(SQLITE_ROW == iRes){
        iVal = sqlite3_column_int(stmt, 0);
    }else{
        sqlite3_finalize(stmt);
        cout << "[ExecSqlGetOneIntValue]faild. step. msg=" << sqlite3_errmsg(db) << ", sql=" << pSql << endl;
        return 1;
    }
    sqlite3_finalize(stmt);

    *iOut = iVal;
    return 0;
}


static int BeginTransaction(sqlite3 *db)
{
    const char* sql = "BEGIN TRANSACTION";
    int iRes = ExecSqlNoReturn(db, sql);
    if(iRes){
        cout << "[BeginTransaction]BEGIN TRANSACTION faild." << endl;
        return 1;
    }

    return 0;
}


static int CommitTransaction(sqlite3 *db)
{
    const char* sql = "COMMIT TRANSACTION";
    int iRes = ExecSqlNoReturn(db, sql);

    if(iRes){
        cout << "[CommitTransaction]COMMIT TRANSACTION faild." << endl;
        return 1;
    }

    return 0;
}


static int RollbackTransaction(sqlite3 *db)
{
    const char* sql = "ROLLBACK TRANSACTION";
    int iRes = ExecSqlNoReturn(db, sql);
    if(iRes){
        cout << "[RollbackTransaction]ROLLBACK TRANSACTION faild." << endl;
        return 1;
    }

    return 0;
}


class TransactionControler{
private:
    bool isCurActive;
public:
    TransactionControler(sqlite3 *db):_db(db){
        int iRes = BeginTransaction(_db);
        if(iRes){
            isCurActive = false;
        }else{
            isCurActive = true;
        }
    };
    virtual ~TransactionControler(){
        if(isCurActive)RollbackTransaction(_db);
    };

    int Commit(){
        int iRes = CommitTransaction(_db);
        if(0 == iRes)isCurActive = false;
        return iRes;
    }

private:
    sqlite3 *_db;
};

#endif