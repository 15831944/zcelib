/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_mysql_field.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2004��7��24��
* @brief      MySQL���ֶη�װ
* 
* @details
* 
*/

#ifndef ZCE_LIB_MYSQL_DB_FIELD_H_
#define ZCE_LIB_MYSQL_DB_FIELD_H_

//�����Ҫ��MYSQL�Ŀ�
#if defined MYSQL_VERSION_ID


/*!
@brief      MySQL���ֶη�װ

*/
class ZCELIB_EXPORT ZCE_Mysql_Field
{

public:
    //���캯��
    ZCE_Mysql_Field():
        field_data_(NULL),
        field_length_(0),
        field_type_(FIELD_TYPE_NULL)
    {
    };

    //���캯��
    ZCE_Mysql_Field(const char *fdata, unsigned int flength, enum_field_types ftype):
        field_data_(fdata),
        field_length_(flength),
        field_type_(ftype)
    {
    };

    ///��������
    ~ZCE_Mysql_Field()
    {
    };

    /*!
    * @brief      �����ֶ�����
    * @param[in]  fdata    �ֶ�����
    * @param[in]  flength  �ֶγ���
    * @param[in]  ftype    �ֶ�����
    */
    inline void set_field(const char *fdata,
                          unsigned int flength,
                          enum_field_types ftype);

    ///�õ��ֶ�����
    inline enum_field_types get_type() const;
    ///�õ��ֶ�����,Char*
    inline const char *get_data() const;
    ///�õ��ֶεĳ���
    inline unsigned int get_length() const;

    ///�Ƿ�����Ϊ��
    inline bool is_null() const;

    //���ṩBOOL���͵�ת��,��ΪMYSQL�����ṩ������ͣ�ת���ĸо����ǹֵֹ�
    //ת��ΪBOOL����,��������ֵ���� 0��ʾfalse,��0��ʾ��
    //inline bool AsBool() const;

    //���ٵĵõ��ַ���,���ַ�����Ϊ��ʱ���ʹ���������
    void get_string(std::string &) const;

    //����ת���Ĳ�������

    ///�õ��ַ�����
    inline operator std::string () const;

    //ת��Ϊ8λ����
    inline operator char() const;
    //ת��Ϊ16λ������
    inline operator short() const;
    ///ת��Ϊ32Ϊ����
    inline operator int() const;
    ///ת��Ϊ������
    inline operator long() const;
    ///ת��Ϊ64λ����
    inline operator long long() const;

    //ת��Ϊ8λ�޷�������
    inline operator unsigned char() const;
    //ת��Ϊ16λ���޷�������
    inline operator unsigned short() const;
    ///ת��Ϊ�޷���32Ϊ����
    inline operator unsigned int() const;
    ///ת��Ϊ�޷��ų�����
    inline operator unsigned long() const;
    ///ת��Ϊ64λ�޷��ų�����
    inline operator unsigned long long() const;

    ///ת��Ϊdouble��
    inline operator double() const;
    ///ת��Ϊfloat��
    inline operator float() const;

private:

    ///�ֶ�����,ָ�룬ָ�����������ݣ����Խ�����������
    const char      *field_data_;
    ///�ֶγ���
    unsigned int     field_length_;
    ///�ֶ�����
    enum_field_types field_type_;

};

//����ֵ
inline void ZCE_Mysql_Field::set_field(const char *fdata, unsigned int flength, enum_field_types ftype)
{
    field_data_ = fdata;
    field_length_ = flength;
    field_type_ = ftype;
};

//�õ�Field ����
inline enum_field_types ZCE_Mysql_Field::get_type() const
{
    return field_type_;
}

//�õ�Field����
inline unsigned int ZCE_Mysql_Field::get_length() const
{
    return field_length_;
}

//
inline const char *ZCE_Mysql_Field::get_data() const
{
    return field_data_;
};

//�Ƿ�����Ϊ��
inline bool ZCE_Mysql_Field::is_null() const
{
    if (field_data_ == NULL)
    {
        return true;
    }

    return false;
};

