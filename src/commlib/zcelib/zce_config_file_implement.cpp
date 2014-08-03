#include "zce_predefine.h"
#include "zce_os_adapt_string.h"
#include "zce_os_adapt_file.h"
#include "zce_trace_log_debug.h"
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

#if defined ZCE_USE_RAPIDXML && ZCE_USE_RAPIDXML == 1

/******************************************************************************************
class ZCE_XML_Implement INI�ļ������ö�ȡ��д��ʵ����
******************************************************************************************/
ZCE_XML_Implement::ZCE_XML_Implement()
{
}

ZCE_XML_Implement::~ZCE_XML_Implement()
{
}

int ZCE_XML_Implement::read(const char *file_name, ZCE_Conf_PropertyTree &propertytree)
{
    int ret = 0;
    size_t file_size = 0;
    ret = ZCE_OS::filelen(file_name, &file_size);
    if (0 != ret)
    {
        return 0;
    }
    size_t buf_len = file_size+16,read_len = 0;
    //ֻ��unique_ptr ����Ĭ��ֱ��ʹ�����飬
    std::unique_ptr<char[]> file_data(new char[buf_len]);
    ret = ZCE_OS::read_file_data(file_name, file_data.get(), buf_len, &read_len);
    if (0 != ret)
    {
        return 0;
    }
    try
    {
        // character type defaults to char  
        rapidxml::xml_document<char> doc;   
        //parse_non_destructive
        doc.parse<rapidxml::parse_default>(file_data.get());

        rapidxml::xml_node<char> *root = doc.first_node();
        //��ȱ���dom tree

    }
    catch (rapidxml::parse_error &e)
    {
        ZCE_LOGMSG(RS_ERROR, "[ZCELIB]file [%s] don't parse error what[%s] where[%s].",
            e.what(),
            e.where<char>());
        return -1;
    }
    
    
    return 0;
}


//
void ZCE_XML_Implement::read_bfs(const rapidxml::xml_node<char> &note,
    ZCE_Conf_PropertyTree &propertytree)
{
    propertytree.leaf_node_ = note.name();

}

#endif



