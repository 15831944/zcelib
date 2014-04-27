
#include "zce_predefine.h"
#include "zce_mysql_command.h"

//�����Ҫ��MYSQL�Ŀ�
#if defined MYSQL_VERSION_ID

/*********************************************************************************
class ZCE_Mysql_Command Database mysql_result_ implementation
*********************************************************************************/
ZCE_Mysql_Command::ZCE_Mysql_Command():
    mysql_connect_(NULL)
{
    //����INITBUFSIZE�Ŀռ�
    mysql_command_.reserve(INITBUFSIZE);
}

ZCE_Mysql_Command::ZCE_Mysql_Command(ZCE_Mysql_Connect *conn):
    mysql_connect_(NULL)
{
    //assert(conn != NULL);
    if (conn != NULL && conn->is_connected())
    {
        mysql_connect_ = conn;
    }

    //����INITBUFSIZE�Ŀռ�
    mysql_command_.reserve(INITBUFSIZE);
}

ZCE_Mysql_Command::~ZCE_Mysql_Command()
{
}

//ΪZCE_Mysql_Command������ص����Ӷ��󣬶����Ǳ����Ѿ��ɹ����������ݵ�
int ZCE_Mysql_Command::set_connection(ZCE_Mysql_Connect *conn)
{
    if (conn != NULL && conn->is_connected())
    {
        mysql_connect_ = conn;
        return 0;
    }

    return -1;
}

///����SQL Command���,��̬�����汾
int ZCE_Mysql_Command::set_sql_command(const char *sql_format, ...)
{
    char  tmpBuf[INITBUFSIZE];
    va_list args;
    va_start(args, sql_format);

    //_vsnprintf����ANSI C��׼����,���Ǵ󲿷ֺ�����Ӧ��ʵ������,�Ͼ�vsprintfȱ�������İ�ȫ��
    int ret = vsnprintf(tmpBuf, INITBUFSIZE, sql_format, args);

    va_end(args);

    //������ؽ������,_vsnprintf =-1��ʾBuf���Ȳ���
    if (ret < 0)
    {
        return -1;
    }

    //����
    mysql_command_.assign(tmpBuf);

    //�ɹ�
    return 0;
}

//�õ�SQL Command. ���ض�����ʽ,�����ı�����
const char *ZCE_Mysql_Command::get_sql_command() const
{
    return mysql_command_.c_str();
}

// �õ�SQL ���. ��������,�����char buf�����Ƿ��㹻�Լ���֤
int ZCE_Mysql_Command::get_sql_command( char *cmdbuf, size_t &szbuf) const
{
    if (cmdbuf == NULL )
    {
        ZCE_ASSERT(false);
        return -1;
    }

    size_t size_sql = mysql_command_.length();

    if (size_sql + 1 > szbuf)
    {
        return -1;
    }

    szbuf = size_sql;
    memcpy(cmdbuf, mysql_command_.c_str(), szbuf);
    return 0;
}

//
void ZCE_Mysql_Command::get_sql_command(std::string &strcmd) const
{
    //Ԥ�ȷ����ڴ�,��֤Ч��
    strcmd.reserve(mysql_command_.length());
    strcmd = mysql_command_;
}

//int �����Ƿ�ɹ�����ʧ�� MYSQL_RETURN_FAIL��ʾʧ��
//ִ��SQL��䣬����ȫ����������ʹ��
int ZCE_Mysql_Command::execute(unsigned int *num_affect , unsigned int *lastid, ZCE_Mysql_Result *sqlresult, bool bstore)
{
    //���û���������ӻ���û����������
    if (mysql_connect_ == NULL || mysql_command_.empty())
    {
        return -1;
    }

    //ִ��SQL����
    int tmpret = ::mysql_real_query(mysql_connect_->get_mysql_handle(), 
        mysql_command_.c_str(), 
        (unsigned long)mysql_command_.length());
    if (tmpret != 0)
    {
        return tmpret;
    }

    //����û�Ҫ��ת�������
    if (sqlresult)
    {
        MYSQL_RES *tmp_res = NULL;

        if (bstore)
        {
            //ת�����
            tmp_res = ::mysql_store_result(mysql_connect_->get_mysql_handle());
        }
        else
        {
            //ת�����
            tmp_res = ::mysql_use_result(mysql_connect_->get_mysql_handle());
        }

        //��������INSERT��䵫��,��Ҫȡ�ؽ����,����ʱ��Ϊ���ǶԵ�,ֻ�Ƿ��صĽ����Ϊ�ջ����㲻��ע��
        //���ת��ʧ��,Ϊʲô������,��MySQL�ĵ�"Ϊʲô��mysql_query()���سɹ���mysql_store_result()��ʱ����NULL? "
        //�����INSERT��䣬��ômysql_store_result���Ƿ���NULL��mysql_field_countҲӦ�õ���0��
        //���MYSQL�ڲ�����ĳ��������ômysql_store_result ����NULL����mysql_field_count �����0����ʱ�Ǹ�����
        if ( tmp_res == NULL && mysql_field_count(mysql_connect_->get_mysql_handle()) > 0)
        {
            return -1;
        }

        //�õ������,��ѯ�������Ϣ
        sqlresult->set_mysql_result(tmp_res);
    }

    //ִ��SQL����Ӱ���˶�����,mysql_affected_rows ������ת���������,������Ҫע������Ĳ���
    if (num_affect)
    {
        *num_affect = (unsigned int) ::mysql_affected_rows(mysql_connect_->get_mysql_handle());
    }

    if (lastid)
    {
        *lastid = (unsigned int) ::mysql_insert_id(mysql_connect_->get_mysql_handle());
    }

    //�ɹ�
    return 0;
}

