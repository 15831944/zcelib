#include "zce_predefine.h"
#include "zce_bytes_encode.h"
#include "zce_sqlite_process.h"
#include "zce_sqlite_stmt_handler.h"
#include "zce_sqlite_conf_table.h"


//Ŀǰ�汾����ֻ����һ��
#if SQLITE_VERSION_NUMBER >= 3005000


//�������������
AI_IIJIMA_BINARY_DATA::AI_IIJIMA_BINARY_DATA()
{
    ai_iijima_data_[MAX_LEN_OF_AI_IIJIMA_DATA] = '\0';
}
AI_IIJIMA_BINARY_DATA::~AI_IIJIMA_BINARY_DATA()
{

}

//������
void AI_IIJIMA_BINARY_DATA::clear()
{
    index_1_ = 0;
    index_2_ = 0;
    ai_data_length_ = 0;
    ai_iijima_data_[MAX_LEN_OF_AI_IIJIMA_DATA] = '\0';
    last_mod_time_ = 0;
}

//�ȽϺ���
bool AI_IIJIMA_BINARY_DATA::operator < (const AI_IIJIMA_BINARY_DATA right)
{
    if (this->index_1_ < right.index_1_)
    {
        return true;
    }
    else if (this->index_1_ == right.index_1_)
    {
        if (this->index_2_ < right.index_2_)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

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
    if (sqlite_handler_)
    {
        delete sqlite_handler_;
        sqlite_handler_ = NULL;
    }
}

//��һ��ͨ�õ����ݿ�
int ZCE_General_Config_Table::open_dbfile(const char *db_file,
                                          bool create_db)
{
    int ret = sqlite_handler_->open_database(db_file, create_db);
    if (ret != 0)
    {
        return ret;
    }
    return 0;
}

//����TABLE SQL���
void ZCE_General_Config_Table::sql_create_table(unsigned  int table_id)
{
    //��������SQL
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    int len = snprintf(ptmppoint, buflen,
                       "CREATE TABLE IF NOT EXISTS config_table_%u(index_1 INTEGER,"
                       "index_2 INTEGER, conf_data BLOB, last_mod_time INTEGER);"
                       "CREATE UNIQUE INDEX IF NOT EXISTS cfg_table_idx_%u ON "
                       "config_table_%u(index_1, index_2);",
                       table_id,
                       table_id,
                       table_id);
    ptmppoint += len;
    buflen -= len;


}

//��д��SQL
void ZCE_General_Config_Table::sql_replace_stmt(unsigned int table_id,
                                                unsigned int index_1,
                                                unsigned int index_2,
                                                unsigned int last_mod_time)
{
    //��������SQL
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    //ע�������?
    int len = snprintf(ptmppoint, buflen, "REPLACE INTO config_table_%u "
                       "(index_1,index_2,conf_data,last_mod_time ) VALUES ;"
                       "(%u,%u,?,%u) ",
                       table_id,
                       index_1,
                       index_2,
                       last_mod_time
                      );
    ptmppoint += len;
    buflen -= len;
}


///��д��SQL,�ı���ʽ����x
void ZCE_General_Config_Table::sql_replace_one(unsigned  int table_id,
                                               unsigned int index_1,
                                               unsigned int index_2,
                                               size_t blob_len,
                                               const char *blob_data,
                                               unsigned int last_mod_time)
{
    //��������SQL
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    //���ڿռ䣬������Ԥ�����㹻�Ŀռ��
    int len = snprintf(ptmppoint, buflen, "REPLACE INTO config_table_%u "
                       "(index_1,index_2,conf_data,last_mod_time ) VALUES ;"
                       "(%u,%u,x'",
                       table_id,
                       index_1,
                       index_2 );

    ptmppoint += len;
    buflen -= len;

    size_t out_len = buflen;
    ZCE_LIB::base16_encode((const unsigned char *)blob_data,
                           blob_len,
                           (unsigned char *)ptmppoint,
                           &out_len);
    ptmppoint += out_len;
    buflen -= out_len;

    len = snprintf(ptmppoint, buflen, "',%u);", last_mod_time);
    ptmppoint += out_len;
    buflen -= out_len;

}


//�õ�ѡ��һ��ȷ�����ݵ�SQL
void ZCE_General_Config_Table::sql_select_one(unsigned int table_id,
                                              unsigned int index_1,
                                              unsigned int index_2)
{
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    //����SQL
    int len = snprintf(ptmppoint, buflen, "SELECT conf_data,last_mod_time "
                       "FROM config_table_%u WHERE ((index_1=%u) AND (index_2=%u)) ",
                       table_id,
                       index_1,
                       index_2);
    ptmppoint += len;
    buflen -= len;
}

//�õ�ɾ�����ݵ�SQL
void ZCE_General_Config_Table::sql_delete_one(unsigned int table_id,
                                              unsigned int index_1,
                                              unsigned int index_2)
{

    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    int len = snprintf(ptmppoint,
                       buflen,
                       "DELETE FROM config_table_%u WHERE "
                       " ((index_1=%u) AND (index_2=%u)) ;",
                       table_id,
                       index_1,
                       index_2);
    ptmppoint += len;
    buflen -= len;
}
//
void ZCE_General_Config_Table::sql_counter(unsigned int table_id,
                                           unsigned int startno,
                                           unsigned int numquery)
{
    //����SQL
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    int len = snprintf(ptmppoint, buflen, "SELECT COUNT(*) FROM config_table_%u ",
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

//
void ZCE_General_Config_Table::sql_select_array(unsigned int table_id,
                                                unsigned int startno,
                                                unsigned int numquery)
{
    char *ptmppoint = sql_string_;
    size_t buflen = MAX_SQLSTRING_LEN;

    //����SQL
    int len = snprintf(ptmppoint, buflen, "SELECT index_1,index_2,conf_data,last_mod_time "
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

///�������ݱ�
int ZCE_General_Config_Table::create_table(unsigned int table_id)
{

    //����ͽ�������
    sql_create_table(table_id);
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


//����һ����¼��
int ZCE_General_Config_Table::replace_one(unsigned int table_id,
                                          const AI_IIJIMA_BINARY_DATA &conf_data)
{
    //��������SQL
    sql_replace_stmt(table_id,
                     conf_data.index_1_,
                     conf_data.index_2_,
                     conf_data.last_mod_time_);
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

//
int ZCE_General_Config_Table::select_one(unsigned int table_id,
                                         AI_IIJIMA_BINARY_DATA &conf_data)
{
    sql_select_one(table_id,
                   conf_data.index_1_,
                   conf_data.index_2_);
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
    stmt_handler >> conf_data.last_mod_time_;

    return 0;
}

//ɾ��һ����¼
int ZCE_General_Config_Table::delete_one(unsigned int table_id,
                                         unsigned int index_1,
                                         unsigned int index_2)
{
    //��������SQL
    sql_delete_one(table_id, index_1, index_2);
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

//
int ZCE_General_Config_Table::counter(unsigned int table_id,
                                      unsigned int startno,
                                      unsigned int numquery,
                                      unsigned int &rec_count)
{
    sql_counter(table_id, startno, numquery);
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

//��ѯ���еĶ���
int ZCE_General_Config_Table::select_array(unsigned int table_id,
                                           unsigned int startno,
                                           unsigned int numquery,
                                           ARRARY_OF_AI_IIJIMA_BINARY &ary_ai_iijma)
{
    int ret = 0;

    //�ȼ�������
    unsigned int  num_counter = 0;
    ret = counter(table_id, startno, numquery, num_counter);
    if (0 != ret)
    {
        return ret;
    }

    //û���ҵ�����
    if (num_counter == 0)
    {
        return -1;
    }
    ary_ai_iijma.resize(num_counter);

    sql_select_array(table_id, startno, numquery);
    ZCE_SQLite_STMTHdl stmt_handler(sqlite_handler_);

    ret = stmt_handler.prepare_sql_string(sql_string_);
    if (ret != 0)
    {
        return ret;
    }

    bool hash_result;
    ret = stmt_handler.execute_stmt_sql(hash_result);

    for (size_t i = 0; ret == 0 && hash_result == true; ++i)
    {
        stmt_handler >> ary_ai_iijma[i].index_1_;
        stmt_handler >> ary_ai_iijma[i].index_2_;
        stmt_handler >> ZCE_SQLite_STMTHdl::BINARY((void *)ary_ai_iijma[i].ai_iijima_data_,
                                                   ary_ai_iijma[i].ai_data_length_);
        stmt_handler >> ary_ai_iijma[i].last_mod_time_;

        ret = stmt_handler.execute_stmt_sql(hash_result);
    }

    //���ִ������û���ҵ�
    if (0 != ret)
    {
        return ret;
    }

    return 0;
}


//�Ա��������ݱ���ҳ����죬Ȼ���ҳ������SQL
int ZCE_General_Config_Table::compare_table(const char *old_db,
                                            const char *new_db,
                                            unsigned int table_id,
                                            std::string &update_sql)
{
    int ret = 0;



    //��ȡ������
    ret = open_dbfile(old_db, false);
    if (0 != ret)
    {
        return ret;
    }
    ARRARY_OF_AI_IIJIMA_BINARY old_ai_iijma;
    ret = select_array(table_id,
                       0,
                       0,
                       old_ai_iijma);
    if (0 != ret)
    {
        return ret;
    }

    //��ȡ������
    ret = open_dbfile(new_db, false);
    if (0 != ret)
    {
        return ret;
    }
    ARRARY_OF_AI_IIJIMA_BINARY new_ai_iijma;
    ret = select_array(table_id,
                       0,
                       0,
                       new_ai_iijma);
    if (0 != ret)
    {
        return ret;
    }

    //���¾��������򣬷���Ƚ�
    std::sort(old_ai_iijma.begin(), old_ai_iijma.end());
    std::sort(new_ai_iijma.begin(), new_ai_iijma.end());

    update_sql.reserve(MAX_SQLSTRING_LEN);

    //�����������ҳ������Ԫ��
    size_t p = 0, q = 0;
    for (; p < old_ai_iijma.size();)
    {
        //����Աȵ��������
        if (old_ai_iijma[p].index_1_ == new_ai_iijma[q].index_1_ &&
            old_ai_iijma[p].index_2_ == new_ai_iijma[q].index_2_)
        {
            if (old_ai_iijma[p].ai_data_length_ == new_ai_iijma[q].ai_data_length_ &&
                0 == ::memcmp(old_ai_iijma[p].ai_iijima_data_,
                              new_ai_iijma[q].ai_iijima_data_,
                              old_ai_iijma[p].ai_data_length_))
            {
                //old[p] = new[q]��ͬ��
                ++p;
                ++q;
            }
            else
            {
                //old[p] != new[q],�Աȵļ�¼����ͬ��REPLACE
                sql_replace_one(table_id,
                                new_ai_iijma[q].index_1_,
                                new_ai_iijma[q].index_2_,
                                new_ai_iijma[q].ai_data_length_,
                                new_ai_iijma[q].ai_iijima_data_,
                                new_ai_iijma[q].last_mod_time_);
                update_sql += sql_string_;

                ++p;
                ++q;

            }

        }
        //������ new ����Ѱ�� old[p]
        else
        {
            size_t r = q++;
            for (; r < new_ai_iijma.size();)
            {
                if (old_ai_iijma[p].index_1_ == new_ai_iijma[r].index_1_ &&
                    old_ai_iijma[p].index_2_ == new_ai_iijma[r].index_2_)
                {
                    bool r_is_equal = false;
                    if (old_ai_iijma[p].ai_data_length_ == new_ai_iijma[r].ai_data_length_ &&
                        0 == ::memcmp(old_ai_iijma[p].ai_iijima_data_,
                                      new_ai_iijma[r].ai_iijima_data_,
                                      old_ai_iijma[p].ai_data_length_))
                    {
                        r_is_equal = true;
                    }

                    size_t end_pos = r_is_equal ? r - 1 : r;
                    for (size_t s = q; s < end_pos; ++s)
                    {
                        //new[q] �� new[r] ���������ģ�REPLACE
                        sql_replace_one(table_id,
                                        new_ai_iijma[s].index_1_,
                                        new_ai_iijma[s].index_2_,
                                        new_ai_iijma[s].ai_data_length_,
                                        new_ai_iijma[s].ai_iijima_data_,
                                        new_ai_iijma[s].last_mod_time_);
                        update_sql += sql_string_;
                    }

                    break;

                }

                //
            }


            if (r < new_ai_iijma.size())
            {
                ++p;
                q = r++;
            }
            //old[p] �Ƕ���ģ�DELETE
            else
            {

                sql_delete_one(table_id, old_ai_iijma[p].index_1_, old_ai_iijma[p].index_2_);
                update_sql += sql_string_;
                ++p;
            }
        }

    }

    //���new[q] ���� ���һ���ڵ㣬��ônew[q]��β���������������� REPLACE
    for (; q < new_ai_iijma.size(); ++q)
    {
        //new[q] �� new[r] ���������ģ�REPLACE
        sql_replace_one(table_id,
                        new_ai_iijma[q].index_1_,
                        new_ai_iijma[q].index_2_,
                        new_ai_iijma[q].ai_data_length_,
                        new_ai_iijma[q].ai_iijima_data_,
                        new_ai_iijma[q].last_mod_time_);
        update_sql += sql_string_;
    }

    return 0;
}

#endif //SQLITE_VERSION_NUMBER >= 3005000



