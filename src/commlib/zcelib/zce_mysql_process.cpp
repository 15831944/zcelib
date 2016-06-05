
#include "zce_predefine.h"
#include "zce_trace_debugging.h"
#include "zce_mysql_process.h"

//�����Ҫ��MYSQL�Ŀ�
#if defined ZCE_USE_MYSQL

//-----------------------------------------------------------------
//ʵ��,Ϊ��SingleTon��׼��
ZCE_Mysql_Process *ZCE_Mysql_Process::instance_ = NULL;

//���캯��������������
ZCE_Mysql_Process::ZCE_Mysql_Process():
    db_port_(MYSQL_PORT)
{

}
ZCE_Mysql_Process::~ZCE_Mysql_Process()
{
    //���ô���ʲô����صĳ�Ա�����������������˴���
}

//��ʼ��������,ʹ��hostname��������,���Բ��������Ӻ��������ӣ����Լ����ơ�
int ZCE_Mysql_Process::init_mysql_server(const char *host_name,
                                         const char *user,
                                         const char *pwd,
                                         unsigned int port,
                                         bool connect_atonce)
{

    db_hostname_ = host_name;
    db_user_name_ = user;
    db_password_ = pwd;
    db_port_ = port;

    if (connect_atonce)
    {
        return connect_mysql_server();
    }

    return 0;
}

//��ʼ��MYSQL��ʹ��UNIX socket file����(UNIX��)�����������ܵ�(Windows��),ֻ��ʼ��Ҳ������,ֻ�����ڱ���
int ZCE_Mysql_Process::init_mysql_socketfile(const char *socket_file,
                                             const char *user,
                                             const char *pwd,
                                             bool connect_atonce)
{
    db_socket_file_ = socket_file;
    db_user_name_ = user;
    db_password_ = pwd;

    if (connect_atonce)
    {
        return connect_mysql_server();
    }

    return 0;
}

//
int ZCE_Mysql_Process::connect_mysql_server( )
{
    int ret = 0;

    //�������ݿ�
    if (db_connect_.is_connected() == false)
    {

        //������ù�HOST����HOST NAME��������
        if (db_hostname_.length() > 0)
        {
            ret = db_connect_.connect_by_host(db_hostname_.c_str(),
                                              db_user_name_.c_str(),
                                              db_password_.c_str(),
                                              NULL,
                                              db_port_);
        }
        else if (db_socket_file_.length() > 0)
        {
            ret = db_connect_.connect_by_socketfile(db_socket_file_.c_str(),
                                                    db_user_name_.c_str(),
                                                    db_password_.c_str());
        }
        else
        {
            ZCE_ASSERT(false);
        }

        //�������
        if (ret != 0)
        {
            ZCE_LOG(RS_ERROR, "[zcelib] DB Error : [%u]:%s.",
                    db_connect_.get_error_no(),
                    db_connect_.get_error_message()
                   );
            return -1;
        }

        //
        db_command_.set_connection(&db_connect_);
    }

    return  0;
}

