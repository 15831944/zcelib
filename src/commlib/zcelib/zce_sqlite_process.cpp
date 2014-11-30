
#include "zce_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_sqlite_process.h"

//����SQLITE����Ͱ汾����
#if SQLITE_VERSION_NUMBER >= 3005000

//=========================================================================================

/******************************************************************************************
SQLite3_DB_Handler SQLite3DB Handler ���Ӵ���һ��SQLite3���ݿ��Handler
******************************************************************************************/
ZCE_SQLite_DB_Handler::ZCE_SQLite_DB_Handler():
    sqlite3_handler_(NULL)
{
}

ZCE_SQLite_DB_Handler::~ZCE_SQLite_DB_Handler()
{
    close_database();
}

//const char* db_file ,���ݿ������ļ�·��,�ӿ�Ҫ��UTF8���룬
//int == 0��ʾ�ɹ�������ʧ��
int ZCE_SQLite_DB_Handler::open_database(const char *db_file, bool create_db)
{
    int flags = SQLITE_OPEN_READWRITE;
    if (create_db)
    {
        flags |= SQLITE_OPEN_CREATE;
    }

    int ret = ::sqlite3_open_v2(db_file,
                                &sqlite3_handler_,
                                flags,
                                NULL);
    if (ret != SQLITE_OK )
    {
        ZCE_LOGMSG(RS_ERROR, "[zcelib] sqlite3_open_v2 fail:[%d][%s]",
                   error_code(),
                   error_message());
        return -1;
    }

    return 0;

}



//��ֻ���ķ�ʽ��һ�����ݿ�
//�������Ҫ3.5�Ժ�İ汾�ſ����á�
int ZCE_SQLite_DB_Handler::open_readonly_db(const char *db_file)
{

    int ret = ::sqlite3_open_v2(db_file,
                                &sqlite3_handler_,
                                SQLITE_OPEN_READONLY,
                                NULL);
    //
    if (ret != SQLITE_OK )
    {
        ZCE_LOGMSG(RS_ERROR, "[zcelib] sqlite3_open_v2 open readonly table fail:[%d][%s]",
                   error_code(),
                   error_message());
        return -1;
    }

    return 0;

}


//�ر����ݿ⡣
void ZCE_SQLite_DB_Handler::close_database()
{
    if (sqlite3_handler_)
    {
        ::sqlite3_close_v2(sqlite3_handler_);
        sqlite3_handler_ = NULL;
    }
}

//�������Str
const char *ZCE_SQLite_DB_Handler::error_message()
{
    return ::sqlite3_errmsg(sqlite3_handler_);
}

//DB���صĴ���ID
int ZCE_SQLite_DB_Handler::error_code()
{
    return ::sqlite3_errcode(sqlite3_handler_);
}

//��ʼһ������
int ZCE_SQLite_DB_Handler::begin_transaction()
{
    int ret = 0;
    char *err_msg = NULL;
    ret = ::sqlite3_exec(sqlite3_handler_,
                         "BEGIN TRANSACTION;",
                         NULL,
                         NULL,
                         &err_msg);

    if (ret == SQLITE_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

//�ύһ������
int ZCE_SQLite_DB_Handler::commit_transction()
{
    int ret = 0;
    char *err_msg = NULL;
    ret = ::sqlite3_exec(sqlite3_handler_,
                         "COMMIT TRANSACTION;",
                         NULL,
                         NULL,
                         &err_msg);

    if (ret == SQLITE_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

//��ͬ��ѡ��رգ������ʵ������insert���ٶȣ�����Ϊ�˰�ȫ��������鲻Ҫʹ��
int ZCE_SQLite_DB_Handler::turn_off_synch()
{
    int ret = 0;
    char *err_msg = NULL;
    ret = ::sqlite3_exec(sqlite3_handler_,
                         "PRAGMA synchronous=OFF;",
                         NULL,
                         NULL,
                         &err_msg);

    if (ret == SQLITE_OK)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

//ִ��SQL ��ѯ��ȡ�ý��
int ZCE_SQLite_DB_Handler::get_table(const char *sql_string,
                                     ZCE_SQLite_Result *result)
{
    int ret = SQLITE_OK;
    ret = ::sqlite3_get_table(sqlite3_handler_, sql_string,
                              &(result->result_),
                              &(result->row_),
                              &(result->column_),
                              &(result->err_msg_));
    if (ret != SQLITE_OK)
    {
        ZCE_LOGMSG(RS_ERROR, "[zcelib] sqlite3_get_table execute fail:[%d][%s]",
                   ret,
                   result->err_msg_);
    }
    return 0;
}

//=========================================================================================

ZCE_SQLite_Result::ZCE_SQLite_Result()
{

}


ZCE_SQLite_Result::~ZCE_SQLite_Result()
{

}

//�ͷŽ������
void ZCE_SQLite_Result::free_result()
{
    if (err_msg_)
    {
        ::sqlite3_free(err_msg_);
    }
    if (result_)
    {
        ::sqlite3_free_table(result_);
    }

    column_ = 0;
    row_ = 0;
}



#endif //#if SQLITE_VERSION_NUMBER >= 3005000

