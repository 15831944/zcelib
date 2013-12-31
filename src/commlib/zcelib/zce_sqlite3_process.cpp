
#include "zce_predefine.h"
#include "zce_sqlite3_process.h"

//����SQLITE����Ͱ汾����
#if SQLITE_VERSION_NUMBER >= 3003000

/******************************************************************************************
SQLite3_DB_Handler SQLite3DB Handler ���Ӵ���һ��SQLite3���ݿ��Handler
******************************************************************************************/
SQLite3_DB_Handler::SQLite3_DB_Handler():
    sqlite3_handler_(NULL)
{
}

SQLite3_DB_Handler::~SQLite3_DB_Handler()
{
    close_database();
}

//const char* db_file ,���ݿ������ļ�·��,�ӿ�Ҫ��UTF8���룬
//int == 0��ʾ�ɹ�������ʧ��
int SQLite3_DB_Handler::open_database(const char *db_file, bool string_ansi = true)
{
    //��δ����ǿͻ����ֵ��Ǽӵġ�
#ifdef ZCE_OS_WINDOWS
    //////////////////////////////////////////////////////////////////////////
    // Begin(��һ��ascii���ַ�ת����UTF-8)

    DWORD utf16_buffer_len = MultiByteToWideChar( CP_ACP, 0, db_file, -1, NULL, 0 );
    wchar_t *utf16_buffer = new wchar_t[utf16_buffer_len];

    // ��һ���Ȱ�ascii��ת����UTF-16
    MultiByteToWideChar( CP_ACP, 0, db_file, -1, utf16_buffer, utf16_buffer_len );

    DWORD utf8_buffer_len = WideCharToMultiByte( CP_UTF8, NULL, utf16_buffer, -1, NULL, 0, NULL, FALSE );
    char *utf8_buffer = new char[utf8_buffer_len];

    // �ڶ����ٰ�UTF-16����ת��ΪUTF-8����
    WideCharToMultiByte( CP_UTF8, NULL, utf16_buffer, -1, utf8_buffer, utf8_buffer_len, NULL, 0 );

    // End(��һ��ascii���ַ�ת����UTF-8)
    //////////////////////////////////////////////////////////////////////////

    int ret = sqlite3_open(utf8_buffer, &sqlite3_handler_);

    // ��Ҫ�ͷ�������ڴ�
    delete[] utf16_buffer;
    delete[] utf8_buffer;

    if (ret != SQLITE_OK )
    {
        return -1;
    }

    return 0;

#else

    int ret = sqlite3_open(db_file, &sqlite3_handler_);

    //
    if (ret != SQLITE_OK )
    {
        return -1;
    }

    return 0;
#endif //
}

//�������Ҫ3.5�Ժ�İ汾�ſ�����
#if SQLITE_VERSION_NUMBER >= 3005000

//��ֻ���ķ�ʽ��һ�����ݿ�
//�������Ҫ3.5�Ժ�İ汾�ſ����á�
//���ݿ��ļ�·��,��win32ƽ̨�Դ����������UTF-8ת�룬
int SQLite3_DB_Handler::open_readonly_db(const char *db_file)
{
    //��δ����ǿͻ����ֵ��Ǽӵġ�
#ifdef WIN32
    //////////////////////////////////////////////////////////////////////////
    // Begin(��һ��ascii���ַ�ת����UTF-8)

    DWORD utf16_buffer_len = MultiByteToWideChar( CP_ACP, 0, db_file, -1, NULL, 0 );
    wchar_t *utf16_buffer = new wchar_t[utf16_buffer_len];

    // ��һ���Ȱ�ascii��ת����UTF-16
    MultiByteToWideChar( CP_ACP, 0, db_file, -1, utf16_buffer, utf16_buffer_len );

    DWORD utf8_buffer_len = WideCharToMultiByte( CP_UTF8, NULL, utf16_buffer, -1, NULL, 0, NULL, FALSE );
    char *utf8_buffer = new char[utf8_buffer_len];

    // �ڶ����ٰ�UTF-16����ת��ΪUTF-8����
    WideCharToMultiByte( CP_UTF8, NULL, utf16_buffer, -1, utf8_buffer, utf8_buffer_len, NULL, 0 );

    // End(��һ��ascii���ַ�ת����UTF-8)
    //////////////////////////////////////////////////////////////////////////

    int ret = sqlite3_open_v2(utf8_buffer, &sqlite3_handler_, SQLITE_OPEN_READONLY, NULL);

    // ��Ҫ�ͷ�������ڴ�
    delete[] utf16_buffer;
    delete[] utf8_buffer;

    if (ret != SQLITE_OK )
    {
        return -1;
    }

    return 0;
#else
    int ret = sqlite3_open_v2(db_file, &sqlite3_handler_, SQLITE_OPEN_READONLY, NULL);

    //
    if (ret != SQLITE_OK )
    {
        return -1;
    }

    return 0;
#endif
}

#endif //#if SQLITE_VERSION_NUMBER >= 3005000

//�ر����ݿ⡣
void SQLite3_DB_Handler::close_database()
{
    if (sqlite3_handler_)
    {
        sqlite3_close(sqlite3_handler_);
        sqlite3_handler_ = NULL;
    }
}

//�������Str
const char *SQLite3_DB_Handler::get_dbret_errstr()
{
    return sqlite3_errmsg(sqlite3_handler_);
}

//DB���صĴ���ID
unsigned int SQLite3_DB_Handler::get_dbret_errid()
{
    return sqlite3_errcode(sqlite3_handler_);
}

//��ʼһ������
int SQLite3_DB_Handler::begin_transaction()
{
    int ret = 0;
    char *err_msg = NULL;
    ret = sqlite3_exec(sqlite3_handler_,
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
int SQLite3_DB_Handler::commit_transction()
{
    int ret = 0;
    char *err_msg = NULL;
    ret = sqlite3_exec(sqlite3_handler_,
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
int SQLite3_DB_Handler::turn_off_synch()
{
    int ret = 0;
    char *err_msg = NULL;
    ret = sqlite3_exec(sqlite3_handler_,
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

#endif //#if SQLITE_VERSION_NUMBER >= 3003000

