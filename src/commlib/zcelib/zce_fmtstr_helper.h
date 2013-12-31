/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_fmtstr_helper.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2012��5��2��
* @brief      �ַ���������������ߣ�����Adapt�ȣ�������ͬ�����������
*             ������ʹ��ģ���ʱ��ͺܷ����ˡ�
*             Ҳ�ṩ��һ�����������
*             output_helper ����Ϊģ�庯���ṩͳһ����ڣ�
*             XXXXX_Out_Helper ��Щ��һ����Ϊ�˰����ṩ��ʽ�������
*             ZCE_XXXX_YYYY_OUT �ĺ�һ����Ϊ�˼򻯸�ʽ�������
* 
* @details    �������������ͣ�ZCE_LIB����Ļ����������ͣ������������
* 
*             2012��5��
*             ǰ��������Ϊ�����𾴵ĺ����أ����飩Ůʿ����ϴ�ɽ�����˳��������׶�
*             ���ѣ��־壬ĬĬ�����������У����ǿ�����һ�����⣬
*             ����Ϊ���ǵĴ��ڣ����ǿ�����ϣ��
* 
*/

#ifndef ZCE_FMTSTR_HELPER_H_
#define ZCE_FMTSTR_HELPER_H_

#include "zce_fmtstr_format.h"
#include "zce_os_adapt_time.h"
#include "zce_os_adapt_socket.h"
#include "zce_time_value.h"
#include "zce_socket_addr_in.h"
#include "zce_socket_addr_in6.h"
#include "zce_uuid_128_generator.h"
#include "zce_uuid_64_generator.h"

namespace ZCE_LIB
{

//��׼�������͵��������Ĭ�ϵĸ�ʽ------------------------------------------------------------------------

///�������һ��bool ����ֵ
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const bool &out_data)
{
    if (out_data)
    {
        const size_t SIZE_OF_STR_TRUE = 4;
        use_len = std::min<size_t>(max_len, SIZE_OF_STR_TRUE);
        strncpy(buffer, "TRUE", use_len);
    }
    else
    {
        const size_t SIZE_OF_STR_FALSE = 5;
        use_len = std::min<size_t>(max_len, SIZE_OF_STR_FALSE);
        strncpy(buffer, "FALSE", max_len);
    }

}

///�������һ��CHAR
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const char &out_data)
{

    const size_t CHAR_NEEN_ROOM = 1;

    //ֻ���һ���ַ�
    if (max_len >= CHAR_NEEN_ROOM)
    {
        *buffer = out_data;
        use_len = CHAR_NEEN_ROOM;
    }
    else
    {
        //max_len == 0;
        use_len = 0;
    }
}

///��������޷���CHAR
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const unsigned char &out_data)
{
    const size_t CHAR_NEEN_ROOM = 1;

    //ֻ���һ���ַ�
    if (max_len >= CHAR_NEEN_ROOM)
    {
        *buffer = out_data;
        use_len = CHAR_NEEN_ROOM;
    }
    else
    {
        //max_len == 0;
        use_len = 0;
    }
}

///����������������ַ���
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const short &out_data)
{
    int64_t cvt_out_data = out_data;
    ZCE_LIB::fmt_int64(buffer,
                       max_len,
                       use_len,
                       cvt_out_data,
                       BASE_DECIMAL,
                       0,
                       0,
                       0);
}

///��������޷��Ŷ�����
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const unsigned short &out_data)
{
    int64_t cvt_out_data = out_data;
    ZCE_LIB::fmt_int64(buffer,
                       max_len,
                       use_len,
                       cvt_out_data,
                       BASE_DECIMAL,
                       0,
                       0,
                       ZCE_LIB::FMT_UNSIGNED);
}

///�������int����
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const int &out_data)
{
    int64_t cvt_out_data = out_data;
    ZCE_LIB::fmt_int64(buffer,
                       max_len,
                       use_len,
                       cvt_out_data,
                       BASE_DECIMAL,
                       0,
                       0,
                       0);
}

///�޷���int����
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const unsigned int &out_data)
{
    int64_t cvt_out_data = out_data;
    ZCE_LIB::fmt_int64(buffer,
                       max_len,
                       use_len,
                       cvt_out_data,
                       BASE_DECIMAL,
                       0,
                       0,
                       ZCE_LIB::FMT_UNSIGNED);
}

