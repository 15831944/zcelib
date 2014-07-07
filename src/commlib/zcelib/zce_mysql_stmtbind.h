/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_mysql_stmtbind.h
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

#ifndef ZCE_LIB_MYSQL_STMT_RESULT_H_
#define ZCE_LIB_MYSQL_STMT_RESULT_H_

//�����Ҫ��MYSQL�Ŀ�
#if defined ZCE_USE_MYSQL

//��Щ��������4.1.2��İ汾����
#if MYSQL_VERSION_ID >= 40100

#include "zce_trace_log_debug.h"


class STMT_BindData_Adaptor;
class STMT_BindTime_Adaptor;
class ZCE_Mysql_STMT_Bind;

/*!
* @brief      ������Ϊ������ ZCE_Mysql_STMT_Bind << �Ĳ�������
*             ��2��������
*/
class ZCELIB_EXPORT STMT_BindData_Adaptor
{

    friend class ZCE_Mysql_STMT_Bind;

public:
    //
    STMT_BindData_Adaptor(enum_field_types strtype, void *strdata, unsigned long szstr):
        stmt_data_type_(strtype),
        stmt_pdata_(strdata),
        stmt_data_length_(szstr)
    {
    };
    //
    ~STMT_BindData_Adaptor()
    {
    };
protected:
    //
    enum_field_types  stmt_data_type_;
    //
    void             *stmt_pdata_;
    //
    unsigned long     stmt_data_length_;
};

/*!
@brief      ������Ϊ������ ZCE_Mysql_STMT_Bind << �Ĳ�������

*/
class ZCELIB_EXPORT STMT_BindTime_Adaptor
{
    friend class ZCE_Mysql_STMT_Bind;
protected:
    //
    enum_field_types  stmt_timetype_;
    //
    MYSQL_TIME       *stmt_ptime_;

public:
    //
    STMT_BindTime_Adaptor(enum_field_types timetype, MYSQL_TIME *pstmttime):
        stmt_timetype_(timetype),
        stmt_ptime_(pstmttime)
    {
        ZCE_ASSERT(stmt_timetype_ == MYSQL_TYPE_TIMESTAMP ||
                   stmt_timetype_ == MYSQL_TYPE_DATE ||
                   stmt_timetype_ == MYSQL_TYPE_DATETIME ||
                   stmt_timetype_ == MYSQL_TYPE_TIMESTAMP
                  );
    };
    //
    ~STMT_BindTime_Adaptor()
    {
    };
};

/*!
* @brief      ������Ϊ������ ZCE_Mysql_STMT_Bind << �Ĳ�������
*             ��һ���ղ���
* @note
*/
class ZCELIB_EXPORT STMT_BindNULL_Adaptor
{
    friend class ZCE_Mysql_STMT_Bind;

public:
    STMT_BindNULL_Adaptor()
    {
    };
    ~STMT_BindNULL_Adaptor()
    {
    };
};



/*!
* @brief
*
* @note
*/
class ZCELIB_EXPORT ZCE_Mysql_STMT_Bind
{


protected:

    //���������ʵ��,�����޷���,�кܶ�ط����ҷ����ָ��,���ܸ���ǳ�ȸ���
    ZCE_Mysql_STMT_Bind &operator=(const ZCE_Mysql_STMT_Bind &others);

public:

    /*!
    * @brief      ���캯��
    * @param      numbind  Ҫ�󶨱���,����ĸ���
    */
    ZCE_Mysql_STMT_Bind(size_t numbind);
    //
    ~ZCE_Mysql_STMT_Bind();

    /*!
    * @brief      ��һ������
    * @return     int
    * @param      paramno   �����ı��
    * @param      paramtype ��������
    * @param      bisnull   �Ƿ�ΪNULL
    * @param      paramdata ���������ݵ�ָ��
    * @param      szparam   �����ĳ���
    */
    int bind_one_param(size_t paramno,
                       ::enum_field_types paramtype,
                       bool bisnull,
                       void *paramdata ,
                       unsigned long szparam = 0);

    //�õ�STMT HANDLE
    inline MYSQL_BIND *get_stmt_bind_handle();

    inline MYSQL_BIND *operator[](unsigned int paramno) const;

    //����Ӧ��ΪConst����һ�飬���ڰ󶨱���

    //��������
    ZCE_Mysql_STMT_Bind &operator << (char &);
    ZCE_Mysql_STMT_Bind &operator << (short &);
    ZCE_Mysql_STMT_Bind &operator << (int &);
    ZCE_Mysql_STMT_Bind &operator << (long &);
    ZCE_Mysql_STMT_Bind &operator << (long long &);

    ZCE_Mysql_STMT_Bind &operator << (unsigned char &);
    ZCE_Mysql_STMT_Bind &operator << (unsigned short &);
    ZCE_Mysql_STMT_Bind &operator << (unsigned int &);
    ZCE_Mysql_STMT_Bind &operator << (unsigned long &);
    ZCE_Mysql_STMT_Bind &operator << (unsigned long long &);

    ZCE_Mysql_STMT_Bind &operator << (float &);
    ZCE_Mysql_STMT_Bind &operator << (double &);

    //ZCE_Mysql_STMT_Bind& operator << (char *);

    //Ϊ��ʹ�ü������͵�������
    ZCE_Mysql_STMT_Bind &operator << (STMT_BindData_Adaptor &);
    ZCE_Mysql_STMT_Bind &operator << (STMT_BindTime_Adaptor &);
    ZCE_Mysql_STMT_Bind &operator << (STMT_BindNULL_Adaptor &);

    //���ڼ����󶨵Ĳ����Ƿ�ΪNULL
    bool get_bind_is_null(const size_t paramno) const;
    //���õڼ����󶨵Ĳ����Ƿ�ΪNULL
    void set_bind_is_null(const size_t paramno, const bool isnull);

    //�õ��ڼ����󶨵Ĳ���Buf�ĳ���
    unsigned long get_bind_buf_length(const size_t paramno) const;
    //���õڼ����󶨵Ĳ���Buf�ĳ���
    void set_bind_buf_length(const size_t paramno, unsigned long buflen);

    //�õ�BIND���ݵ�ʵ�ʳ���
    unsigned long get_bind_data_length(const size_t paramno) const;
    //
    void set_bind_data_length(const size_t paramno, unsigned long buflen);


protected:

    //�󶨵ı�������
    size_t           num_bind_;

    //��ǰʹ�õİ󶨲������,����<<
    size_t            current_bind_;

    //BIND MySQL�ķ�װ��ʽ���Ҳ�����vector,
    MYSQL_BIND       *stmt_bind_;
    //�Ƿ�ΪNULL
    my_bool          *is_bind_null_;

    //BIND������ʵ�ʵĳ���
    unsigned long    *bind_length_;
};

inline MYSQL_BIND *ZCE_Mysql_STMT_Bind::get_stmt_bind_handle()
{
    return stmt_bind_;
}

#endif //MYSQL_VERSION_ID >= 40100

#endif //#if defined ZCE_USE_MYSQL

#endif //ZCE_LIB_MYSQL_STMT_RESULT_H_

