#include "zce_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_mysql_connect.h"

//�����Ҫ��MYSQL�Ŀ�
#if defined MYSQL_VERSION_ID

/*********************************************************************************
class ZCE_Mysql_Connect
*********************************************************************************/

ZCE_Mysql_Connect::ZCE_Mysql_Connect()
{
    mysql_init(&mysql_handle_);
    //�ÿ�ʼ״̬
    if_connected_ = false;
}

ZCE_Mysql_Connect::~ZCE_Mysql_Connect()
{
    // disconnect if if_connected_ to ZCE_Mysql_Connect
    disconnect();
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2004��8��15��
Function        : ZCE_Mysql_Connect::connect_by_optionfile
Return          : int
Parameter List  :
  Param1: const char* optfile ѡ���ļ�
  Param2: const char* groupconst ѡ���ļ�Ҫ��ȡ��SECTION��������ö�Ҫ�����SECTION��,���Ϊ�գ�
                  ��ȡ[client]�µ�������
Description     : ���ʹ��ѡ���ļ���������,
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int ZCE_Mysql_Connect::connect_by_optionfile(const char *optfile, const char *group)
{
    //����Ѿ�����,�ر�ԭ��������
    if (if_connected_ == true)
    {
        disconnect();
    }

    //��ʼ��MYSQL���
    mysql_init(&mysql_handle_);

    if (optfile != NULL)
    {
        int opret = mysql_options(&mysql_handle_, MYSQL_READ_DEFAULT_FILE, optfile);

        //���ʹgroup==NULL,����дoptfile��[client]����,�����дgroup�µ�����
        if (group != NULL)
        {
            opret = mysql_options(&mysql_handle_, MYSQL_READ_DEFAULT_GROUP, group);
        }

        MYSQLCONNECTCHECK(opret == 0);
    }

    //�������ݿ�
    MYSQL *ret = mysql_real_connect(&mysql_handle_, NULL, NULL, NULL, NULL, 0, NULL, 0);

    //�����,���ش���,�����׳��쳣
    MYSQLCONNECTCHECK(ret);

    if_connected_ = true;
    //���سɹ� MYSQL_RETURN_OK=0
    return MYSQL_RETURN_OK;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2004��8��15��
Function        : ZCE_Mysql_Connect::connect
Return          : int
Parameter List  :
  Param1: const char* host ���ݿ������
  Param2: const char *socket_file,UNIX SOCKET�ļ����ƻ��������ܵ�����
  Param2: const char* user �û�,Ĭ��Ϊmysql
  Param3: const char* pwd  �û�����,Ĭ��Ϊ""
  Param4: unsigned int port �˿�,Ĭ��ΪMYSQL_PORT
  Param5: const char* db    ʹ�õ�Ĭ�����ݿ�,Ĭ��Ϊ�ձ�ʾ��ѡ��
  Param6: const const unsigned int timeout �������ݿ�ĳ�ʱʱ�䣬Ĭ��Ϊ0,��ʾ������
  Param7: bool bmultisql �Ƿ�ʹ�ö����ͬʱִ�еķ�ʽ,Ĭ��Ϊfalse,����������ȴ�������ЩЧ��
Description     : �������ݷ�������
Calls           :
Called By       :
Other           : ��������ѡ���ļ�
Modify Record   :
******************************************************************************************/
int ZCE_Mysql_Connect::connect_i(const char *host_name,
                                 const char *socket_file,
                                 const char *user,
                                 const char *pwd,
                                 const char *db,
                                 const unsigned int port,
                                 const unsigned int timeout,
                                 bool if_multi_sql)
{

    //����Ѿ�����,�ر�ԭ��������
    if (if_connected_ == true)
    {
        disconnect();
    }

    //��ʼ��MYSQL���
    mysql_init(&mysql_handle_);

    //�������ӵ�timeout
    if (timeout != 0)
    {
        mysql_options(&mysql_handle_, MYSQL_OPT_CONNECT_TIMEOUT, (char *)(&timeout));
    }

    //50013,�汾���ṩ�����ѡ���ԭ���İ汾�����ѡ����Ĭ�ϴ򿪵ġ�
#if MYSQL_VERSION_ID >= 50013
    mysql_options(&mysql_handle_, MYSQL_OPT_RECONNECT, "1");
#endif

    unsigned long client_flag = 0;

#if MYSQL_VERSION_ID > 40100

    if (if_multi_sql)
    {
        client_flag |= CLIENT_MULTI_STATEMENTS;
    }

#endif

    //�������ݿ�
    MYSQL *ret = NULL;

    //���ʹ����������IP��ַ��������
    if (host_name)
    {
        ret = mysql_real_connect(&mysql_handle_, host_name, user, pwd, db, port, NULL, client_flag);
    }
    //���ʹ��UNIXSOCKET���������ܵ����б�������
    else if (socket_file)
    {
        //����ط�����ע��һ�£�WINDOWS�£�����mysql_real_connect�������host_name����ΪNULL�����Ƚ��������ܵ����ӣ����������TCP/IP���ӱ���
        //���Ҫ����֤����ʹ�������ܵ��������host_name=".",
        ret =  mysql_real_connect(&mysql_handle_, NULL, user, pwd, db, port, socket_file, client_flag);
    }
    //����ʹ�ô��󣬲���host��unixsocket��ΪNULL
    else
    {
        ZCE_ASSERT(false);
    }

    //�����,���ش���,�����׳��쳣
    MYSQLCONNECTCHECK(ret);

    if_connected_ = true;
    //���سɹ� MYSQL_RETURN_OK=0
    return MYSQL_RETURN_OK;
}

//�������ݷ�����,ͨ��IP��ַ����������
int ZCE_Mysql_Connect::connect_by_host(const char *host_name,
                                       const char *user,
                                       const char *pwd,
                                       const char *db,
                                       const unsigned int port,
                                       unsigned int timeout,
                                       bool if_multi_sql)
{
    return connect_i(host_name, NULL, user, pwd, db, port, timeout, if_multi_sql);
}

//�������ݿ��������ͨ��UNIXSOCKET�ļ���UNIX�£����������ܵ���WINDOWS�£�����ͨ�ţ�ֻ�����ڱ���
int ZCE_Mysql_Connect::connect_by_socketfile(const char *socket_file,
                                             const char *user,
                                             const char *pwd,
                                             const char *db,
                                             unsigned int timeout,
                                             bool if_multi_sql)
{
    return connect_i(NULL, socket_file, user, pwd, db, 0, timeout, if_multi_sql);
}

//�Ͽ����ݿ����������
void ZCE_Mysql_Connect::disconnect()
{
    //û������
    if (if_connected_ == false)
    {
        return;
    }

    mysql_close(&mysql_handle_);
    if_connected_ = false;
}

//ѡ��һ��Ĭ�����ݿ�,���������ݿ������
int ZCE_Mysql_Connect::select_database(const char *db)
{
    int ret = mysql_select_db(&mysql_handle_, db);

    //�����,���ش���,�����׳��쳣
    MYSQLCONNECTCHECK(ret);

    return MYSQL_RETURN_OK;
}

//������ӶϿ����������ӣ��ͳɱ��ĺ÷���,����ʲô��������
int ZCE_Mysql_Connect::ping()
{
    int ret = mysql_ping(&mysql_handle_);

    //�����,���ش���,�����׳��쳣
    MYSQLCONNECTCHECK(ret);

    return MYSQL_RETURN_OK;
}

//�õ���ǰ���ݷ�������״̬
const char *ZCE_Mysql_Connect::get_mysql_status()
{
    return mysql_stat(&mysql_handle_);
}

//�õ�ת����Escaple String ,û�и��ݵ�ǰ���ַ����Ͻ��в���,
unsigned int ZCE_Mysql_Connect::make_escape_string(char *tostr, const char *fromstr, unsigned int fromlen)
{
    return mysql_escape_string(tostr, fromstr, fromlen);
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��3��8��
Function        : ZCE_Mysql_Connect::make_real_escape_string
Return          : unsigned int ����ĳ���
Parameter List  :
  Param1: char* tostr          : ת���õ����ַ���
  Param2: const char* fromstr  : ����ת�����ַ���
  Param3: unsigned int fromlen : ת�����ַ�������
  Description     : �õ�Real Escape String ,Real��ʾ���ݵ�ǰ��MYSQL connect���ַ���,�õ�Escape String
Calls           : Ϊʲô������������ֲ���˳��,��Ϊmysql_real_escape_string
Called By       :
Other           : Escape String Ϊ���ַ����е�����ַ�����ת�������,����',",\���ַ�
Modify Record   :
******************************************************************************************/
unsigned int ZCE_Mysql_Connect::make_real_escape_string(char *tostr, const char *fromstr, unsigned int fromlen)
{
    return mysql_real_escape_string(&mysql_handle_, tostr, fromstr, fromlen);
}

//��Щ��������4.1��İ汾����
#if MYSQL_VERSION_ID > 40100

//�����Ƿ��Զ��ύ
int ZCE_Mysql_Connect::set_auto_commit(bool bauto)
{
    //my_bool��ʵ��char
    my_bool mode =  (bauto == true ) ? 1 : 0;

    int ret = mysql_autocommit(&mysql_handle_, mode);

    //�����,���ش���,�����׳��쳣
    MYSQLCONNECTCHECK(ret);

    return MYSQL_RETURN_OK;
}

//�ύ����Commit Transaction
int ZCE_Mysql_Connect::trans_commit()
{

    int ret = mysql_commit(&mysql_handle_);

    //�����,���ش���,�����׳��쳣
    MYSQLCONNECTCHECK(ret);

    return MYSQL_RETURN_OK;
}

//�ع�����Rollback Transaction
int ZCE_Mysql_Connect::trans_rollback()
{
    int ret = mysql_rollback(&mysql_handle_);

    //�����,���ش���,�����׳��쳣
    MYSQLCONNECTCHECK(ret);

    return MYSQL_RETURN_OK;
}

#endif // MYSQL_VERSION_ID > 40100

//�����Ҫ��MYSQL�Ŀ�
#endif //#if defined MYSQL_VERSION_ID

