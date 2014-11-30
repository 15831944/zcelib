#include "zce_predefine.h"
#include "zce_sqlite_process.h"
#include "zce_sqlite_stmt_handler.h"
#include "zce_sqlite_conf_table.h"


//Ŀǰ�汾����ֻ����һ��
#if SQLITE_VERSION_NUMBER >= 3005000


/*****************************************************************************************************************
struct General_SQLite_Config һ����ͨ�õĴ�DB�м�õ�ͨ��������Ϣ�ķ���
*****************************************************************************************************************/
ZCE_General_Config_Table::ZCE_General_Config_Table()
{
    sql_string_ = new char[MAX_SQLSTRING_LEN];
    sqlite_handler_ = new ZCE_SQLite_DB_Handler();
}


ZCE_General_Config_Table::~ZCE_General_Config_Table()
{
    if (sql_string_)
    {
        delete sql_string_;
        sql_string_ = NULL;
    }
}


//��д��SQL
void ZCE_General_Config_Table::sql_replace_one(unsigned int table_id,
                                               unsigned int conf_id_1,
                                               unsigned int conf_id_2,
                                               unsigned int last_mod_time)
{
    //��������SQL
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    //ע�������?
    int len = snprintf(ptmppoint, buflen, "REPLACE INTO config_table_%u "
                       "(conf_id_1,conf_id_2,conf_data,last_mod_time ) VALUES "
                       "(%u,%u,?,%u) ",
                       table_id,
                       conf_id_1,
                       conf_id_2,
                       last_mod_time
                      );
    ptmppoint += len;
    buflen -= len;
}

//�õ�ѡ��һ��ȷ�����ݵ�SQL
void ZCE_General_Config_Table::sql_select_one(unsigned int table_id,
                                              unsigned int conf_id_1,
                                              unsigned int conf_id_2)
{
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    //����SQL
    int len = snprintf(ptmppoint, buflen, "SELECT conf_data,last_mod_time "
                       "FROM config_table_%u WHERE ((conf_id_1=%u) AND (conf_id_2=%u)) ",
                       table_id,
                       conf_id_1,
                       conf_id_2);
    ptmppoint += len;
    buflen -= len;
}

//�õ�ɾ�����ݵ�SQL
void ZCE_General_Config_Table::sql_delete_one(unsigned int table_id,
                                              unsigned int conf_id_1,
                                              unsigned int conf_id_2)
{

    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    int len = snprintf(ptmppoint,
                       buflen,
                       "DELETE FROM config_table_%u WHERE "
                       " ((conf_id_1=%u) AND (conf_id_2=%u)) ",
                       table_id,
                       conf_id_1,
                       conf_id_2);
    ptmppoint += len;
    buflen -= len;
}
//
void ZCE_General_Config_Table::sql_counter(unsigned int table_id)
{
    //����SQL
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    int len = snprintf(ptmppoint, buflen, "SELECT COUNT(*) FROM config_table_%u ",
                       table_id);
    ptmppoint += len;
    buflen -= len;
}

//
void ZCE_General_Config_Table::sql_select_array(unsigned int table_id,
                                                unsigned int startno,
                                                unsigned int numquery)
{
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    //����SQL
    int len = snprintf(ptmppoint, buflen, "SELECT conf_data "
                       "FROM config_table_%u ",
                       table_id);
    ptmppoint += len;
    buflen -= len;

    //���Ҫ��ѯLIMIT����Ŀ
    if (numquery != 0)
    {
        len = snprintf(ptmppoint, buflen, "LIMIT %u,%u ", startno, numquery);
        ptmppoint += len;
        buflen -= len;
    }
}