//�Ͽ�����
void ZCE_Mysql_Process::disconnect_mysql_server()
{

    if (db_connect_.is_connected() == true)
    {
        db_connect_.disconnect();
    }

}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2004��9��22��
Function        : ZCE_Mysql_Process::db_process_query
Return          : int
Parameter List  :
Param1: const char* sql SQL���
Param2: unsigned int& numaffect ���ص��յ�Ӱ��ļ�¼����
Param3: unsigned int& insertid  ���صĲ����LAST_INSERT_ID
Param4: size_t sqllen           SQL��䳤��,����BIN��SQL���,Ĭ��Ϊ0,��ʾʹ��STR��SQL
Description     : ִ�м����SQL���,���ڷ�SELECT���(INSERT,UPDATE)
Calls           :
Called By       :
Other           : ע��:����db_process_query�����������ھ��Ƕ�����������,ZCE_Mysql_Connect����������ʱ������
Modify Record   : �Զ��������ܼ���
******************************************************************************************/
int ZCE_Mysql_Process::db_process_query(const char *sql,
                                        unsigned int &numaffect,
                                        unsigned int &insertid,
                                        size_t sqllen)
{
    int ret = 0;

    //�������ݿ�
    if (db_connect_.is_connected() == false)
    {
        connect_mysql_server();
    }
    //����Ѿ����ӹ����ݿ�,�����ٴ�����,pingһ�ξ�OK��,�ɱ���
    else
    {
        db_connect_.ping();
    }

    //TEXT���͵�SQL
    if (sqllen == 0)
    {
        ZCE_LOGMSG_DBG(RS_DEBUG, "[db_process_query]SQL:[%s].", sql);
        db_command_.set_sql_command(sql, strlen(sql));
    }
    //BIN���͵�SQL
    else
    {
        ZCE_LOGMSG_DBG(RS_DEBUG, "[db_process_query]SQL:[%.*s].", sqllen, sql);
        db_command_.set_sql_command(sql, sqllen);
    }

    ret = db_command_.execute(numaffect, insertid);

    //�������
    if (ret != 0)
    {
        ZCE_LOG(RS_ERROR, "[zcelib] DB Error:[%u]:[%s]. SQL:%s",
                db_connect_.get_error_no(),
                db_connect_.get_error_message(),
                sql);
        return -1;
    }

    //�ɹ�
    return  0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2004��9��22��
Function        : ZCE_Mysql_Process::db_process_query
Return          : int
Parameter List  :
Param1: const char* sql         SQL���
Param2: unsigned int& numaffect ���ز���,���صĲ�ѯ�ļ�¼����
Param3: ZCE_Mysql_Result& dbresult ���ز���,��ѯ�Ľ������
Param4: size_t sqllen           SQL��䳤��,����BIN��SQL���,Ĭ��Ϊ0,��ʾʹ��STR��SQL
Description     : ִ�м����SQL���,����SELECT���,ֱ��ת��������ϵķ���
Calls           :
Called By       :
Other           : ע��:����db_process_query�������������ھ��Ƕ�����������,ZCE_Mysql_Connect����������ʱ������
Modify Record   :
******************************************************************************************/
int ZCE_Mysql_Process::db_process_query(const char *sql, unsigned int &numaffect, ZCE_Mysql_Result &dbresult, size_t sqllen)
{
    int ret = 0;

    //�������ݿ�
    if (db_connect_.is_connected() == false)
    {
        connect_mysql_server();
    }
    //����Ѿ����ӹ����ݿ�,�����ٴ�����,pingһ�ξ�OK��,�ɱ���
    else
    {
        db_connect_.ping();
    }

    //TEXT���͵�SQL
    if (sqllen == 0)
    {
        ZCE_LOGMSG_DBG(RS_DEBUG, "[db_process_query]SQL:[%s]", sql);
        db_command_.set_sql_command(sql, strlen(sql));
    }
    //BIN���͵�SQL
    else
    {
        ZCE_LOGMSG_DBG(RS_DEBUG, "[db_process_query]SQL:[%.*s]", sqllen, sql);
        db_command_.set_sql_command(sql, sqllen);
    }

    ret = db_command_.execute(numaffect, dbresult);

    //�������
    if (ret != 0)
    {
        ZCE_LOG(RS_ERROR, "[zcelib] DB Error:[%u]:[%s]. SQL:%s.",
                db_connect_.get_error_no(),
                db_connect_.get_error_message(),
                sql);
        return -1;
    }

    //�ɹ�
    return  0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2004��9��22��
Function        : ZCE_Mysql_Process::db_process_query
Return          : int
Parameter List  :
Param1: const char*             SQL���
Param2: ZCE_Mysql_Result& dbresult ���صĽ�����
Param3: size_t sqllen SQL       ��䳤��
Description     : ����SELECT���,����use_result�õ�������ϵķ���
Calls           :
Called By       :
Other           : ���ڽ����̫��,��ռ��̫���ڴ�ĵĴ���,���Ƽ�ʹ��,
Modify Record   :
******************************************************************************************/
int ZCE_Mysql_Process::db_process_query(const char *sql, ZCE_Mysql_Result &dbresult, size_t sqllen)
{
    int ret = 0;

    //�������ݿ�
    if (db_connect_.is_connected() == false)
    {
        connect_mysql_server();
    }
    //����Ѿ����ӹ����ݿ�,�����ٴ�����,pingһ�ξ�OK��,�ɱ���
    else
    {
        db_connect_.ping();
    }

    //TEXT���͵�SQL
    if (sqllen == 0)
    {
        db_command_.set_sql_command(sql, strlen(sql));
    }
    //BIN���͵�SQL
    else
    {
        db_command_.set_sql_command(sql, sqllen);
    }

    ret = db_command_.execute(dbresult);

    //�������
    if (ret != 0)
    {
        ZCE_LOG(RS_ERROR, "[zcelib] DB Error:[%u]:[%s]. SQL:%s",
                db_connect_.get_error_no(),
                db_connect_.get_error_message(),
                sql);
        return -1;
    }

    //�ɹ�
    return  0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��3��9��
Function        : ZCE_Mysql_Process::get_return_error
Return          : unsigned int ���صĴ���Number
Parameter List  :
Param1: char* szerr   ���صĴ�����Ϣ
Param2: size_t buflen Buf�ĳ���
Description     : ���صĵ�DB���ʵĴ�����Ϣ
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
unsigned int ZCE_Mysql_Process::get_return_error(char *szerr, size_t buflen)
{
    snprintf(szerr, buflen, "[%d]:%s ", db_connect_.get_error_no(), db_connect_.get_error_message());
    return db_connect_.get_error_no();
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��8��18��
Function        : ZCE_Mysql_Process::get_query_sql
Return          : static const char*
Parameter List  :
Param1: void
Description     : �õ�DB���ʵ����
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
const char *ZCE_Mysql_Process::get_query_sql(void)
{
    return db_command_.get_sql_command();
}

//�õ�������Ϣ���
const char *ZCE_Mysql_Process::get_return_error_str()
{
    return db_connect_.get_error_message();
}

//�õ�������ϢID
unsigned int ZCE_Mysql_Process::get_return_error_id()
{
    return db_connect_.get_error_no();
}

//�õ�Real Escape String ,Real��ʾ���ݵ�ǰ��MYSQL Connet���ַ���,�õ�Escape String
//Escape String Ϊ���ַ����е�����ַ�����ת�������,����',",\���ַ�
unsigned int ZCE_Mysql_Process::make_real_escape_string(char *tostr,
                                                        const char *fromstr,
                                                        unsigned int fromlen)
{
    return mysql_real_escape_string(db_connect_.get_mysql_handle(), tostr, fromstr, fromlen);
}

//-----------------------------------------------------------------
//ʵ��,Ϊ��SingleTon��׼��

//ʵ����ֵ
void ZCE_Mysql_Process::instance(ZCE_Mysql_Process *instance)
{
    clean_instance();
    instance_ = instance;
}
//���ʵ��
ZCE_Mysql_Process *ZCE_Mysql_Process::instance()
{
    if (instance_)
    {
        delete instance_ ;
        instance_ = NULL;
    }

    instance_ = new ZCE_Mysql_Process();
    return  instance_;

}

//���ʵ��
void ZCE_Mysql_Process::clean_instance()
{
    if (instance_)
    {
        delete instance_ ;
        instance_ = NULL;
    }
}

//�����Ҫ��MYSQL�Ŀ�
#endif //#if defined ZCE_USE_MYSQL

