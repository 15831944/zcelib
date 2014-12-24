
#include "zce_predefine.h"
#include "zce_mysql_stmtbind.h"

//��Щ��������4.1.2��İ汾����
#if MYSQL_VERSION_ID >= 40100

//���캯��
ZCE_Mysql_STMT_Bind::ZCE_Mysql_STMT_Bind(size_t numbind):
    num_bind_(numbind),
    current_bind_(0),
    stmt_bind_(NULL)
{
    stmt_bind_ = new MYSQL_BIND [numbind];
    memset(stmt_bind_, 0, sizeof(MYSQL_BIND )* numbind);

}

//��������
ZCE_Mysql_STMT_Bind::~ZCE_Mysql_STMT_Bind()
{
    if (stmt_bind_)
    {
        delete [] stmt_bind_;
    }

}

//��һ������
int ZCE_Mysql_STMT_Bind::bind_one_param(size_t paramno,
                                        ::enum_field_types paramtype,
                                        my_bool *is_null,
                                        void *paramdata,
                                        unsigned long szparam)
{
    ZCE_ASSERT(paramno < num_bind_);
    if (paramno >= num_bind_)
    {
        return -1;
    }

    stmt_bind_[paramno].buffer_type = paramtype;
    stmt_bind_[paramno].buffer = paramdata;

    stmt_bind_[paramno].is_null = is_null;
    stmt_bind_[paramno].length = NULL;
    stmt_bind_[paramno].buffer_length  = szparam;
    return 0;
}

int ZCE_Mysql_STMT_Bind::bind_one_result(size_t paramno,
                                         ::enum_field_types paramtype,
                                         void *paramdata,
                                         unsigned long *szparam )
{
    ZCE_ASSERT(paramno < num_bind_);
    if (paramno >= num_bind_)
    {
        return -1;
    }

    stmt_bind_[paramno].buffer_type = paramtype;
    stmt_bind_[paramno].buffer = paramdata;

    stmt_bind_[paramno].buffer_length = *szparam;
    stmt_bind_[paramno].length = szparam;
    return 0;
}



