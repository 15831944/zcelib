
#include "zce_predefine.h"

//����SQLITE����Ͱ汾����
#if SQLITE_VERSION_NUMBER >= 3003000

#include "zce_trace_log_debug.h"
#include "zce_sqlite3_process.h"
#include "zce_sqlite3_stmt_handler.h"





/******************************************************************************************
SQLite3_STMT_Handler
******************************************************************************************/
//���캯��,�Ӻ�ϸС�ĵط��Ϳ��Կ���SQLITE������в��㣬һ��INDEX��1��ʼ��1����0
SQLite3_STMT_Handler::SQLite3_STMT_Handler(SQLite3_DB_Handler *sqlite3_handler):
    sqlite3_db_handler_(sqlite3_handler),
    sqlite3_stmt_handler_(NULL),
    current_bind_(1),
    current_col_(0)
{
    assert(sqlite3_handler != NULL && sqlite3_handler->get_sqlite_handler() != NULL);
}

SQLite3_STMT_Handler::~SQLite3_STMT_Handler()
{
    finalize_stmt_handler();
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��12��
Function        : SQLite3_STMT_Handler::finalize_stmt_handler
Return          : int ==0 ��ʾ�ɹ�������ʧ��
Parameter List  : NULL
Description     : ����SQLITE3��STMT HANDLER���ָ���ʼ��ֵ�ȡ�
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int SQLite3_STMT_Handler::finalize_stmt_handler()
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
int SQLite3_STMT_Handler::reset_stmt_handler()
{
    int ret =   sqlite3_reset(sqlite3_stmt_handler_);

    if ( SQLITE_OK  != ret)
    {
        return -1;
    }

    current_bind_ = 1;
    current_col_ = 0;

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��12��
Function        : SQLite3_STMT_Handler::prepare_sql_string
Return          : int == 0
Parameter List  :
  Param1: const char* sql_string
Description     : ����SQL��䣬����Ƿ��ܹ���ȷִ��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int SQLite3_STMT_Handler::prepare_sql_string(const char *sql_string)
{
    if (sqlite3_stmt_handler_)
    {
        finalize_stmt_handler();
    }

    int ret =  sqlite3_prepare(sqlite3_db_handler_->get_sqlite_handler(),
                               sql_string,
                               -1,                                      //ע���������������С��0
                               &sqlite3_stmt_handler_,
                               NULL);

    //�������������󣬻��߲���һ��SQL
    if ( SQLITE_OK  != ret || sqlite3_stmt_handler_ == NULL)
    {
        //�������ش���
        ZLOG_ERROR("[zcelib] Error:[%d][%s]", get_dbret_error_id(), get_db_reterror_str());
        return -1;
    }

    //
    current_bind_ = 1;
    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��12��
Function        : SQLite3_STMT_Handler::execute_stmt_sql
Return          : int ==0 ��ʾ�ɹ�
Parameter List  :
  Param1: bool& has_reuslt �Ƿ��н��
Description     :
Calls           :
Called By       :
Other           : Ҫִ�ж�Σ���һ�εõ�������ϣ������ƶ��αꡣ
Modify Record   :
******************************************************************************************/
int SQLite3_STMT_Handler::execute_stmt_sql(bool &has_reuslt)
{
    has_reuslt = false;
    //
    int ret = sqlite3_step(sqlite3_stmt_handler_);

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
        return 0;
    }

    //�������ش���
    ZLOG_ERROR("[zcelib] Error:[%d][%s]", get_dbret_error_id(), get_db_reterror_str());
    return -1;
}

//���ص�ǰ�еĳ���
unsigned int SQLite3_STMT_Handler::get_cur_field_length()
{
    return sqlite3_column_bytes(sqlite3_stmt_handler_, current_col_);
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (char tmpvalue)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( tmpvalue));

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (short tmpvalue)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( tmpvalue));

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (int tmpvalue)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, tmpvalue);

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (long tmpvalue)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( tmpvalue));

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (long long tmpvalue)
{
    int ret = sqlite3_bind_int64(sqlite3_stmt_handler_, current_bind_, tmpvalue);

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (unsigned char tmpvalue)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( tmpvalue));

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (unsigned short tmpvalue)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( tmpvalue));

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (unsigned int tmpvalue)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, tmpvalue);

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (unsigned long tmpvalue)
{
    int ret = sqlite3_bind_int(sqlite3_stmt_handler_, current_bind_, static_cast<int>( tmpvalue));

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (unsigned long long tmpvalue)
{
    int ret = sqlite3_bind_int64(sqlite3_stmt_handler_, current_bind_, tmpvalue);

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (float tmpvalue)
{
    int ret = sqlite3_bind_double(sqlite3_stmt_handler_, current_bind_, static_cast<double>( tmpvalue));

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (double tmpvalue)
{

    int ret = sqlite3_bind_double(sqlite3_stmt_handler_, current_bind_, tmpvalue);

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (const char *tmpvalue)
{
    //SQLITE_TRANSIENT�Ǳ�ʾ�ײ�Ḵ��������������������е��Ρ�
    int ret = sqlite3_bind_text(sqlite3_stmt_handler_,
                                current_bind_,
                                tmpvalue,
                                static_cast<int>(strlen(tmpvalue)),
                                SQLITE_TRANSIENT);

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (const  std::string &tmpvalue)
{
    //SQLITE_TRANSIENT�Ǳ�ʾ�ײ�Ḵ��������������������е��Ρ�
    //
    int ret = sqlite3_bind_text(sqlite3_stmt_handler_,
                                current_bind_,
                                tmpvalue.c_str(),
                                static_cast<int>(tmpvalue.length()),
                                SQLITE_TRANSIENT);

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator << (const SQLite3_STMT_Handler::BINARY &tmpvalue)
{
    //SQLITE_TRANSIENT�Ǳ�ʾ�ײ�Ḵ��������������������е��Ρ�
    int ret = sqlite3_bind_blob(sqlite3_stmt_handler_,
                                current_bind_,
                                tmpvalue.binary_data_,
                                tmpvalue.binary_len_,
                                SQLITE_TRANSIENT);

    if ( SQLITE_OK  != ret)
    {
        return *this;
    }

    ++current_bind_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (char &tmpvalue)
{
    tmpvalue = static_cast<char>( sqlite3_column_int(sqlite3_stmt_handler_, current_col_));

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (short &tmpvalue)
{
    tmpvalue = static_cast<short>( sqlite3_column_int(sqlite3_stmt_handler_, current_col_));

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (int &tmpvalue)
{
    tmpvalue = sqlite3_column_int(sqlite3_stmt_handler_, current_col_);

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (long &tmpvalue)
{
    tmpvalue  = sqlite3_column_int(sqlite3_stmt_handler_, current_col_);

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (long long &tmpvalue)
{
    tmpvalue = sqlite3_column_int64(sqlite3_stmt_handler_, current_col_);

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (unsigned char &tmpvalue)
{
    tmpvalue = static_cast<unsigned char>( sqlite3_column_int(sqlite3_stmt_handler_, current_col_));
    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (unsigned short &tmpvalue)
{
    tmpvalue = static_cast<unsigned short>( sqlite3_column_int(sqlite3_stmt_handler_, current_col_));

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (unsigned int &tmpvalue)
{
    tmpvalue = static_cast<unsigned int>(  sqlite3_column_int(sqlite3_stmt_handler_, current_col_));

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (unsigned long &tmpvalue)
{
    tmpvalue  = static_cast<unsigned long>(  sqlite3_column_int(sqlite3_stmt_handler_, current_col_));

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (unsigned long long &tmpvalue)
{
    tmpvalue = static_cast<unsigned long long > (sqlite3_column_int64(sqlite3_stmt_handler_, current_col_));

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (float &tmpvalue)
{
    tmpvalue = static_cast<float > (sqlite3_column_double(sqlite3_stmt_handler_, current_col_));

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (double &tmpvalue)
{
    tmpvalue = sqlite3_column_double(sqlite3_stmt_handler_, current_col_);

    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (char *tmpvalue)
{
    //Fisk�����̬���Ҹ��˵ط���Ϊ�˰�ȫ��顣
    strncpy(tmpvalue,
            reinterpret_cast<const char *>(sqlite3_column_text(sqlite3_stmt_handler_, current_col_)),
            sqlite3_column_bytes(sqlite3_stmt_handler_, current_col_) + 1);
    ++current_col_ ;
    return *this;
}
//�����Ƶ�����Ҫ�ر���һ��,�ַ������ر�+1��,�����������ݲ�Ҫ��������
SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (SQLite3_STMT_Handler::BINARY &tmpvalue)
{
    tmpvalue.binary_len_ = sqlite3_column_bytes(sqlite3_stmt_handler_, current_col_);
    //Ϊ�˻�ȡ���������ݣ���ZCE_Mysql_Result���Ӧ,���Ȳ�+1
    memcpy(tmpvalue.binary_data_, sqlite3_column_blob(sqlite3_stmt_handler_, current_col_),
           tmpvalue.binary_len_);
    ++current_col_ ;
    return *this;
}

SQLite3_STMT_Handler &SQLite3_STMT_Handler::operator >> (std::string &tmpvalue)
{
    tmpvalue = reinterpret_cast<const char *>(sqlite3_column_text(sqlite3_stmt_handler_, current_col_));
    ++current_col_ ;
    return *this;
}

//
void SQLite3_STMT_Handler::get_column_count(int &num_col)
{
    num_col = sqlite3_column_count(sqlite3_stmt_handler_);
}

//��ʼһ������
int SQLite3_STMT_Handler::begin_transaction()
{
    return sqlite3_db_handler_->begin_transaction();
}

//�ύһ������
int SQLite3_STMT_Handler::commit_transction()
{
    return sqlite3_db_handler_->commit_transction();
}

int SQLite3_STMT_Handler::turn_off_synch()
{
    return sqlite3_db_handler_->turn_off_synch();
}

#endif //#if SQLITE_VERSION_NUMBER >= 3003000

