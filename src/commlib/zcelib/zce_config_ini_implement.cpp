#include "zce_predefine.h"
#include "zce_os_adapt_string.h"
#include "zce_config_ini_implement.h"

/******************************************************************************************
class ZCE_INI_Implemention INI�ļ������ö�ȡ��д��ʵ����
******************************************************************************************/
ZCE_INI_Implemention::ZCE_INI_Implemention()
{
}

ZCE_INI_Implemention::~ZCE_INI_Implemention()
{
}

//
int ZCE_INI_Implemention::read(const char *file_name, ZCE_Conf_PropertyTree &propertytree)
{
    //8*1024,1�е����ֵ
    const size_t LINE_BUFFER_LEN = 8192;
    char one_line[LINE_BUFFER_LEN + 1], str_key[LINE_BUFFER_LEN + 1], str_value[LINE_BUFFER_LEN + 1];

    one_line[LINE_BUFFER_LEN] = '\0';
    str_key[LINE_BUFFER_LEN] = '\0';
    str_value[LINE_BUFFER_LEN] = '\0';

    std::string cur_section;

    std::ifstream cfgfile(file_name);

    //�ļ��򲻿�������Ĭ��ֵ
    if (!cfgfile)
    {
        return -1;
    }

    while (cfgfile)
    {

        cfgfile.getline(one_line, LINE_BUFFER_LEN);
        //����
        ZCE_OS::strtrim(one_line);

        //ע����
        if (one_line[0] == ';' || one_line[0] == '#' )
        {
            continue;
        }

        //�ҵ�һ��section
        if (one_line[0] == '[' && one_line[strlen(one_line) - 1] == ']')
        {
            //�Ѿ��ҵ���һ��Section,û�з�����ص�Key������Ĭ��ֵ

            //ȥ��'[',']'
            memmove(one_line, one_line + 1, strlen(one_line) - 1);
            one_line[strlen(one_line) - 2] = '\0';

            //����ո�
            ZCE_OS::strtrim(one_line);
            cur_section  = one_line;

            //
            propertytree.new_child("", one_line);
        }

        char *str = strstr(one_line, "=");

        if (str != NULL)
        {
            char *snext = str + 1;
            *str = '\0';
            strncpy(str_key, one_line, LINE_BUFFER_LEN);
            strncpy(str_value, snext, LINE_BUFFER_LEN);
            ////
            ZCE_OS::strtrim(str_key);
            ZCE_OS::strtrim(str_value);

            //�ҵ����ء�
            propertytree.put_leaf_str(cur_section, str_key, str_value);
        }
    }

    return 0;

}

/******************************************************************************************
class ZCE_INI_PropertyTree INI�ļ������ö�ȡ���ŵ��������������չ��Ҫ�Ǹ�һЩ���﷨�Ǹ����
******************************************************************************************/

//��ȡstring����,
int ZCE_INI_PropertyTree::get_string_value(const std::string &sectionname,
                                           const std::string &keyname,
                                           std::string &value) const
{
    return get_leaf_str(sectionname, keyname, value);
}

//��ȡstring,����,
int ZCE_INI_PropertyTree::get_string_value(const std::string &sectionname,
                                           const std::string &keyname,
                                           char *buf,
                                           size_t size_buf) const
{
    return get_leaf_str(sectionname, keyname, buf, size_buf);
}

//��ȡ����,����int32
int ZCE_INI_PropertyTree::get_int32_value(const std::string &sectionname,
                                          const std::string &keyname,
                                          int &value) const
{
    return get_leaf_int(sectionname, keyname, value);
}

//��ȡ����,����uint32
int ZCE_INI_PropertyTree::get_uint32_value(const std::string &sectionname,
                                           const std::string &keyname,
                                           uint32_t &value) const
{

    return get_leaf_uint(sectionname, keyname, value);
}

//
int ZCE_INI_PropertyTree::get_bool_value(const std::string &sectionname,
                                         const std::string &keyname,
                                         bool &value) const
{
    return get_leaf_bool(sectionname, keyname, value);
}

