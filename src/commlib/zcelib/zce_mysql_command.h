/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_mysql_command.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2004��7��24��
* @brief      MySQL��SQL�������Command����
* 
* @details
* 
*/

#ifndef ZCE_LIB_MYSQL_DB_COMMAND_H_
#define ZCE_LIB_MYSQL_DB_COMMAND_H_

//�����Ҫ��MYSQL�Ŀ⣬�����MYSQL��ͷ�ļ�
#if defined MYSQL_VERSION_ID

#include "zce_boost_non_copyable.h"
#include "zce_mysql_predefine.h"
#include "zce_mysql_connect.h"
#include "zce_mysql_result.h"

/*!
* @brief      MYSQL������������ڴ���SQL����ִ�У���ý����
* 
*/
class ZCELIB_EXPORT ZCE_Mysql_Command: public ZCE_NON_Copyable
{

public:
    ///�������Ĺ��캯��
    ZCE_Mysql_Command();
    ///�������Ĺ��캯����ָ��һ��connect
    ZCE_Mysql_Command(ZCE_Mysql_Connect *);
    ///����������������
    ~ZCE_Mysql_Command();

    /*!
    * @brief      ����Command��ZCE_Mysql_Connect
    * @return     int  0�ɹ���-1ʧ��
    * @param      conn ���Ӷ��󣬱����Ѿ����ӳɹ��
    */
    int set_connection(ZCE_Mysql_Connect *conn);

    /*!
    * @brief      �õ���Command��ZCE_Mysql_Connect����
    * @return     ZCE_Mysql_Connect*
    */
    inline ZCE_Mysql_Connect *get_connection();

    /*!
    * @brief      ����SQL Command���,ΪBIN�͵�SQL���׼��
    * @param      sqlcmd SQL���
    * @param      szsql  SQL��䳤��
    */
    inline int set_sql_command(const char *sqlcmd, size_t szsql);

    /*!
    * @brief      ����SQL Command���,TXT,BIN��䶼����
    * @param      sqlcmd SQL���
    */
    inline int set_sql_command(const std::string &sqlcmd);

    /*!
    * @brief      ����SQL Command���,��̬�����汾
    * @return     int  0�ɹ���-1ʧ��
    * @param      sql_format    ��ʽ����SQL��䣬
    * @param      ...           ��̬����
    */
    int set_sql_command( const char *sql_format, ...);

    /*!
    * @brief      �õ�SQL Command���,TXT��
    * @return     const char*
    */
    const char *get_sql_command() const;

    /*!
    * @brief      �õ�SQL Command���,ΪBIN�����
    * @return     int  0�ɹ���-1ʧ��
    * @param[out]    cmdbuf   �����ַ���
    * @param[in,out] szbuf �ַ���buffer�ĳ���
    * @note
    */
    int get_sql_command(char *cmdbuf, size_t &szbuf) const;

    /*!
    * @brief      �õ�SQL Command���
    * @param      std::string& ���ص�SQL���
    */
    void get_sql_command(std::string &) const;

    /*!
    * @brief      ִ��SQL���,�������������ϵ����֣�INSERT,UPDATE����
    * @return     int         0�ɹ���-1ʧ��
    * @param      num_affect  ��ѯ�õ�������
    * @param      lastid      ����ID�ȣ������������ֶε�ʱ��(UINT32Ҳ���������ã��Ǻ�)
    */
    int execute(unsigned int &num_affect, unsigned int &lastid);

    /*!
    * @brief      ִ��SQL���,SELECT���,ת��������ϵ�����,ע������������õ���mysql_store_result.
    * @return     int         0�ɹ���-1ʧ��
    * @param      num_affect  ��ѯ�õ�������
    * @param      sqlresult   ���صĽ������
    */
    int execute(unsigned int &num_affect, ZCE_Mysql_Result &sqlresult);

    /*!
    * @brief      ִ��SQL���,SELECT���,USE������ϵ�����,ע������õ���mysql_use_result,num_affect������Ч
    *             ���ڽ����̫��Ĵ���,һ�δ����ռ��̫���ڴ�ĵĴ���,���Ƽ�ʹ��,
    * @return     int
    * @param      sqlresult ���صĽ������
    */
    int execute(ZCE_Mysql_Result &sqlresult);

#if MYSQL_VERSION_ID > 40100

    /*!
    * @brief      ���һ��ִ�ж���SQL��䣬�����������ȡ�ؽ������
    * @return     int       0��ʾ�ɹ��������ʶʧ��
    * @param[out] sqlresult ���ص�MySQL�������
    * @param[out] bstore    ʹ��mysql_store_resultȡ�ؽ�����ϣ�����mysql_use_result
    */
    int fetch_next_multi_result(ZCE_Mysql_Result &sqlresult, bool bstore = true);

#endif //MYSQL_VERSION_ID > 40100

    ///C++ ��һЩϰ���÷���Ϊ��ִ�ŵ�C++����ʹ���߱�д,��д��δ����ʱ�򣬺����C++����������Ȥ����
    /// =������,
    inline ZCE_Mysql_Command &operator =(const char *sqlcmd);
    inline ZCE_Mysql_Command &operator =(const std::string &sqlcmd);

