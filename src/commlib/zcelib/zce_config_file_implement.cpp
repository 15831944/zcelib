#include "zce_predefine.h"
#include "zce_os_adapt_string.h"
#include "zce_os_adapt_file.h"
#include "zce_os_adapt_error.h"
#include "zce_trace_log_debug.h"
#include "zce_config_file_implement.h"

/******************************************************************************************
class ZCE_INI_Implement INI�ļ������ö�ȡ��д��ʵ����
******************************************************************************************/


//
int ZCE_INI_Implement::read(const char *file_name, ZCE_Conf_PropertyTree *propertytree)
{
    //1�е����ֵ
    char one_line[LINE_BUFFER_LEN + 1], str_key[LINE_BUFFER_LEN + 1], str_value[LINE_BUFFER_LEN + 1];

    one_line[LINE_BUFFER_LEN] = '\0';
    str_key[LINE_BUFFER_LEN] = '\0';
    str_value[LINE_BUFFER_LEN] = '\0';

    ZCE_Conf_PropertyTree *cur_node = NULL;

    std::ifstream cfgfile(file_name);

    //�ļ��򲻿�������Ĭ��ֵ
    if (!cfgfile)
    {
        ZCE_LOGMSG(RS_ERROR, "[zcelib]: ZCE_INI_Implement::read config fail.path=[%s] ,last error [%d]",
                   file_name,
                   ZCE_LIB::last_error() );
        return -1;
    }

    while (cfgfile)
    {

        cfgfile.getline(one_line, LINE_BUFFER_LEN);
        //����
        ZCE_LIB::strtrim(one_line);

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
            ZCE_LIB::strtrim(one_line);

            ZCE_Conf_PropertyTree *tree_node = NULL;
            propertytree->add_child(one_line, tree_node);
            cur_node = tree_node;

            continue;
        }

        char *str = strstr(one_line, "=");
        if (str != NULL && cur_node)
        {
            char *snext = str + 1;
            *str = '\0';
            strncpy(str_key, one_line, LINE_BUFFER_LEN);
            strncpy(str_value, snext, LINE_BUFFER_LEN);
            ////
            ZCE_LIB::strtrim(str_key);
            ZCE_LIB::strtrim(str_value);

            //�ҵ����ء�
            std::string val(str_value);
            cur_node->set_leaf<std::string &>(str_key, val);
        }
    }

    return 0;

}

#if defined ZCE_USE_RAPIDXML && ZCE_USE_RAPIDXML == 1

/******************************************************************************************
class ZCE_XML_Implement INI�ļ������ö�ȡ��д��ʵ����
******************************************************************************************/


int ZCE_XML_Implement::read(const char *file_name, ZCE_Conf_PropertyTree *propertytree)
{
    int ret = 0;
    size_t file_size = 0;
    ret = ZCE_LIB::filelen(file_name, &file_size);
    if (0 != ret)
    {
        ZCE_LOGMSG(RS_ERROR, "[zcelib]: ZCE_XML_Implement::read fail,ZCE_LIB::filelen."
                   "path=[%s],last error [%d]",
                   file_name,
                   ZCE_LIB::last_error());
        return ret;
    }
    size_t buf_len = file_size + 16, read_len = 0;
    //ֻ��unique_ptr ����Ĭ��ֱ��ʹ�����飬
    std::unique_ptr<char[]> file_data(new char[buf_len]);
    ret = ZCE_LIB::read_file_data(file_name, file_data.get(), buf_len, &read_len);
    if (0 != ret)
    {
        ZCE_LOGMSG(RS_ERROR, "[zcelib]: ZCE_XML_Implement::read fail,ZCE_LIB::read_file_data."
                   "path=[%s],last error [%d]",
                   file_name,
                   ZCE_LIB::last_error());
        return ret;
    }

    try
    {
        // character type defaults to char
        rapidxml::xml_document<char> doc;
        //parse_non_destructive
        doc.parse<rapidxml::parse_default>(file_data.get());

        const rapidxml::xml_node<char> *root = doc.first_node();
        //��ȱ���dom tree
        read_dfs(root, propertytree);
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


//������ȶ�д
void ZCE_XML_Implement::read_dfs(const rapidxml::xml_node<char> *node,
                                 ZCE_Conf_PropertyTree *propertytree)
{

    if (NULL == node->value() && NULL == node->first_attribute()  && NULL == node->first_node() )
    {
        return;
    }
    //һЩnode��ʱ������
    if (node->type() == rapidxml::node_comment)
    {
        return;
    }
    ZCE_Conf_PropertyTree *pt_note = NULL;
    propertytree->add_child(node->name(), pt_note);

    if (node->value())
    {
        pt_note->set_leaf("<self_note>", node->value());
    }
    if (node->first_attribute())
    {
        rapidxml::xml_attribute<char> *node_attr = node->first_attribute();
        do
        {
            pt_note->set_leaf(node_attr->name(), node_attr->value());
            node_attr = node_attr->next_attribute();
        }
        while (node_attr);
    }
    //�����ӽڵ㣬��ȵݹ�
    if (node->first_node())
    {
        rapidxml::xml_node<char> *node_child = node->first_node();
        do
        {
            read_dfs(node_child, pt_note);
            node_child = node_child->next_sibling();
        }
        while (node_child);
    }
}

#endif



