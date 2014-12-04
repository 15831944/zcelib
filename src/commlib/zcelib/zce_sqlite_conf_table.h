/*!
* @copyright  2004-2014  Apache License, Version 2.0 FULLSAIL
* @filename   zce_sqlite_conf_table.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       Thursday, November 27, 2009
* @brief
*
*
* @details
*
*
*
* @note       AI_IIJIMA ������
*             ����ȽϷ����㷳Ҳ���㷳�����μ��ͷ�����һ����ͣ����еĲ����ƹ�̫�谵���ұ�Ѱ��������ɫ��Сҩ��
*             ����е�����ൽ�˼��㣬����Ҳ���ĵ��˼��㣬�������������������߰���Ķ���Խ��Խ��.
*             ˵�˺ܶ෹�����ķϻ��������ڴ����������ɣ�һ������ì���˵ģ������а��������������������룬ȴһ���ް���
*             ������ʢ������������������֪�����������ȴ����sina������ҳ�������й�����Ķ��ձ�һ��Ů�����
*             ���Ƽ�����С˵������ͼʽ�԰�����С˵��һ�����淴ӳ�˺ڰ����ʱ���������һ��СŮ�����ĵĴݲУ������Ƽ���Ҷ�����
*             ���Ҫ˵���ǣ��������ף�ϣ�������ϧ��
*             �����Ƕΰ�һ����Ʒ����3�����ʷ��
*/


/*
�����������壬
CREATE TABLE IF NOT EXISTS config_table_1(index_1 INTEGER,index_2 INTEGER,conf_data BLOB ,last_mod_time INTEGER);
CREATE UNIQUE INDEX IF NOT EXISTS cfg_table_idx_1 ON config_table_1 (index_1,index_2)

*/

#ifndef ZCE_LIB_SQLITE_CONF_TABLE_H_
#define ZCE_LIB_SQLITE_CONF_TABLE_H_

//Ŀǰ�汾����ֻ����һ��
#if SQLITE_VERSION_NUMBER >= 3005000



/*!
* @brief      ���ڽ�һЩ�ṹת��Ϊ���������ݣ�����������ֶ���
*
* @note
*/
struct ZCELIB_EXPORT AI_IIJIMA_BINARY_DATA
{


public:

    ///�������������
    AI_IIJIMA_BINARY_DATA();
    ~AI_IIJIMA_BINARY_DATA();

    ///
    void clear();

    ///�ȽϺ���
    bool operator < (const AI_IIJIMA_BINARY_DATA right);

#if defined ZCE_USE_PROTOBUF && ZCE_USE_PROTOBUF == 1

    ///��һ���ṹ���б���
    template<class T>
    int protobuf_encode(unsigned int index_1,
                        unsigned int index_2,
                        const T &info);

    ///��һ���ṹ���н���
    template<class T>
    int protobuf_decode(unsigned int &index_1,
                        unsigned int &index_2,
                        T &info);

#endif

public:

    //��̬����������������
    static const int MAX_LEN_OF_AI_IIJIMA_DATA = 32 * 1024 - 1;

public:

    ///����1
    unsigned int index_1_ = 0;
    ///����2 Ĭ��Ϊ0
    unsigned int index_2_ = 0;

    ///����������
    int ai_data_length_ = 0;
    ///��̬����ȡ
    char ai_iijima_data_[MAX_LEN_OF_AI_IIJIMA_DATA];

    ///����޸�ʱ��
    unsigned int last_mod_time_ = 0;
};


#if defined ZCE_USE_PROTOBUF && ZCE_USE_PROTOBUF == 1

template<class T>
int AI_IIJIMA_BINARY_DATA::protobuf_decode(unsigned int &index_1,
                                           unsigned int &index_2,
                                           T &info)
{
    bool bret = info.ParseFromArray(ai_iijima_data_, ai_data_length_);

    if (false == bret)
    {
        ZCE_LOGMSG(RS_ERROR, "Class %s protobuf decode fail,ParseFromArray return false.\n", typeid(info).name());
        return -1;
    }

    return 0;
}