//��һ��char
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (char &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_TINY;
    stmt_bind_[current_bind_].buffer = (void *)(&val);
    stmt_bind_[current_bind_].buffer_length = sizeof(char);

    //���ӵ�ǰ�󶨱������
    ++current_bind_;

    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (short &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_SHORT;
    stmt_bind_[current_bind_].buffer = (void *)(&val);
    stmt_bind_[current_bind_].buffer_length = sizeof(short);

    //���ӵ�ǰ�󶨱������
    ++current_bind_;

    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (int &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_LONG;
    stmt_bind_[current_bind_].buffer = (void *)(&val);

    //���ӵ�ǰ�󶨱������
    ++current_bind_;

    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (long &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_LONG;
    stmt_bind_[current_bind_].buffer = (void *)(&val);

    //���ӵ�ǰ�󶨱������
    ++current_bind_;

    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (long long &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_LONGLONG;
    stmt_bind_[current_bind_].buffer = (void *)(&val);
    //���ӵ�ǰ�󶨱������
    ++current_bind_;

    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (unsigned char &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_TINY;
    stmt_bind_[current_bind_].buffer = (void *)(&val);

    //�޷���,�󶨽��ʱӦ�ò���
    stmt_bind_[current_bind_].is_unsigned = 1;
    //���ӵ�ǰ�󶨱������
    ++current_bind_;
    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (unsigned short &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_SHORT;
    stmt_bind_[current_bind_].buffer = (void *)(&val);

    //�޷���,�󶨽��ʱӦ�ò���
    stmt_bind_[current_bind_].is_unsigned = 1;
    //���ӵ�ǰ�󶨱������
    ++current_bind_;
    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (unsigned int &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_LONG;
    stmt_bind_[current_bind_].buffer = (void *)(&val);

    //�޷���,�󶨽��ʱӦ�ò���
    stmt_bind_[current_bind_].is_unsigned = 1;
    //���ӵ�ǰ�󶨱������
    ++current_bind_;
    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (unsigned long &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_LONG;
    stmt_bind_[current_bind_].buffer = (void *)(&val);
    stmt_bind_[current_bind_].buffer_length  = sizeof(unsigned long );
    //�޷���,�󶨽��ʱӦ�ò���
    stmt_bind_[current_bind_].is_unsigned = 1;

    //���ӵ�ǰ�󶨱������
    ++current_bind_;

    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (unsigned long long &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_LONGLONG;
    stmt_bind_[current_bind_].buffer = reinterpret_cast<void *>(&val);
    stmt_bind_[current_bind_].buffer_length  = sizeof(unsigned long long);
    //�޷���,�󶨽��ʱӦ�ò���
    stmt_bind_[current_bind_].is_unsigned = 1;
    //���ӵ�ǰ�󶨱������
    ++current_bind_;

    return *this;
}
//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (float &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_FLOAT;
    stmt_bind_[current_bind_].buffer = reinterpret_cast<void *>(&val);

    //ѭ����ǰ�󶨱������
    current_bind_  = (current_bind_ < num_bind_ - 1) ? current_bind_ + 1 : current_bind_;

    return *this;
}

//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (double &val)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_DOUBLE;
    stmt_bind_[current_bind_].buffer = reinterpret_cast<void *>(&val);

    //���ӵ�ǰ�󶨱������
    ++current_bind_;
    return *this;
}

//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (ZCE_Mysql_STMT_Bind::BinData_Param &bin_data)
{
    stmt_bind_[current_bind_].buffer_type = bin_data.stmt_data_type_;
    stmt_bind_[current_bind_].buffer = bin_data.stmt_pdata_;

    //������ܼ��ǰ󶨲���,Ҳ�ǰ󶨽��
    stmt_bind_[current_bind_].buffer_length  = bin_data.stmt_data_length_;
    stmt_bind_[current_bind_].length = NULL;

    //���ӵ�ǰ�󶨱������
    ++current_bind_;
    return *this;
}

//�󶨶����ƽ����������
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (ZCE_Mysql_STMT_Bind::BinData_Result &bin_data)
{
    stmt_bind_[current_bind_].buffer_type = bin_data.stmt_data_type_;
    stmt_bind_[current_bind_].buffer = bin_data.stmt_pdata_;

    //������ܼ��ǰ󶨲���,Ҳ�ǰ󶨽��
    stmt_bind_[current_bind_].buffer_length = *bin_data.stmt_data_length_;
    stmt_bind_[current_bind_].length = bin_data.stmt_data_length_;

    //���ӵ�ǰ�󶨱������
    ++current_bind_;
    return *this;
}

//
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (ZCE_Mysql_STMT_Bind::TimeData &timeadaptor)
{
    stmt_bind_[current_bind_].buffer_type = timeadaptor.stmt_timetype_;
    stmt_bind_[current_bind_].buffer = reinterpret_cast<void *>(timeadaptor.stmt_ptime_);

    stmt_bind_[current_bind_].buffer_length  = sizeof(MYSQL_TIME);
    stmt_bind_[current_bind_].length = NULL;

    //���ӵ�ǰ�󶨱������
    ++current_bind_;
    return *this;
}


//��һ���ղ���
ZCE_Mysql_STMT_Bind &ZCE_Mysql_STMT_Bind::operator << (ZCE_Mysql_STMT_Bind::NULL_Param & bind_null)
{
    stmt_bind_[current_bind_].buffer_type = MYSQL_TYPE_NULL;
    
    stmt_bind_[current_bind_].is_null = bind_null.is_null_;

    //���ӵ�ǰ�󶨱������
    ++current_bind_;
    return *this;
}



#endif //MYSQL_VERSION_ID >= 40100

