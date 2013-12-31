/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_mysql_stmtcmd.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2005��10��17��
* @brief
* 
* @details
* 
* @note
* 
*/

//��ƾ���������ѡ��,��ѡ����ϲ���ĺ͸о���õġ�

#ifndef ZCE_LIB_MYSQL_STMT_COMMAND_H_
#define ZCE_LIB_MYSQL_STMT_COMMAND_H_

//�����Ҫ��MYSQL�Ŀ�
#if defined MYSQL_VERSION_ID

#include "zce_mysql_predefine.h"
#include "zce_mysql_connect.h"

//STMT��������4.1.2��İ汾����
#if MYSQL_VERSION_ID >= 40100

/*********************************************************************************
class ZCE_Mysql_STMT_Command
*********************************************************************************/

class ZCE_Mysql_Connect;
class ZCE_Mysql_STMT_Bind;
class ZCE_Mysql_Result;

class ZCELIB_EXPORT ZCE_Mysql_STMT_Command
{

protected:
    //׼�����Ұ󶨲���
    int stmt_prepare_bind(ZCE_Mysql_STMT_Bind *bindparam, ZCE_Mysql_STMT_Bind *bindresult);
    //ִ��SQL����,
    int execute(unsigned int *num_affect , unsigned int *lastid);

public:
    //
    ZCE_Mysql_STMT_Command();
    //ָ��һ��connect
    ZCE_Mysql_STMT_Command(ZCE_Mysql_Connect *);
    //
    ~ZCE_Mysql_STMT_Command();

    //����Command��ZCE_Mysql_Connect
    int set_connection(ZCE_Mysql_Connect *);
    //�õ���Command��ZCE_Mysql_Connect����
    inline ZCE_Mysql_Connect *get_connection();

    inline MYSQL_STMT *get_stmt_handle();

    //����SQL Command���,ΪBIN�͵�SQL���׼��,ͬʱ�󶨲���,���
    int set_stmt_command(const std::string &sqlcmd,
                         ZCE_Mysql_STMT_Bind *bindparam,
                         ZCE_Mysql_STMT_Bind *bindresult);

    //����SQL Command���,TXT,BIN��䶼����,ͬʱ�󶨲���,���
    int set_stmt_command(const char *stmtcmd, size_t szsql,
                         ZCE_Mysql_STMT_Bind *bindparam,
                         ZCE_Mysql_STMT_Bind *bindresult);

    //�õ�SQL Command���,TXT��
    const char *get_stmt_command() const;
    //�õ�SQL Command���,ΪBIN�����
    void get_stmt_command(char *, size_t &) const;
    //�õ�SQL Command���
    void get_stmt_command(std::string &) const;

    //ִ��SQL���,�������������ϵ�����
    int execute(unsigned int &num_affect, unsigned int &lastid);
    //ִ��SQL���,SELECT���,ת��������ϵ�����,ע������������õ���mysql_store_result.
    int execute(unsigned int &num_affect);

    //�ӽ�����ȡ������
    int fetch_row_next() const;
    //
    int SeekResultRow(unsigned int nrow) const;

    //ȡ��һ��
    int  fetch_column(MYSQL_BIND *bind, unsigned int column, unsigned int offset) const;

    //���ؽ����������Ŀ
    inline unsigned int get_num_of_result_rows() const;
    //���ؽ����������Ŀ
    inline unsigned int get_num_of_result_fields() const;

    //������ת��ΪMetaData,MySQL�Ľ������
    void ParamToMetadata(ZCE_Mysql_Result *) const;
    //�����ת��ΪMetaData,MySQL�Ľ������
    void ResultToMetadata(ZCE_Mysql_Result *) const;

    // ���ش�����Ϣ
    inline const char *get_error_message() const;
    // ���ش����
    inline unsigned int get_error_no() const;

protected:

    ///����
    ZCE_Mysql_Connect  *mysql_connect_;

    ///STMT SQL ����
    std::string         stmt_command_;

    ///STMT ��Handle
    MYSQL_STMT         *mysql_stmt_;

    ///�Ƿ�󶨽����
    bool               is_bind_result_;

};

//�õ�connect �ľ��
inline ZCE_Mysql_Connect *ZCE_Mysql_STMT_Command::get_connection()
{
    return mysql_connect_;
}

//����STMT Handle
inline MYSQL_STMT *ZCE_Mysql_STMT_Command::get_stmt_handle()
{
    return mysql_stmt_;
}

// ���ش�����Ϣ
inline const char *ZCE_Mysql_STMT_Command::get_error_message() const
{
    return mysql_stmt_error(mysql_stmt_);
}

// ���ش����
inline unsigned int ZCE_Mysql_STMT_Command::get_error_no() const
{
    return mysql_stmt_errno(mysql_stmt_);
}

//���ؽ����������Ŀ
inline unsigned int ZCE_Mysql_STMT_Command::get_num_of_result_rows() const
{
    return static_cast <unsigned int>(mysql_stmt_num_rows(mysql_stmt_));
}

//���ؽ����������Ŀ
inline unsigned int ZCE_Mysql_STMT_Command::get_num_of_result_fields() const
{
    return static_cast <unsigned int>(mysql_stmt_field_count(mysql_stmt_));
}

#endif //MYSQL_VERSION_ID >= 40100

//�����Ҫ��MYSQL�Ŀ�
#endif //#if defined MYSQL_VERSION_ID

#endif //ZCE_LIB_MYSQL_STMT_COMMAND_H_

