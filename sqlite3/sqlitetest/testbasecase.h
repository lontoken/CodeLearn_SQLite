#ifndef _TEST_BASECASE_H
#define _TEST_BASECASE_H

#include <stdlib.h>
#include <cstdio>
#include <string>
#include "sqlite3.h"

int TestBaseCase()
{
    int rc;
    int i, nrows, ncols, tr;
    char *errmsg = NULL;
    char **results;
    
    sqlite3 *db = NULL;
    rc = sqlite3_open("demodb", &db);
    if (rc)
    {
        fprintf(stderr, "can't open db!\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    else
    {
        printf("db open successfully!\n");
    }
    sqlite3_get_table(db,"select * from clients;",&results,&nrows,&ncols,&errmsg);
    printf("DB has %d rows and %d cols\n\n",nrows,ncols);
    tr=(nrows+1)*ncols;
    for(i=0;i<tr;++i) //�����ѯ���
    {
        printf("results[%d]= %s/n",i,results[i]); //�˴������Լ����������ʽ��
    }
    sqlite3_free_table(results); //free 
    sqlite3_close(db);

    printf("====");
    //int iRes = sqlite3_open_v2("hb.db", &db, SQLITE_OPEN_READWRITE, NULL);
    //if(iRes == SQLITE_OK){
    //    printf("hb.db  exit\n");
    //    sqlite3_close(db);

    //    int iRes = sqlite3_open("hb.db", &db);
    //    if(iRes != SQLITE_OK){
    //        printf("hb.db create faild.");
    //        sqlite3_close(db);
    //    }
    //}else{
    //    printf("hb.db not exit. %d, %s\n", iRes, sqlite3_errmsg(db));

        int iRes = sqlite3_open("hb.db", &db);
        if(iRes != SQLITE_OK){
            printf("hb.db create faild.");
            sqlite3_close(db);
        }else{
            const char* createTableSQL = 
                "CREATE TABLE IF NOT EXISTS  ReportedEntrust (report_id INT, exreport_id INT, entrust_id INT, float_col REAL, string_col TEXT)";
            sqlite3_stmt* stmt = NULL;
            int len = strlen(createTableSQL);
            //2. ׼���������ݱ��������ʧ�ܣ���Ҫ��sqlite3_finalize�ͷ�sqlite3_stmt�����Է�ֹ�ڴ�й¶��
            if (sqlite3_prepare_v2(db,createTableSQL, len, &stmt, NULL) != SQLITE_OK) {
                if (stmt)
                    sqlite3_finalize(stmt);
                sqlite3_close(db);
                return 1;
            }
            //3. ͨ��sqlite3_step����ִ�д��������䡣����DDL��DML�����ԣ�sqlite3_stepִ����ȷ�ķ���ֵ
            //ֻ��SQLITE_DONE������SELECT��ѯ���ԣ���������ݷ���SQLITE_ROW������������ĩβʱ�򷵻�
            //SQLITE_DONE��
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return 1;
            }
            //4. �ͷŴ��������������Դ��
            sqlite3_finalize(stmt);
            printf("Succeed to create test table now.\n");
        }
    //}

    //5. ��ʽ�Ŀ���һ�����
    sqlite3_stmt* stmt2 = NULL;
    const char* beginSQL = "BEGIN TRANSACTION";
    if (sqlite3_prepare_v2(db,beginSQL,strlen(beginSQL),&stmt2,NULL) != SQLITE_OK) {
        if (stmt2)
            sqlite3_finalize(stmt2);

        printf("sqlite3_prepare_v2. %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }
    if (sqlite3_step(stmt2) != SQLITE_DONE) {
        sqlite3_finalize(stmt2);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_finalize(stmt2);

    //6. �������ڰ󶨱����Ĳ������ݡ�
    const char* insertSQL = "INSERT INTO ReportedEntrust VALUES(?,?,?,?,?)";
    sqlite3_stmt* stmt3 = NULL;
    if (sqlite3_prepare_v2(db,insertSQL,strlen(insertSQL),&stmt3,NULL) != SQLITE_OK) {
        if (stmt3)
            sqlite3_finalize(stmt3);

        printf("sqlite3_prepare_v2. %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);

        int a;
        scanf("%d",&a);

        return 1;
    }
    int insertCount = 10;
    const char* strData = "This is a test.";
    //7. �������е�SQL��䣬�����İ󶨲�ͬ�ı�������
    for (int i = 0; i < insertCount; ++i) {
        //�ڰ�ʱ��������ı�������ֵ��1��
        sqlite3_bind_int(stmt3,1,i);
        sqlite3_bind_int(stmt3,2,i);
        sqlite3_bind_int(stmt3,3,i);
        sqlite3_bind_double(stmt3,4,i * 1.0);
        sqlite3_bind_text(stmt3,5,strData,strlen(strData),SQLITE_TRANSIENT);

        if (sqlite3_step(stmt3) != SQLITE_DONE) {
            sqlite3_finalize(stmt3);
            sqlite3_close(db);
            return 1;
        }
        //���³�ʼ����sqlite3_stmt����󶨵ı�����
        sqlite3_reset(stmt3);
        printf("Insert Succeed.\n");
    }
    sqlite3_finalize(stmt3);

    //8. �ύ֮ǰ�����
    const char* commitSQL = "COMMIT";
    sqlite3_stmt* stmt4 = NULL;
    if (sqlite3_prepare_v2(db,commitSQL,strlen(commitSQL),&stmt4,NULL) != SQLITE_OK) {
        if (stmt4)
            sqlite3_finalize(stmt4);
        sqlite3_close(db);
        return 1;
    }
    if (sqlite3_step(stmt4) != SQLITE_DONE) {
        sqlite3_finalize(stmt4);
        sqlite3_close(db);
        return 1;
    }
    sqlite3_finalize(stmt4);

    sqlite3_get_table(db,"select * from ReportedEntrust;",&results,&nrows,&ncols,&errmsg);
    printf("DB ReportedEntrust has %d rows and %d cols\n\n",nrows,ncols);
    tr=(nrows+1)*ncols;
    for(i=0;i<tr;++i) //�����ѯ���
    {
        printf("results[%d]= %s\n",i,results[i]); //�˴������Լ����������ʽ��
    }
    sqlite3_free_table(results); //free 
    sqlite3_close(db);

    return 0;
}

#endif