    ///+=��������,������SQL Command �����STR
    inline ZCE_Mysql_Command &operator +=(const char *sqlcmd);
    inline ZCE_Mysql_Command &operator +=(const std::string &sqlcmd);

    /// << ��������,������SQL Command �����
    ZCE_Mysql_Command &operator << (const char );
    ZCE_Mysql_Command &operator << (const short );
    ZCE_Mysql_Command &operator << (const long );
    ZCE_Mysql_Command &operator << (const long long );

    ZCE_Mysql_Command &operator << (const unsigned char );
    ZCE_Mysql_Command &operator << (unsigned short );
    ZCE_Mysql_Command &operator << (unsigned long );
    ZCE_Mysql_Command &operator << (const unsigned long long );

    ZCE_Mysql_Command &operator << (const float );
    ZCE_Mysql_Command &operator << (const double );

    ZCE_Mysql_Command &operator << (const char *);
    ZCE_Mysql_Command &operator << (const std::string &);

    /*!
    * @brief      ���ش�����Ϣ
    * @return     const char*
    */
    inline const char *get_error_message();
    
    /*!
    * @brief      ���ش���ID
    * @return     unsigned int
    */
    inline unsigned int get_error_no();

    //��Щ��������4.1��İ汾����
#if MYSQL_VERSION_ID > 40100

    /*!
    * @brief      �����Ƿ��Զ��ύ
    * @return     int
    * @param      bauto
    * @note
    */
    int set_auto_commit(bool bauto);

    /*!
    * @brief      �ύ����Commit Transaction
    * @return     int
    * @note
    */
    int trans_commit();

    /*!
    * @brief      �ع�����Rollback Transaction
    * @return     int
    */
    int trans_rollback();

#endif //MYSQL_VERSION_ID > 40100

protected:

    /*!
    * @brief      ִ��SQL���,�ڲ��Ļ�������,�ô�ҹ�ͬ���õĻ�������
    * @return     int         int  0�ɹ���-1ʧ��
    * @param[out] num_affect  Ӱ����������������߷��ؽ��������
    * @param[out] lastid      ���Ĳ���ID��ʲô��
    * @param[out] sqlresult   SQLִ�к�Ľ������
    * @param[out] bstore      ʹ��ʲô��ʽ��ý����ture��ʹ��mysql_store_result,false��ʹ��mysql_use_result����Ҫ��ν�����,
    */
    int execute(unsigned int *num_affect ,
                unsigned int *lastid,
                ZCE_Mysql_Result *sqlresult,
                bool bstore);

protected:

    ///����
    ZCE_Mysql_Connect  *mysql_connect_;
    ///SQL
    std::string         mysql_command_;

};

//��Щ��������4.1��İ汾����
#if MYSQL_VERSION_ID > 40100

//�õ�������Ϣ
inline int ZCE_Mysql_Command::set_auto_commit(bool bauto)
{
    return mysql_connect_->set_auto_commit(bauto);
}

//�õ�������Ϣ
inline int ZCE_Mysql_Command::trans_commit()
{
    return mysql_connect_->trans_commit();
}

//�õ�������Ϣ
inline int ZCE_Mysql_Command::trans_rollback()
{
    return mysql_connect_->trans_rollback();
}

#endif //MYSQL_VERSION_ID > 40100

//�õ�connect �ľ��
inline ZCE_Mysql_Connect *ZCE_Mysql_Command::get_connection()
{
    return mysql_connect_;
}

//�õ�������Ϣ
inline const char *ZCE_Mysql_Command::get_error_message()
{
    return mysql_connect_->get_error_message();
}

//�õ������ID
inline unsigned int ZCE_Mysql_Command::get_error_no()
{
    return mysql_connect_->get_error_no();
}

//SQLԤ�Ƶĸ�ֵ��
inline int ZCE_Mysql_Command::set_sql_command(const char *sqlcmd, size_t szsql)
{
    //�������,����
    if (sqlcmd == NULL)
    {
        ZCE_ASSERT(false);
        return MYSQL_RETURN_FAIL;
    }

    //
    mysql_command_.assign(sqlcmd, szsql);
    return MYSQL_RETURN_OK;
}

//ΪTXT,BIN�����Ƶ�SQL�����ṩ�ĸ�ֵ��ʽ ,
inline int ZCE_Mysql_Command::set_sql_command(const std::string &sqlcmd)
{
    mysql_command_ = sqlcmd;
    return MYSQL_RETURN_OK;
}

//
inline ZCE_Mysql_Command &ZCE_Mysql_Command::operator =(const char *sqlcmd)
{
    set_sql_command(sqlcmd);
    return *this;
}
//
inline ZCE_Mysql_Command &ZCE_Mysql_Command::operator =(const std::string &sqlcmd)
{
    set_sql_command(sqlcmd);
    return *this;
}

//
inline ZCE_Mysql_Command &ZCE_Mysql_Command::operator +=(const char *sqlcmd)
{
    mysql_command_.append(sqlcmd);
    return *this;
}

inline ZCE_Mysql_Command &ZCE_Mysql_Command::operator +=(const std::string &sqlcmd)
{
    mysql_command_.append(sqlcmd);
    return *this;
}

#endif //#if defined MYSQL_VERSION_ID

#endif //ZCE_LIB_MYSQL_DB_COMMAND_H_