template<class T>
int AI_IIJIMA_BINARY_DATA::protobuf_encode(unsigned int index_1,
                                           unsigned int index_2,
                                           const T &info)
{

    if (info.IsInitialized())
    {
        ZCE_LOGMSG(RS_ERROR, "class %s protobuf encode fail, IsInitialized return false.\n",
                   typeid(info).name());
        return -1;
    }

    index_1_ = index_1;
    index_2_ = index_2;

    int protobuf_len = info.ByteSize();
    if (protobuf_len > MAX_LEN_OF_AI_IIJIMA_DATA)
    {
        ZCE_LOGMSG(RS_ERROR, "Config [%d|%d] class %s protobuf encode fail, ByteSize return %d >"
                   " MAX_LEN_OF_AI_IIJIMA_DATA %d.\n",
                   index_1,
                   index_2,
                   typeid(info).name(),
                   protobuf_len);
        return -1
    }

    bool bret = info.SerializeToArray(ai_iijima_data_, MAX_LEN_OF_AI_IIJIMA_DATA);
    if (bret)
    {
        ZCE_LOGMSG(RS_ERROR, "Config [%d|%d] class %s protobuf encode fail, SerializeToArray return false.\n",
                   index_1,
                   index_2,
                   typeid(info).name());
        return -1;
    }
    ai_data_length_ = protobuf_len;
    return 0;
}

#endif

typedef std::vector <AI_IIJIMA_BINARY_DATA>   ARRARY_OF_AI_IIJIMA_BINARY;



/******************************************************************************************
struct BaobaoGeneralDBConf һ����ͨ�õĴ�DB�м�õ�ͨ��������Ϣ�ķ���
******************************************************************************************/

//һ����ͨ�õĴ�DB�м�õ�ͨ��������Ϣ�Ľṹ
class ZCELIB_EXPORT ZCE_General_Config_Table
{

public:

    ZCE_General_Config_Table();
    ~ZCE_General_Config_Table();

protected:

    ///����TABLE SQL���
    void sql_create_table(unsigned  int table_id);
    ///����INDEX SQL���
    void sql_create_index(unsigned  int table_id);


    //��д��STMT SQL
    void sql_replace_bind(unsigned  int table_id,
                          unsigned int index_1,
                          unsigned int index_2,
                          unsigned int last_mod_time);

    ///��д��SQL,�ı���ʽ����x
    void sql_replace_one(unsigned  int table_id,
                         unsigned int index_1,
                         unsigned int index_2,
                         size_t blob_len,
                         const char *blob_data,
                         unsigned int last_mod_time);


    ///�õ�ѡ��һ��ȷ�����ݵ�SQL
    void sql_select_one(unsigned int table_id,
                        unsigned int index_1,
                        unsigned int index_2);

    //�õ�ɾ�����ݵ�SQL
    void sql_delete_one(unsigned int table_id,
                        unsigned int index_1,
                        unsigned int index_2);


    ///�����ѯ������
    void sql_counter(unsigned int table_id,
                     unsigned int startno,
                     unsigned int numquery);

    ///��ѯ���ݶ��У��������ݻ���ȫ������
    void sql_select_array(unsigned int table_id,
                          unsigned int startno,
                          unsigned int numquery);

public:

    ///��һ��ͨ�õ����ݿ�
    int open_dbfile(const char *db_file,
                    bool create_db);

    ///�������ݱ�
    int create_table(unsigned int table_id);

    ///
    int replace_one(unsigned int table_id,
                    const AI_IIJIMA_BINARY_DATA &conf_data);

    ///��ѯ��һ����¼
    int select_one(unsigned int table_id,
                   AI_IIJIMA_BINARY_DATA &conf_data);

    ///ɾ��һ����¼
    int delete_one(unsigned int table_id,
                   unsigned int index_1,
                   unsigned int index_2);

    ///��ѯ��¼����
    int counter(unsigned int table_id,
                unsigned int startno,
                unsigned int numquery,
                unsigned int &rec_count);

    ///��ѯ���ݶ��У��������ݣ����Ʋ�ѯ����������ȫ������
    int select_array(unsigned int table_id,
                     unsigned int startno,
                     unsigned int numquery,
                     ARRARY_OF_AI_IIJIMA_BINARY &ary_ai_iijma);


    ///�Ա��������ݱ���ҳ����죬Ȼ���ҳ������SQL
    int compare_table(const char *old_db,
                      const char *new_db,
                      unsigned int table_id,
                      std::string &update_sql);
protected:
    //
    const static size_t MAX_SQLSTRING_LEN = AI_IIJIMA_BINARY_DATA::MAX_LEN_OF_AI_IIJIMA_DATA * 2 + 1024;

public:

    ///
    char *sql_string_ = NULL;
    ///
    ZCE_SQLite_DB_Handler *sqlite_handler_;


};


#endif //SQLITE_VERSION_NUMBER >= 3005000

#endif //ZCE_LIB_SQLITE_CONF_TABLE_H_
