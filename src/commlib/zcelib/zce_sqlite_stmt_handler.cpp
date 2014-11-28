
#include "zce_predefine.h"

//����SQLITE����Ͱ汾����
#if SQLITE_VERSION_NUMBER >= 3005000

#include "zce_trace_log_debug.h"
#include "zce_sqlite_process.h"
#include "zce_sqlite_stmt_handler.h"



/******************************************************************************************
SQLite3_STMT_Handler
******************************************************************************************/
//���캯��,�Ӻ�ϸС�ĵط��Ϳ��Կ���SQLITE������в��㣬һ��INDEX��1��ʼ��1����0
SQLite_STMT_Handler::SQLite_STMT_Handler(SQLite_DB_Handler *sqlite3_handler):
    sqlite_handler_(sqlite3_handler),
    sqlite3_stmt_handler_(NULL),
    current_bind_(1),
    current_col_(0)
{
    assert(sqlite3_handler != NULL && sqlite3_handler->get_sqlite_handler() != NULL);
}

SQLite_STMT_Handler::~SQLite_STMT_Handler()
{
    finalize_stmt_handler();
}


//����SQLITE3��STMT HANDLER���ָ���ʼ��ֵ�ȡ�
int SQLite_STMT_Handler::finalize_stmt_handler()
{
    //����SQLITE3��STMT HANDLER
    int ret =  sqlite3_finalize(sqlite3_stmt_handler_);

    if ( SQLITE_OK  != ret)
    {
        return -1;
    }

    sqlite3_stmt_handler_ = NULL;
    current_bind_ = 1;
    current_col_ = 0;
    return 0;
}

//
int SQLite_STMT_Handler::reset_stmt_handler()
{
    int ret = ::sqlite3_reset(sqlite3_stmt_handler_);

    if ( SQLITE_OK  != ret)
    {
        return -1;
    }

    current_bind_ = 1;
    current_col_ = 0;

    return 0;
}


//����SQL��䣬����Ƿ��ܹ���ȷִ��
int SQLite_STMT_Handler::prepare_sql_string(const char *sql_string)
{
    if (sqlite3_stmt_handler_)
    {
        finalize_stmt_handler();
    }

    int ret =  ::sqlite3_prepare_v2(sqlite_handler_->get_sqlite_handler(),
                                    sql_string,
                                    -1,                                      //ע���������������С��0
                                    &sqlite3_stmt_handler_,
                                    NULL);

    //�������������󣬻��߲���һ��SQL
    if ( SQLITE_OK  != ret || sqlite3_stmt_handler_ == NULL)
    {
        //�������ش���
        ZLOG_ERROR("[zcelib] Error:[%d][%s]",
                   error_code(),
                   error_message());
        return -1;
    }

    //
    current_bind_ = 1;
    return 0;
}

//
int SQLite_STMT_Handler::execute_stmt_sql(bool &has_reuslt)
{
    has_reuslt = false;
    //
    int ret = ::sqlite3_step(sqlite3_stmt_handler_);

    //
    if (SQLITE_ROW == ret)
    {
        has_reuslt = true;
        current_col_ = 0;
        return 0;
    }
    //
    else if (SQLITE_DONE == ret)
    {
        has_reuslt = false;
        return 0;
    }

    //�������ش���
    ZLOG_ERROR("[zcelib] Error:[%d][%s]", error_code(), error_message());
    return -1;
}

//Bind ����Ⱥ
template<>
int SQLite_STMT_Handler::bind(char val)
{
    int ret = ::sqlite3_bind_int(sqlite3_stmt_handler_,
                                 current_bind_,
                                 static_cast<int>( val));
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(short val)
{
    int ret = ::sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( val));
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(int val)
{
    int ret = ::sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, val);
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]",
                   error_code(),
                   error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(long val)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( val));
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]",
                   error_code(),
                   error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(long long val)
{
    int ret = sqlite3_bind_int64(sqlite3_stmt_handler_, current_bind_, val);
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]",
                   error_code(),
                   error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(unsigned char val)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( val));
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]",
                   error_code(),
                   error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(unsigned short val)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( val));
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return ret;
}

