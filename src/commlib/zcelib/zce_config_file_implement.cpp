#include "zce_predefine.h"
#include "zce_os_adapt_string.h"
#include "zce_config_file_implement.h"

/******************************************************************************************
class ZCE_INI_Implement INI�ļ������ö�ȡ��д��ʵ����
******************************************************************************************/
ZCE_INI_Implement::ZCE_INI_Implement()
{
}

ZCE_INI_Implement::~ZCE_INI_Implement()
{
}

//
int ZCE_INI_Implement::read(const char *file_name, ZCE_Conf_PropertyTree &propertytree)
{
    //1�е����ֵ
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
        if (one_line[0] == ';' || one_line[0] == '#')
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
            cur_section = one_line;

            //
            PROPERTY_TREE_NODE *tree_node=NULL;
            propertytree.put_child("", one_line, tree_node);
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
            propertytree.put_leaf<std::string>(cur_section, str_key, str_value);
        }
    }

    return 0;

}



/******************************************************************************************
class ZCE_XML_Implement INI�ļ������ö�ȡ��д��ʵ����
******************************************************************************************/
ZCE_XML_Implement::ZCE_XML_Implement()
{
}

ZCE_XML_Implement::~ZCE_XML_Implement()
{
}