///
int ZCE_General_Config_Table::replace_one(unsigned int table_id,
                                          unsigned int conf_id_1,
                                          unsigned int conf_id_2,
                                          const AI_IIJIMA_BINARY_DATA &conf_data,
                                          unsigned int last_mod_time)
{
    //��������SQL
    sql_replace_one(table_id, conf_id_1, conf_id_2, last_mod_time);
    ZCE_SQLite_STMTHdl stmt_handler(sqlite_handler_);
    int ret = 0;
    ret = stmt_handler.prepare_sql_string(sql_string_);
    if (ret != 0)
    {
        return ret;
    }

    stmt_handler << ZCE_SQLite_STMTHdl::BINARY((void *)conf_data.ai_iijima_data_,
                                               conf_data.ai_data_length_);
    if (ret != 0)
    {
        return ret;
    }

    bool hash_result = false;
    ret = stmt_handler.execute_stmt_sql(hash_result);
    if (ret != 0)
    {
        return ret;
    }
    return 0;
}

///
int ZCE_General_Config_Table::select_one(unsigned int table_id,
                                         unsigned int conf_id_1,
                                         unsigned int conf_id_2,
                                         AI_IIJIMA_BINARY_DATA &conf_data,
                                         unsigned int &last_mod_time)
{
    sql_select_one(table_id, conf_id_1, conf_id_2);
    ZCE_SQLite_STMTHdl stmt_handler(sqlite_handler_);
    int ret = 0;
    ret = stmt_handler.prepare_sql_string(sql_string_);
    if (ret != 0)
    {
        return ret;
    }

    bool hash_result = false;
    ret = stmt_handler.execute_stmt_sql(hash_result);
    if (ret != 0 )
    {
        return ret;
    }

    if (false == hash_result)
    {
        return -1;
    }


    stmt_handler >> ZCE_SQLite_STMTHdl::BINARY((void *)conf_data.ai_iijima_data_,
                                               conf_data.ai_data_length_);
    stmt_handler >> last_mod_time ;

    return 0;
}

///
int ZCE_General_Config_Table::delete_one(unsigned int table_id,
                                         unsigned int conf_id_1,
                                         unsigned int conf_id_2)
{
    //��������SQL
    sql_delete_one(table_id, conf_id_1, conf_id_2);
    ZCE_SQLite_STMTHdl stmt_handler(sqlite_handler_);
    int ret = 0;
    ret = stmt_handler.prepare_sql_string(sql_string_);
    if (ret != 0)
    {
        return ret;
    }
    bool hash_result = false;
    ret = stmt_handler.execute_stmt_sql(hash_result);
    if (ret != 0)
    {
        return ret;
    }
    return 0;
}

///
int ZCE_General_Config_Table::counter(unsigned int table_id,
                                      unsigned int &rec_count)
{
    sql_counter(table_id);
    ZCE_SQLite_STMTHdl stmt_handler(sqlite_handler_);
    int ret = 0;
    ret = stmt_handler.prepare_sql_string(sql_string_);
    if (ret != 0)
    {
        return ret;
    }

    bool hash_result = false;
    ret = stmt_handler.execute_stmt_sql(hash_result);
    if (ret != 0)
    {
        return ret;
    }

    if (false == hash_result)
    {
        return -1;
    }

    stmt_handler >> rec_count;
    return 0;
}

///
int ZCE_General_Config_Table::select_array(unsigned int table_id,
                                           unsigned int startno,
                                           unsigned int numquery,
                                           ARRARY_OF_AI_IIJIMA_BINARY &ary_ai_iijma)
{
    sql_select_array(table_id, startno, numquery);
    ZCE_SQLite_STMTHdl stmt_handler(sqlite_handler_);
    int ret = 0;
    ret = stmt_handler.prepare_sql_string(sql_string_);
    if (ret != 0)
    {
        return ret;
    }
    bool hash_result;
    ret = stmt_handler.execute_stmt_sql(hash_result);
    while (ret == 0 && hash_result == true)
    {

        //ary_ai_iijma.push_back(house_info);
        ret = stmt_handler.execute_stmt_sql(hash_result);
    }

    //���ִ������û���ҵ�
    if (0 != ret)
    {
        return ret;
    }

    //û���ҵ�����
    if (ary_ai_iijma.size() == 0)
    {
        return -1;
    }
    return 0;
}


#endif //SQLITE_VERSION_NUMBER >= 3005000