//ִ��SQL���,�������������ϵ�����,��SELECT���
//num_affect Ϊ���ز���,�������޸��˼���
int ZCE_Mysql_Command::execute(unsigned int &num_affect, unsigned int &lastid)
{
    return execute(&num_affect, &lastid, NULL, false);
}

//ִ��SQL���,SELECT���,ת��������ϵ�����,ע������������õ���mysql_store_result.
//num_affect Ϊ���ز���,�������޸��˼���,SELECT�˼���
int ZCE_Mysql_Command::execute(unsigned int &num_affect, ZCE_Mysql_Result &sqlresult)
{
    return execute(&num_affect, NULL, &sqlresult, true);
}

//ִ��SQL���,SELECT���,USE������ϵ�����,ע������õ���mysql_use_result,num_affect������Ч
//���ڽ����̫��Ĵ���,���һ��ת���������ռ��̫���ڴ�Ĵ���,���Կ�������,
//�����Ƽ�ʹ��,һ��ȡһ��,����̫��
int ZCE_Mysql_Command::execute(ZCE_Mysql_Result &sqlresult)
{
    return execute(NULL, NULL, &sqlresult, false);
}

//<< ��������,������SQL Command ���������
ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const short tmpshort)
{
    const size_t TEMP_BUFFER_LEN = 64;
    char tmpbuf[TEMP_BUFFER_LEN + 1];
    snprintf(tmpbuf, TEMP_BUFFER_LEN, "%hd", tmpshort);

    mysql_command_.append(tmpbuf);
    return *this;
}

ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const long tmplong)
{
    const size_t TEMP_BUFFER_LEN = 64;
    char tmpbuf[TEMP_BUFFER_LEN + 1];
    snprintf(tmpbuf, TEMP_BUFFER_LEN, "%ld", tmplong);

    mysql_command_.append(tmpbuf);
    return *this;
}

ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const long long tmplonglong)
{
    const size_t TEMP_BUFFER_LEN = 64;
    char tmpbuf[TEMP_BUFFER_LEN + 1];
    snprintf(tmpbuf, TEMP_BUFFER_LEN, "%lld", tmplonglong);

    mysql_command_.append(tmpbuf);
    return *this;
}

ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const unsigned short ushort)
{
    const size_t TEMP_BUFFER_LEN = 64;
    char tmpbuf[TEMP_BUFFER_LEN + 1];
    snprintf(tmpbuf, TEMP_BUFFER_LEN, "%hu", ushort);

    mysql_command_.append(tmpbuf);
    return *this;
}

ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const unsigned long ulong)
{
    const size_t TEMP_BUFFER_LEN = 64;
    char tmpbuf[TEMP_BUFFER_LEN + 1];
    snprintf(tmpbuf, TEMP_BUFFER_LEN, "%lu", ulong);

    mysql_command_.append(tmpbuf);
    return *this;
}

ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const unsigned long long ulonglong)
{
    const size_t TEMP_BUFFER_LEN = 64;
    char tmpbuf[TEMP_BUFFER_LEN + 1];
    snprintf(tmpbuf, TEMP_BUFFER_LEN, "%llu", ulonglong);

    mysql_command_.append(tmpbuf);
    return *this;
}

ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const float tmpfloat)
{
    const size_t TEMP_BUFFER_LEN = 64;
    char tmpbuf[TEMP_BUFFER_LEN + 1];
    snprintf(tmpbuf, TEMP_BUFFER_LEN, "%f", tmpfloat);

    mysql_command_.append(tmpbuf);
    return *this;
}

ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const double tmpdouble)
{
    const size_t TEMP_BUFFER_LEN = 64;
    char tmpbuf[TEMP_BUFFER_LEN + 1];
    snprintf(tmpbuf, TEMP_BUFFER_LEN, "%f", tmpdouble);

    mysql_command_.append(tmpbuf);
    return *this;
}

ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const char *tmpstr)
{
    mysql_command_.append(tmpstr);
    return *this;
}

ZCE_Mysql_Command &ZCE_Mysql_Command::operator << (const std::string &tmpstr)
{
    mysql_command_.append(tmpstr);
    return *this;
}

#if MYSQL_VERSION_ID > 40100

//���� multiple-statement executions �еõ����
//���
int ZCE_Mysql_Command::fetch_next_multi_result(ZCE_Mysql_Result &sqlresult, bool bstore)
{

    int tmpret = ::mysql_next_result(mysql_connect_->get_mysql_handle());

    //tmpret == -1��ʾû�н����,����<0��ֵ��ʾ����
    if (tmpret < 0 )
    {
        return -1;
    }

    MYSQL_RES *tmp_res = NULL;

    if (bstore)
    {
        //ת�����
        tmp_res = ::mysql_store_result(mysql_connect_->get_mysql_handle());
    }
    else
    {
        //ת�����
        tmp_res = ::mysql_use_result(mysql_connect_->get_mysql_handle());
    }

    //��������INSERT��䵫��,��Ҫȡ�ؽ����,����ʱ��Ϊ���ǶԵ�,ֻ�Ƿ��صĽ����Ϊ�ջ����㲻��ע��
    //���ת��ʧ��,Ϊʲô������,��MySQL�ĵ�"Ϊʲô��mysql_query()���سɹ���mysql_store_result()��ʱ����NULL? "
    if ( tmp_res == NULL && ::mysql_field_count(mysql_connect_->get_mysql_handle()) > 0)
    {
        return -1;
    }

    //�õ������,��ѯ�������Ϣ
    sqlresult.set_mysql_result(tmp_res);

    //�ɹ�
    return 0;
}

#endif //MYSQL_VERSION_ID > 40100

//�����Ҫ��MYSQL�Ŀ�
#endif //#if defined MYSQL_VERSION_ID

