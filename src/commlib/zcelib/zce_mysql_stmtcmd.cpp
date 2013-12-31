#include "zce_predefine.h"
#include "zce_mysql_stmtcmd.h"
#include "zce_mysql_stmtbind.h"
#include "zce_mysql_result.h"

//��Щ��������4.1.2��İ汾����
#if MYSQL_VERSION_ID >= 40100

ZCE_Mysql_STMT_Command::ZCE_Mysql_STMT_Command():
    mysql_connect_(NULL),
    mysql_stmt_(NULL),
    is_bind_result_(false)
{
    //����INITBUFSIZE�Ŀռ�
    stmt_command_.reserve(INITBUFSIZE);
}

//ָ��һ��connect
ZCE_Mysql_STMT_Command::ZCE_Mysql_STMT_Command(ZCE_Mysql_Connect *conn ):
    mysql_connect_(NULL),
    mysql_stmt_(NULL),
    is_bind_result_(false)
{

    //assert(conn != NULL);
    if (conn != NULL && conn->is_connected())
    {
        mysql_connect_ = conn;
        //����
        mysql_stmt_ = mysql_stmt_init(mysql_connect_->get_mysql_handle());
    }

    //����INITBUFSIZE�Ŀռ�
    stmt_command_.reserve(INITBUFSIZE);
}

