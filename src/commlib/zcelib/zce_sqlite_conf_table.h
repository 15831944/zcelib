
#ifndef ZCE_LIB_SQLITE_CONF_TABLE_H_
#define ZCE_LIB_SQLITE_CONF_TABLE_H_

//Ŀǰ�汾����ֻ����һ��
#if SQLITE_VERSION_NUMBER >= 3003000


/************************************************************************************************************
struct AI_IIJIMA_BINARY_DATA ���ڽ�һЩ�ṹת��Ϊ���������ݣ�����������ֶ���
����ȽϷ����㷳Ҳ���㷳�����μ��ͷ�����һ����ͣ����еĲ����ƹ�̫�谵���ұ�Ѱ��������ɫ��Сҩ��
���Ҳ�ȽϷ���д��һ����ʧ�ܵ�PPT�����������������ǰ׻��ͣ�ȴ�����˷�Ҫ���������ˣ���Ҳ�����𣿲���⣬ȥTMD��
����е�����ൽ�˼��㣬����Ҳ���ĵ��˼��㣬�������������������߰���Ķ���Խ��Խ��.
˵�˺ܶ෹�����ķϻ��������ڴ����������ɣ�һ������ì���˵ģ������а��������������������룬ȴһ���ް���
������ʢ������������������֪�����������ȴ����sina������ҳ�������й�����Ķ��ձ�һ��Ů�����
���Ƽ�����С˵������ͼʽ�԰�����С˵��һ�����淴ӳ�˺ڰ����ʱ���������һ��СŮ�����ĵĴݲУ������Ƽ���Ҷ�����
���Ҫ˵���ǣ��������ף�ϣ�������ϧ��
***********************************************************************************************************/
struct AI_IIJIMA_BINARY_DATA
{
public:

    //��̬����������������
    static const size_t MAX_LEN_OF_AI_IIJIMA_DATA = 32 * 1024;

public:

    //����������
    unsigned int           ai_data_length_;
    //��̬����ȡ
    char                   ai_iijima_data_[MAX_LEN_OF_AI_IIJIMA_DATA + 1];

public:
    //�������������
    AI_IIJIMA_BINARY_DATA();
    ~AI_IIJIMA_BINARY_DATA();
    //
    void clear();

    //��һ���ṹ���б���
    template<class T>
    int AiIijimaDataEnCode(const T &info);
    //��һ���ṹ���н���
    template<class T>
    int AiIijimaDataDeCode(T &info);

};

template<class T>
int AI_IIJIMA_BINARY_DATA::AiIijimaDataDeCode(T &info)
{

    ACE_InputCDR incdr(ai_iijima_data_, ai_data_length_);
    //����ͨ��good_bit�жϽ��
    if (false == (incdr >> info))
    {
        ACE_DEBUG((LM_ERROR, "Class %s CDR Decode Fail.\n", typeid(info).name()));
        return -1;
    }

    return -1;
}

template<class T>
int AI_IIJIMA_BINARY_DATA::AiIijimaDataEnCode(const T &info)
{
    ACE_OutputCDR outcdr(ai_iijima_data_, MAX_LEN_OF_AI_IIJIMA_DATA);
    if (false == (outcdr << info))
    {
        //��֪������д�Ƿ��Ӱ��ͻ��˵�ʹ��?
        ACE_DEBUG((LM_ERROR, "Class %s CDR Encode Fail.\n", typeid(info).name()));
        return -1;
    }

    size_t szdatalen = outcdr.total_length();

#if defined DEBUG || defined _DEBUG
    ZCE_ASSERT(szdatalen <= MAX_LEN_OF_AI_IIJIMA_DATA);
#endif

    //����̫��
    if (szdatalen > MAX_LEN_OF_AI_IIJIMA_DATA)
    {
        return -1;
    }
    //����FRAME�ĳ���,
    ai_data_length_ = static_cast<unsigned int>(szdatalen);
    if (ai_data_length_ > MAX_LEN_OF_AI_IIJIMA_DATA / 2)
    {
        return -1;
    }

    return 0;
}


typedef std::vector <AI_IIJIMA_BINARY_DATA>   ARRARY_OF_AI_IIJIMA_BINARY;



/******************************************************************************************
struct BaobaoGeneralDBConf һ����ͨ�õĴ�DB�м�õ�ͨ��������Ϣ�ķ���
******************************************************************************************/
//һ����ͨ�õĴ�DB�м�õ�ͨ��������Ϣ�Ľṹ


class General_SQLite_Config
{


public:

    //��д��SQL
    static void ReplaceSQLString(unsigned int table_id,
                                 unsigned int conf_id_1,
                                 unsigned int conf_id_2,
                                 const AI_IIJIMA_BINARY_DATA &conf_data_1,
                                 const AI_IIJIMA_BINARY_DATA &conf_data_2,
                                 unsigned int last_mod_time,
                                 char *sql_string,
                                 size_t &buflen);

    //�õ�ѡ��һ��ȷ�����ݵ�SQL
    static void SelectSQLString(unsigned int table_id,
                                unsigned int conf_id_1,
                                unsigned int conf_id_2,
                                char *sql_string,
                                size_t &buflen);

    //�õ�ɾ�����ݵ�SQL
    static void DeleteSQLString(unsigned int table_id,
                                unsigned int conf_id_1,
                                unsigned int conf_id_2,
                                char *sql_string,
                                size_t &buflen);


    //
    static void GetCountSQLString(unsigned int table_id,
                                  char *sql_string,
                                  size_t &buflen);

    //
    static void GetArraySQLString(unsigned int table_id,
                                  unsigned int startno,
                                  unsigned int numquery,
                                  char *sql_string,
                                  size_t &buflen);

public:





};


#endif //SQLITE_VERSION_NUMBER >= 3003000

#endif //ZCE_LIB_SQLITE_CONF_TABLE_H_