///����long����
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const long &out_data)
{
    int64_t cvt_out_data = out_data;
    ZCE_LIB::fmt_int64(buffer,
                       max_len,
                       use_len,
                       cvt_out_data,
                       BASE_DECIMAL,
                       0,
                       0,
                       0);
}

///��������޷���long����
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const unsigned long &out_data)
{
    int64_t cvt_out_data = out_data;
    ZCE_LIB::fmt_int64(buffer,
                       max_len,
                       use_len,
                       cvt_out_data,
                       BASE_DECIMAL,
                       0,
                       0,
                       ZCE_LIB::FMT_UNSIGNED);
}

///��������з���long long����
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const long long &out_data)
{
    int64_t cvt_out_data = out_data;
    ZCE_LIB::fmt_int64(buffer,
                       max_len,
                       use_len,
                       cvt_out_data,
                       BASE_DECIMAL,
                       0,
                       0,
                       0);
}

///��������޷���long long����
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const unsigned long long &out_data)
{
    int64_t cvt_out_data = out_data;
    ZCE_LIB::fmt_int64(buffer,
                       max_len,
                       use_len,
                       cvt_out_data,
                       BASE_DECIMAL,
                       0,
                       0,
                       ZCE_LIB::FMT_UNSIGNED);
}

///�������float�����ȸ���
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const float &out_data)
{
    double cvt_out_data = out_data;
    ZCE_LIB::fmt_double(buffer,
                        max_len,
                        use_len,
                        cvt_out_data);
}

///�������double˫���ȸ���
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const double &out_data)
{
    double cvt_out_data = out_data;
    ZCE_LIB::fmt_double(buffer,
                        max_len,
                        use_len,
                        cvt_out_data);
}

///�������const char�ַ��������������
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const char *out_data)
{
    const char *cvt_out_data = out_data;
    size_t str_len = strlen (cvt_out_data);
    ZCE_LIB::fmt_str(buffer,
                     max_len,
                     use_len,
                     cvt_out_data,
                     str_len);
}

///�������std::string�ַ������
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const std::string &out_data)
{
    ZCE_LIB::fmt_str(buffer,
                     max_len,
                     use_len,
                     out_data.c_str(),
                     out_data.length()
                    );
}

//===========================================================================================
//һЩC���ԵĽṹ���ַ������

///���Timeʱ��
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          timeval &out_data)
{

    const char *ret_str = ZCE_OS::timestamp(&out_data,
                                            buffer,
                                            max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = ZCE_OS::LEN_OF_TIME_STAMP;
    }
    else
    {
        use_len = 0;
    }
}

///IPV6��socket��ַ
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const sockaddr_in &out_data)
{
    const char *ret_str = ZCE_OS::socketaddr_ntop_ex((const sockaddr *)(&out_data),
                                                     buffer,
                                                     max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = strlen(buffer);
    }
    else
    {
        use_len = 0;
    }
}

///IPV6��socket��ַ
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const sockaddr_in6 &out_data)
{
    const char *ret_str = ZCE_OS::socketaddr_ntop_ex((const sockaddr *)(&out_data),
                                                     buffer,
                                                     max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = strlen(buffer);
    }
    else
    {
        use_len = 0;
    }
}

///sockaddr *��ָ������
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const sockaddr *out_data)
{
    const char *ret_str = ZCE_OS::socketaddr_ntop_ex(out_data,
                                                     buffer,
                                                     max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = strlen(buffer);
    }
    else
    {
        use_len = 0;
    }
}

///����IPV4��socket��ַ�ַ���
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const in_addr &out_data)
{
    const char *ret_str = ZCE_OS::inet_ntop(AF_INET,
                                            (void *)(&out_data),
                                            buffer,
                                            max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = strlen(buffer);
    }
    else
    {
        use_len = 0;
    }
}

///�������IPV6��socket��ַ�ַ���
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const in6_addr &out_data)
{
    const char *ret_str = ZCE_OS::inet_ntop(AF_INET6,
                                            (void *)(&out_data),
                                            buffer,
                                            max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = strlen(buffer);
    }
    else
    {
        use_len = 0;
    }
}

//===========================================================================================
//һЩZCELIB���õĽṹ���ַ������

///���ZCE_Time_Value��ʱ��
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const ZCE_Time_Value &out_data)
{
    const char *ret_str = out_data.timestamp(buffer,
                                             max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = ZCE_OS::LEN_OF_TIME_STAMP;
    }
    else
    {
        use_len = 0;
    }
}

