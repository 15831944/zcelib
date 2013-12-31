#include "zce_predefine.h"
#include "zce_mysql_field.h"

//�����Ҫ��MYSQL�Ŀ�
#if defined MYSQL_VERSION_ID

//High Speed �õ�String
void ZCE_Mysql_Field::get_string(std::string &tmpstr) const
{
    tmpstr.reserve(field_length_);
    tmpstr.assign(field_data_, field_length_);
}

//�����Ҫ��MYSQL�Ŀ�
#endif //#if defined MYSQL_VERSION_ID