//���ٵĵõ��ַ���,���ַ�����Ϊ��ʱ���ʹ���������
inline ZCE_Mysql_Field::operator std::string() const
{
    std::string tmp_str;
    tmp_str.reserve(field_length_);
    tmp_str.assign(field_data_, field_length_);
    return tmp_str;
}

//�õ��ַ���
inline ZCE_Mysql_Field::operator char() const
{
    char  tmp_tinyint = 0;

    short tmpshort = 0;
    //�õ�������
    sscanf(field_data_, "%hd", &tmpshort);

    //ͨ�����õ�TINY INT ����char
    tmp_tinyint = static_cast<char >( tmpshort & 0xFF);
    return tmp_tinyint;
}

//�õ�������
inline ZCE_Mysql_Field::operator short() const
{
    short tmp_short = 0;

    //�õ�������,����ֵ=0 ,EOF��ʾʧ��
    sscanf(field_data_, "%hd", &tmp_short);

    //ת�����ɹ�����
    return tmp_short;
}

//�õ�������
inline ZCE_Mysql_Field::operator long() const
{
    long tmp_long = 0;

    //�õ�������,
    sscanf(field_data_, "%ld", &tmp_long);

    return tmp_long;
}

//�õ�int����
inline ZCE_Mysql_Field::operator int() const
{
    int tmp_int = 0;

    //�õ�������,
    sscanf(field_data_, "%d", &tmp_int);
    return tmp_int;
}

//�õ�int����
inline ZCE_Mysql_Field::operator unsigned int() const
{
    unsigned int tmp_uint = 0;

    //�õ�������,
    sscanf(field_data_, "%u", &tmp_uint);
    return tmp_uint;
}

//ת��ΪINT64����
inline ZCE_Mysql_Field::operator long long () const
{
    long long tmp_longlong = 0;

    //�õ�64λ������
    sscanf(field_data_, "%lld", &tmp_longlong);

    return tmp_longlong;
}

//ת��Ϊ8λ�޷�������
inline ZCE_Mysql_Field::operator unsigned char() const
{
    unsigned char  tmp_utinyint = 0;
    unsigned short tmpshort = 0;
    //�õ�������
    sscanf(field_data_, "%hu", &tmpshort);

    //ͨ�����õ�TINY INT ����char
    tmp_utinyint = static_cast<unsigned char>(tmpshort & 0xFF);
    return tmp_utinyint;
}

//ת��Ϊ16λ���޷�������
inline ZCE_Mysql_Field::operator unsigned short() const
{
    unsigned short tmp_ushort = 0;

    //�õ�������,����ֵ=0 ,EOF��ʾʧ��
    sscanf(field_data_, "%hu", &tmp_ushort);

    //ת�����ɹ�����
    return tmp_ushort;
}
//ת��Ϊ32λ32�޷��ų�����
inline ZCE_Mysql_Field::operator unsigned long() const
{
    long tmp_ulong = 0;

    //�õ�������,
    sscanf(field_data_, "%lu", &tmp_ulong);
    return tmp_ulong;
}

//ת��Ϊ64λ�޷��ų�����
inline ZCE_Mysql_Field::operator unsigned long long() const
{
    unsigned long long tmp_ulonglong = 0;

    //�õ�64λ������
    sscanf(field_data_, "%llu", &tmp_ulonglong);
    return tmp_ulonglong;
}

//ת��ΪFLOAT����
inline ZCE_Mysql_Field::operator float() const
{
    float tmpfloat = 0.0;
    //ת���õ�FLOAT������
    sscanf(field_data_, "%f", &tmpfloat);
    return tmpfloat;
}

//ת��ΪDOUBLE����
inline ZCE_Mysql_Field::operator double() const
{
    double tmpdouble = 0.0;
    //ת���õ�DOBULE������
    sscanf(field_data_, "%lf", &tmpdouble);
    return tmpdouble;
}

#endif //#if defined MYSQL_VERSION_ID

#endif //ZCE_LIB_MYSQL_DB_FIELD_H_