///���ZCE_Sockaddr_In���ַ���
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const ZCE_Sockaddr_In &out_data)
{
    const char *ret_str = out_data.to_string(buffer,
                                             max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = strlen(buffer);
    }
    else
    {
        use_len = 0;
    }
}

///�������ZCE_Sockaddr_In6���ַ���
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const ZCE_Sockaddr_In6 &out_data)
{
    const char *ret_str = out_data.to_string(buffer,
                                             max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = strlen(buffer);
    }
    else
    {
        use_len = 0;
    }
}

///�������ZCE_UUID64���ַ���
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const ZCE_UUID64 &out_data)
{
    const char *ret_str = out_data.to_string(buffer,
                                             max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = ZCE_UUID64::LEN_OF_ZCE_UUID64_STR;
    }
    else
    {
        use_len = 0;
    }
}

///�������ZCE_UUID128���ַ���
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const ZCE_UUID128 &out_data)
{
    const char *ret_str = out_data.to_string(buffer,
                                             max_len);

    //���سɹ�
    if (ret_str)
    {
        use_len = ZCE_UUID128::LEN_OF_ZCE_UUID128_STR;
    }
    else
    {
        use_len = 0;
    }
}

//==================================================================================================
//����ĸ�ʽ�����ʽ

///INT��ʽ����������࣬���԰�������һЩ��ʽ�����
class Int_Out_Helper
{
public:
    //������Ĺ��캯��
    explicit Int_Out_Helper(int64_t out_data,
                            BASE_NUMBER_SYSTEM base = BASE_DECIMAL,
                            size_t width = -1,
                            size_t precision = size_t(-1),
                            int flags = 0);

    ~Int_Out_Helper();

public:

    ///�����int����
    int64_t            out_data_;
    ///����Ľ��Ʒ�ʽ
    BASE_NUMBER_SYSTEM base_;
    ///�����ȣ�Ĭ��0
    size_t             width_;
    ///������ȣ�Ĭ��-1�����ֵ��
    size_t             precision_;
    ///�����ʽ���Ʊ�־
    int                flags_;
};

//�������16����INT
#ifndef ZCE_INT_HEX_OUT
#define ZCE_INT_HEX_OUT(x)   ZCE_LIB::Int_Out_Helper((x),\
                                                     ZCE_LIB::BASE_HEXADECIMAL,\
                                                     0,\
                                                     size_t(-1),\
                                                     ZCE_LIB::FMT_NUM|ZCE_LIB::FMT_UP)
#endif

//�������16����UINT
#ifndef ZCE_UINT_HEX_OUT
#define ZCE_UINT_HEX_OUT(x)   ZCE_LIB::Int_Out_Helper((x),\
                                                      ZCE_LIB::BASE_HEXADECIMAL,\
                                                      0,\
                                                      size_t(-1),\
                                                      ZCE_LIB::FMT_NUM|ZCE_LIB::FMT_UP|ZCE_LIB::FMT_UNSIGNED)
#endif

//�������8����INT
#ifndef ZCE_INT_OCT_OUT
#define ZCE_INT_OCT_OUT(x)   ZCE_LIB::Int_Out_Helper((x),\
                                                     ZCE_LIB::BASE_OCTAL,\
                                                     0,\
                                                     size_t(-1),\
                                                     ZCE_LIB::FMT_NUM|ZCE_LIB::FMT_UP)
#endif

//�������8����UINT
#ifndef ZCE_UINT_OCT_OUT
#define ZCE_UINT_OCT_OUT(x)   ZCE_LIB::Int_Out_Helper((x),\
                                                      ZCE_LIB::BASE_OCTAL,\
                                                      0,\
                                                      size_t(-1),\
                                                      ZCE_LIB::FMT_NUM|ZCE_LIB::FMT_UP|ZCE_LIB::FMT_UNSIGNED)
#endif

//�������2����INT
#ifndef ZCE_INT_BIN_OUT
#define ZCE_INT_BIN_OUT(x)   ZCE_LIB::Int_Out_Helper((x),\
                                                     ZCE_LIB::BASE_BINARY,\
                                                     0,\
                                                     size_t(-1),\
                                                     ZCE_LIB::FMT_UP)
#endif

