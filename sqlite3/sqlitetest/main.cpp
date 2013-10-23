#include <stdlib.h>
#include <cstdio>
#include "sqlite3.h"

int main()
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
    for(i=0;i<tr;++i) //输出查询结果
    {
        printf("results[%d]= %s/n",i,results[i]); //此处可以自己定义输出格式，
    }
    sqlite3_free_table(results); //free 
    sqlite3_close(db);
    int a;
    scanf("%d",&a);//没有此行代码，运行窗口出现后会马山消失。
    return 0;
}