ZCE_Mysql_STMT_Command::~ZCE_Mysql_STMT_Command()
{
    if ( NULL  != mysql_stmt_)
    {
        int tmpret = mysql_stmt_free_result(mysql_stmt_);
        tmpret = mysql_stmt_close(mysql_stmt_);

    }
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��10��17��
Function        : ZCE_Mysql_STMT_Command::set_connection
Return          : int
Parameter List  :
  Param1: ZCE_Mysql_Connect* conn ���õ�����
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int ZCE_Mysql_STMT_Command::set_connection(ZCE_Mysql_Connect *conn)
{
    //������
    if (conn != NULL && conn->is_connected())
    {
        mysql_connect_ = conn;

        if (mysql_stmt_ != NULL)
        {
            int tmpret = mysql_stmt_close(mysql_stmt_);
            MYSQLSTMTCMDCHECK(tmpret == 0);
        }

        mysql_stmt_ = mysql_stmt_init(mysql_connect_->get_mysql_handle());

        return MYSQL_RETURN_FAIL;
    }

    //
    return MYSQL_RETURN_OK;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��10��17��
Function        : ZCE_Mysql_STMT_Command::stmt_prepare_bind
Return          : int
Parameter List  :
  Param1: ZCE_Mysql_STMT_Bind* bindparam  �󶨵Ĳ���
  Param2: ZCE_Mysql_STMT_Bind* bindresult �󶨵Ľ��
Description     : ׼��SQL,���ҷ����󶨵ı���
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int ZCE_Mysql_STMT_Command::stmt_prepare_bind(ZCE_Mysql_STMT_Bind *bindparam, ZCE_Mysql_STMT_Bind *bindresult)
{

    int tmpret = mysql_stmt_prepare(mysql_stmt_, stmt_command_.c_str(), static_cast<unsigned long>(stmt_command_.size()));
    MYSQLSTMTCMDCHECK(tmpret == 0);

    is_bind_result_ = false;

    //ԭ���������İ󶨱�������,��������������MySQL�Լ����,
    //unsigned long paramcount = mysql_stmt_param_count(mysql_stmt_);
    //ZASSERT(paramcount > 0 &&  bindparam!=NULL || paramcount == 0 && bindparam == NULL);

    //�󶨵Ĳ���
    if ( bindparam != NULL)
    {
        tmpret = mysql_stmt_bind_param(mysql_stmt_, bindparam->get_stmt_bind_handle());
        MYSQLSTMTCMDCHECK(tmpret == 0);
    }

    //�󶨵Ľ��
    if (bindresult)
    {
        tmpret = mysql_stmt_bind_result(mysql_stmt_, bindresult->get_stmt_bind_handle());
        //������,���ߴ���
        MYSQLSTMTCMDCHECK(tmpret == 0);

        is_bind_result_ = true;
    }

    return MYSQL_RETURN_OK;
}

//����SQL Command���,ΪBIN�͵�SQL���׼��,ͬʱ�󶨲���,���
int ZCE_Mysql_STMT_Command::set_stmt_command(const std::string &sqlcmd, ZCE_Mysql_STMT_Bind *bindparam, ZCE_Mysql_STMT_Bind *bindresult)
{
    stmt_command_ = sqlcmd;
    return stmt_prepare_bind(bindparam, bindresult);
}

//����SQL Command���,ΪBIN�͵�SQL���׼��,����Ҫ�ﶨ������SQL,���
int ZCE_Mysql_STMT_Command::set_stmt_command(const char *sqlcmd, size_t szsql, ZCE_Mysql_STMT_Bind *bindparam, ZCE_Mysql_STMT_Bind *bindresult)
{
    ZCE_ASSERT(sqlcmd != NULL);
    //
    stmt_command_.assign(sqlcmd, szsql);

    return stmt_prepare_bind(bindparam, bindresult);
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��10��17��
Function        : ZCE_Mysql_STMT_Command::execute
Return          : int == MYSQL_RETURN_OK 0,��ʾ�ɹ�
Parameter List  :
  Param1: unsigned int* num_affect    ���ص�Ӱ���¼����
  Param2: unsigned int* lastid       ���ص�LASTID
  Param3: ZCE_Mysql_STMT_Bind* bindresult  �󶨵ķ��ؽ��,�����в�ѯ��������
Description     : ִ��STMT SQL���,��������,�ô�ҹ�ͬ���õĻ�������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int ZCE_Mysql_STMT_Command::execute(unsigned int *num_affect , unsigned int *lastid)
{
    int tmpret = 0;

    //ִ��
    tmpret = mysql_stmt_execute(mysql_stmt_);

    //�������ִ�н��,���ش��󣬻����׳��쳣
    MYSQLSTMTCMDCHECK(tmpret == 0);

    //���Ҫ���ؽ��,����ת��
    if (is_bind_result_)
    {
        tmpret = mysql_stmt_store_result(mysql_stmt_);
        //������,���ߴ���
        MYSQLSTMTCMDCHECK(tmpret == 0);
    }

    //ִ��SQL����Ӱ���˶�����,mysql_affected_rows ������ת���������,������Ҫע������Ĳ���
    if (num_affect)
    {
        *num_affect = (unsigned int) mysql_stmt_affected_rows(mysql_stmt_);
    }

    if (lastid)
    {
        *lastid = (unsigned int) mysql_stmt_insert_id(mysql_stmt_);
    }

    //�ɹ�
    return MYSQL_RETURN_OK;
}

//ִ��SQL���,�������������ϵ�����
int ZCE_Mysql_STMT_Command::execute(unsigned int &num_affect, unsigned int &lastid)
{
    return execute(&num_affect , &lastid);
}

//ִ��SQL���,SELECT���,ת��������ϵ�����,
int ZCE_Mysql_STMT_Command::execute(unsigned int &num_affect)
{
    return execute(&num_affect, NULL);
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��10��17��
Function        : ZCE_Mysql_STMT_Command::fetch_row_next
Return          : int
Parameter List  : NULL
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int ZCE_Mysql_STMT_Command::fetch_row_next() const
{
    int tmpret = mysql_stmt_fetch(mysql_stmt_);

    if (tmpret != 0)
    {
        return MYSQL_RETURN_FAIL;
    }

    return MYSQL_RETURN_OK;

}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��10��17��
Function        : ZCE_Mysql_STMT_Command::fetch_column
Return          : int
Parameter List  :
  Param1: MYSQL_BIND* bind
  Param2: unsigned int column
  Param3: unsigned int offset
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int  ZCE_Mysql_STMT_Command::fetch_column(MYSQL_BIND *bind, unsigned int column, unsigned int offset) const
{
    int tmpret = mysql_stmt_fetch_column(mysql_stmt_, bind, column, offset);

    if (tmpret != 0)
    {
        return MYSQL_RETURN_FAIL;
    }

    return MYSQL_RETURN_OK;
}

//
int ZCE_Mysql_STMT_Command::SeekResultRow(unsigned int nrow) const
{
    //���������Ϊ��,���߲���row����
    mysql_stmt_data_seek(mysql_stmt_, nrow);
    int tmpret  = mysql_stmt_fetch(mysql_stmt_);

    MYSQLSTMTCMDCHECK(tmpret == 0);

    return MYSQL_RETURN_OK;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��10��17��
Function        : ZCE_Mysql_STMT_Command::ParamToMetadata
Return          : void
Parameter List  :
  Param1: ZCE_Mysql_Result* tmpres
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void ZCE_Mysql_STMT_Command::ParamToMetadata(ZCE_Mysql_Result *tmpres) const
{
    MYSQL_RES *myres =  mysql_stmt_param_metadata(mysql_stmt_);
    tmpres->set_mysql_result(myres);
    return ;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��10��17��
Function        : ZCE_Mysql_STMT_Command::ResultToMetadata
Return          : void
Parameter List  :
  Param1: ZCE_Mysql_Result* tmpres
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void ZCE_Mysql_STMT_Command::ResultToMetadata(ZCE_Mysql_Result *tmpres) const
{
    MYSQL_RES *myres =  mysql_stmt_result_metadata(mysql_stmt_);
    tmpres->set_mysql_result(myres);
    return ;
}

#endif //MYSQL_VERSION_ID >= 40100