//�������2����UINT
#ifndef ZCE_UINT_BIN_OUT
#define ZCE_UINT_BIN_OUT(x)   ZCE_LIB::Int_Out_Helper((x),\
                                                      ZCE_LIB::BASE_BINARY,\
                                                      0,\
                                                      size_t(-1),\
                                                      ZCE_LIB::FMT_UP|ZCE_LIB::FMT_UNSIGNED)
#endif

//INT��ʽ���������
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const Int_Out_Helper &out_data)
{
    ZCE_LIB::fmt_int64(buffer,
                       max_len,
                       use_len,
                       out_data.out_data_,
                       out_data.base_,
                       out_data.width_,
                       out_data.precision_,
                       out_data.flags_
                      );
}

///DOUBLE��ʽ�����������,������һЩ��ʽ��������
class Double_Out_Helper
{
public:
    explicit Double_Out_Helper(double out_data,
                               size_t width = -1,
                               size_t precision = size_t(-1),
                               int flags = 0);
    ~Double_Out_Helper();

public:
    //Ҫ����ĸ���
    double             out_data_;
    //����Ŀ��
    size_t             width_;
    //����ľ��ȣ�С�������
    size_t             precision_;
    //�����־
    int                flags_;
};

///����������С����ʽ����������е���⣬��double��float�����ԣ�
#ifndef ZCE_DOUBLE_DECIMAL_OUT
#define ZCE_DOUBLE_DECIMAL_OUT(x)   ZCE_LIB::Double_Out_Helper((x),\
                                                               0,\
                                                               size_t(-1),\
                                                               0)
#endif

///����������ָ����ʽ���
#ifndef ZCE_DOUBLE_EXPONENT_OUT
#define ZCE_DOUBLE_EXPONENT_OUT(x)  ZCE_LIB::Double_Out_Helper((x),\
                                                               0,\
                                                               size_t(-1),\
                                                               ZCE_LIB::FMT_UP|ZCE_LIB::FMT_EXPONENT)
#endif

///DOUBLE��ʽ���������
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const Double_Out_Helper &out_data)
{
    ZCE_LIB::fmt_double(buffer,
                        max_len,
                        use_len,
                        out_data.out_data_,
                        out_data.width_,
                        out_data.precision_,
                        out_data.flags_
                       );
}

///String��ʽ����������࣬������һЩ��ʽ������
class String_Out_Helper
{
public:
    //
    explicit String_Out_Helper(const char *out_str_ptr,
                               size_t out_str_len,
                               size_t width ,
                               size_t precision,
                               int flags);
    //
    explicit String_Out_Helper(const std::string &out_str,
                               size_t width = size_t(-1),
                               size_t precision = size_t(-1),
                               int flags = 0);
    //
    explicit String_Out_Helper(const char *out_str_ptr,
                               size_t width = size_t(-1),
                               size_t precision = size_t(-1),
                               int flags = 0);

    ~String_Out_Helper();

public:
    ///�ַ���
    const char        *out_str_ptr_;
    ///�ַ����ĳ���
    size_t             out_str_len_;
    ///����Ŀ��
    size_t             width_;
    ///����ľ��ȣ�С�������
    size_t             precision_;
    ///�����־
    int                flags_;
};

///const char *�ַ��������������
inline void output_helper(char *buffer,
                          size_t max_len,
                          size_t &use_len,
                          const String_Out_Helper &out_data)
{
    ZCE_LIB::fmt_str(buffer,
                     max_len,
                     use_len,
                     out_data.out_str_ptr_,
                     out_data.out_str_len_,
                     out_data.width_,
                     out_data.precision_,
                     out_data.flags_
                    );
}

///�����ַ����������ķ�ʽ���
#ifndef ZCE_STRING_LEFTALIGN_OUT
#define ZCE_STRING_LEFTALIGN_OUT(x,y)   ZCE_LIB::String_Out_Helper((x),\
                                                                   (y),\
                                                                   size_t(-1),\
                                                                   ZCE_LIB::FMT_LEFT_ALIGN)
#endif

///�����ַ������Ҷ���ķ�ʽ���
#ifndef ZCE_STRING_RIGHTALIGN_OUT
#define ZCE_STRING_RIGHTALIGN_OUT(x,y)  ZCE_LIB::String_Out_Helper((x),\
                                                                   (y),\
                                                                   size_t(-1),\
                                                                   0)
#endif

};//ZCE_LIB

#endif