template<>
int SQLite_STMT_Handler::bind(unsigned int val)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, val);
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(unsigned long val)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( val));
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(unsigned long long val)
{
    int ret = sqlite3_bind_int64(sqlite3_stmt_handler_, current_bind_, val);
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(float val)
{
    int ret = sqlite3_bind_double(sqlite3_stmt_handler_, current_bind_, static_cast<double>( val));
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(double val)
{

    int ret = sqlite3_bind_double(sqlite3_stmt_handler_, current_bind_, val);
    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(const char *val)
{
    //SQLITE_TRANSIENT�Ǳ�ʾ�ײ�Ḵ��������������������е��Ρ�
    int ret = sqlite3_bind_text(sqlite3_stmt_handler_,
                                current_bind_,
                                val,
                                static_cast<int>(strlen(val)),
                                SQLITE_TRANSIENT);

    if ( SQLITE_OK  != ret)
    {
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(const  std::string &val)
{
    //SQLITE_TRANSIENT�Ǳ�ʾ�ײ�Ḵ��������������������е��Ρ�
    //
    int ret = sqlite3_bind_text(sqlite3_stmt_handler_,
                                current_bind_,
                                val.c_str(),
                                static_cast<int>(val.length()),
                                SQLITE_TRANSIENT);

    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
int SQLite_STMT_Handler::bind(const SQLite_STMT_Handler::BINARY &val)
{
    //SQLITE_TRANSIENT�Ǳ�ʾ�ײ�Ḵ��������������������е��Ρ�
    int ret = sqlite3_bind_blob(sqlite3_stmt_handler_,
                                current_bind_,
                                val.binary_data_,
                                val.binary_len_,
                                SQLITE_TRANSIENT);

    if ( SQLITE_OK  != ret)
    {
        ZLOG_ERROR("[zcelib] bind error :[%d][%s]", error_code(), error_message());
        return ret;
    }

    ++current_bind_ ;
    return 0;
}

template<>
void SQLite_STMT_Handler::column(char &val)
{
    val = static_cast<char>( sqlite3_column_int(sqlite3_stmt_handler_, current_col_));

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(short &val)
{
    val = static_cast<short>( sqlite3_column_int(sqlite3_stmt_handler_,
                                                 current_col_));

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(int &val)
{
    val = sqlite3_column_int(sqlite3_stmt_handler_,
                             current_col_);

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(long &val)
{
    val  = sqlite3_column_int(sqlite3_stmt_handler_,
                              current_col_);

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(long long &val)
{
    val = sqlite3_column_int64(sqlite3_stmt_handler_,
                               current_col_);

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(unsigned char &val)
{
    val = static_cast<unsigned char>( sqlite3_column_int(sqlite3_stmt_handler_,
                                                         current_col_));
    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(unsigned short &val)
{
    val = static_cast<unsigned short>( sqlite3_column_int(sqlite3_stmt_handler_,
                                                          current_col_));

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(unsigned int &val)
{
    val = static_cast<unsigned int>(  sqlite3_column_int(sqlite3_stmt_handler_,
                                                         current_col_));

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(unsigned long &val)
{
    val  = static_cast<unsigned long>(  sqlite3_column_int(sqlite3_stmt_handler_,
                                                           current_col_));

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(unsigned long long &val)
{
    val = static_cast<unsigned long long > (sqlite3_column_int64(sqlite3_stmt_handler_,
                                                                 current_col_));

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(float &val)
{
    val = static_cast<float > (sqlite3_column_double(sqlite3_stmt_handler_,
                                                     current_col_));

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(double &val)
{
    val = sqlite3_column_double(sqlite3_stmt_handler_,
                                current_col_);

    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(char *val)
{
    //Fisk�����̬���Ҹ��˵ط���Ϊ�˰�ȫ��顣
    strncpy(val,
            reinterpret_cast<const char *>(sqlite3_column_text(sqlite3_stmt_handler_,
                                                               current_col_)),
            sqlite3_column_bytes(sqlite3_stmt_handler_, current_col_) + 1);
    ++current_col_ ;
    return;
}

//�����Ƶ�����Ҫ�ر���һ��,�ַ������ر�+1��,�����������ݲ�Ҫ��������
template<>
void SQLite_STMT_Handler::column(SQLite_STMT_Handler::BINARY &val)
{
    val.binary_len_ = sqlite3_column_bytes(sqlite3_stmt_handler_, current_col_);
    //Ϊ�˻�ȡ���������ݣ���ZCE_Mysql_Result���Ӧ,���Ȳ�+1
    memcpy(val.binary_data_, sqlite3_column_blob(sqlite3_stmt_handler_, current_col_),
           val.binary_len_);
    ++current_col_ ;
    return;
}

template<>
void SQLite_STMT_Handler::column(std::string &val)
{
    val = reinterpret_cast<const char *>(sqlite3_column_text(sqlite3_stmt_handler_,
                                                             current_col_));
    ++current_col_ ;
    return;
}



//��ʼһ������
int SQLite_STMT_Handler::begin_transaction()
{
    return sqlite_handler_->begin_transaction();
}

//�ύһ������
int SQLite_STMT_Handler::commit_transction()
{
    return sqlite_handler_->commit_transction();
}

int SQLite_STMT_Handler::turn_off_synch()
{
    return sqlite_handler_->turn_off_synch();
}

#endif //#if SQLITE_VERSION_NUMBER >= 3005000

