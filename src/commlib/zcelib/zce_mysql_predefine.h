/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_mysql_predefine.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2004��7��24��
* @brief      MYSQL���һЩ�������壬�ʼ����ϣ��ͬʱ���ж��ԣ�����ֵ�����쳣3��ģʽ��
*             �������������һֱ����ʹ�÷���ֵ�ķ������쳣�����ް���
* 
* @details    ���Ȼ�Ǹ�����Ѷʱд�Ĵ��룬������ʱ�����Ƽ��õ����ݿ⻹��MYSQL 3.23��
*             ʱ����ķɿ죬���ִ��Ч��ԶԶ�ϲ���ʱ������ŵ��ٶȡ�
* 
* @note
* 
*/

#ifndef ZCE_LIB_MYSQL_DB_DEFINE_H_
#define ZCE_LIB_MYSQL_DB_DEFINE_H_

//Ĭ�ϵ�ѡ����û��ѡ���쳣�Ͷ��ԣ�����ѡ���˷��ش���

//
#define ER_BOUND_OVER_FLOW     3001
#define ER_DATA_CONVERT_ERROR  3002
#define ER_FIELD_TYPE_ERROR    3003

const char STR_BOUND_OVER_FLOW[]    =  "Result set is NULL , or access overflow bound data,or parameter error.";
const char STR_DATA_CONVERT_ERROR[] =  "Field data is NULL, or Convert fail,or parameter error.";
const char STR_FIELD_TYPE_ERROR[]   =  "Could exist field convert error. Please Check.";

//INITBUFSIZE =10K
#define INITBUFSIZE 1024*64

// OK
enum ZCELIB_EXPORT MYSQLRETURN
{
    MYSQL_RETURN_OK   = 0,
    MYSQL_RETURN_FAIL = -1,
};

class ZCELIB_EXPORT MySQLDBException
{
public:
    const unsigned int  m_ErrorID;
    const char         *m_ErrorStr;
public:

    MySQLDBException(unsigned int erid, const char *erstr):
        m_ErrorID(erid),
        m_ErrorStr(erstr)
    {
    };
    ~MySQLDBException()
    {
    };
    MySQLDBException &operator =(MySQLDBException &others)
    {
        if (&others != this)
        {
            memcpy(this, &others, sizeof(MySQLDBException));
        }

        return *this;
    }
};

//MYSQLCONNECTCHECK,MYSQLCOMMANDCHECK,MYSQLRESULTCHECK,FIELDCONVERTCHECK ���ڼ�����,
//�����ⲿ��Ķ��壬���������׳��쳣�����߷��ش��� MYSQL_RETURN_FAIL

//MYSQLCONNECTCHECK ���ڼ�� ZCE_Mysql_Connect ���еĴ���,

//MYSQLCOMMANDCHECK ���ڼ�� ZCE_Mysql_Command ���еĴ���,

//MYSQLRESULTCHECK������,����MYSQL_RETURN_FAIL,�����׳��쳣,����ʲôҲ����
//MYSQLRESULTCHECK ���ڼ�� ZCE_Mysql_Result ���еĴ���,

//FIELDCONVERTCHECK ���ڼ������ת��(sscanf)����

//��鷢������ʱ�ڴ�����ʹ���쳣
#if defined(MYSQLUSEEXCEPTION)

#define MYSQLCONNECTCHECK(f)  if(!(f)) \
    {\
        MySQLDBException sqlexp(get_error_no(),get_error_message()); \
        throw sqlexp;\
    }

#define MYSQLCOMMANDCHECK(f)  if(!(f)) \
    {\
        MySQLDBException sqlexp(get_error_no(),get_error_message());\
        throw sqlexp; \
    }

#define MYSQLSTMTCMDCHECK(f)  if(!(f)) \
    {\
        MySQLDBException sqlexp(get_error_no(),get_error_message());\
        throw sqlexp; \
    }

#define MYSQLRESULTCHECK(f)   if(!(f))\
    {\
        MySQLDBException sqlexp(ER_BOUND_OVER_FLOW,STR_FIELD_TYPE_ERROR);\
        throw sqlexp; \
    }

#define FIELDCONVERTCHECK(f,g,h)   if((f))\
    {\
        if(sscanf(f,g,h) <= 0) \
        {\
            MySQLDBException sqlexp1(ER_DATA_CONVERT_ERROR,STR_DATA_CONVERT_ERROR);\
            throw sqlexp1; \
        }\
    }

#define CHECKFIELDTYPE(f)  if(!(f))\
    {\
        MySQLDBException sqlexp(ER_FIELD_TYPE_ERROR,STR_BOUND_OVER_FLOW);\
        throw sqlexp; \
    }

//��鷢������ʱ�ڴ�����ʹ�ö���
#elif defined(MYSQLUSEASSERT) && defined(DEBUG)

#define MYSQLCONNECTCHECK(f)  if(!(f)) ZCE_ASSERT(false)

#define MYSQLCOMMANDCHECK(f)  if(!(f)) ZCE_ASSERT(false)

#define MYSQLSTMTCMDCHECK(f)  if(!(f)) ZCE_ASSERT(false)

#define MYSQLRESULTCHECK(f)   if(!(f)) ZCE_ASSERT(false)

#define FIELDCONVERTCHECK(f,g,h)   if((f))\
    {\
        if(sscanf(f,g,h) <= 0) \
        {\
            ZCE_ASSERT(false); \
        }\
    }

#define CHECKFIELDTYPE(f)  if(!(f)) ZCE_ASSERT(false)

//�ڴ����в�ʹ���쳣�Ͷ���,Ĭ������£�
#else //defined(USEMYSQLEXCEPTION)

#define MYSQLCONNECTCHECK(f)  if(!(f)) \
    { \
        return MYSQL_RETURN_FAIL; \
    }

#define MYSQLCOMMANDCHECK(f)  if(!(f)) \
    { \
        return MYSQL_RETURN_FAIL;  \
    }

#define MYSQLSTMTCMDCHECK(f)  if(!(f)) \
    { \
        return MYSQL_RETURN_FAIL; \
    }

#define FIELDCONVERTCHECK(f,g,h)   if((f)) \
    { \
        sscanf(f,g,h); \
    }

#define CHECKFIELDTYPE(f)         ((void)0)

//����ڷ�DEBUG�汾,����ٶ��з��İ���,�������Լ��϶���֤
//ȷ�Ͽ�ֵ�ͱ߽���,���������������� MYSQLNOCHECK

#if defined(MYSQLNOCHECK) && !defined(DEBUG)

#define MYSQLRESULTCHECK(f)       ((void)0)

#else //defined(NO_MYSQLCHECK) && !defined(DEBUG)

#define MYSQLRESULTCHECK(f)   if(!(f)) \
    { \
        return MYSQL_RETURN_FAIL; \
    }

#endif //defined(NOMYSQLCHECK) && !defined(DEBUG)

#endif //defined(USEMYSQLEXCEPTION)

#endif //ZCE_LIB_MYSQL_DB_DEFINE_H_

