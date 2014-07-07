#include "zce_predefine.h"
#include "zce_socket_addr_in.h"
#include "zce_socket_addr_in6.h"
#include "zce_config_property_tree.h"

//�ָ��,
char ZCE_Conf_PropertyTree::SEPARATOR_STRING[2] = ".";

//���캯��,��������
ZCE_Conf_PropertyTree::ZCE_Conf_PropertyTree()
{
}
ZCE_Conf_PropertyTree::~ZCE_Conf_PropertyTree()
{
}

//�õ������ڵ㣬const
int ZCE_Conf_PropertyTree::get_child(const std::string &path_str,
                                     const PROPERTY_TREE_NODE *& child_data) const
{
    child_data = NULL;

    //�������Լ�
    if (0 == path_str.length())
    {
        child_data = this;
        return 0;
    }

    //�ҵ�
    size_t str_pos = path_str.find(SEPARATOR_STRING, 0);

    std::string start_str(path_str, 0, str_pos);

    PROPERTY_TREE_MAP::const_iterator iter_tmp = child_node_map_.find(start_str);

    if (child_node_map_.end() == iter_tmp)
    {
        return -1;
    }

    const PROPERTY_TREE_NODE *child_tree = &(iter_tmp->second);

    //����·�������еݹ��ѯ
    if (str_pos != std::string::npos)
    {
        std::string remain_str(path_str, str_pos + 1);
        return child_tree->get_child(remain_str, child_data);
    }
    else
    {
        //����޷�����ǰ������,
        child_data = child_tree;
        return 0;
    }
}

//�õ������ڵ㣬
int ZCE_Conf_PropertyTree::get_child(const std::string &path_str,
                                     PROPERTY_TREE_NODE *& child_data)
{
    child_data = NULL;

    //�������Լ�
    if ( 0 == path_str.length() )
    {
        child_data = this;
        return 0;
    }

    //�ҵ�
    size_t str_pos = path_str.find(SEPARATOR_STRING, 0);

    std::string start_str(path_str, 0, str_pos);

    PROPERTY_TREE_MAP::iterator iter_tmp = child_node_map_.find(start_str);

    if ( child_node_map_.end() == iter_tmp )
    {
        return -1;
    }

    PROPERTY_TREE_NODE *child_tree = &(iter_tmp->second);

    //����·�������еݹ��ѯ
    if (str_pos != std::string::npos)
    {
        std::string remain_str(path_str, str_pos + 1);
        return child_tree->get_child(remain_str, child_data);
    }
    else
    {
        //����޷�����ǰ������,
        child_data = child_tree;
        return 0;
    }
}

//
int ZCE_Conf_PropertyTree::get_leafptr(const std::string &path_str,
                                       const std::string *& leaf_str_ptr) const
{
    int ret = 0;
    leaf_str_ptr = NULL;
    size_t last_key_pos = path_str.find_last_not_of(SEPARATOR_STRING);

    std::string tree_path, leaf_key;
    const PROPERTY_TREE_NODE *child_note = NULL;
    //���û�зָ����ţ�
    if (std::string::npos != last_key_pos)
    {
        tree_path.assign(path_str.cbegin(), path_str.cbegin() + last_key_pos);
        leaf_key.assign(path_str.cbegin() + last_key_pos + 1, path_str.cend());

        ret = get_child(tree_path, child_note);
        if (0 != ret)
        {
            return ret;
        }
    }
    else
    {
        leaf_key = path_str;
        child_note = this;
    }

    KEY_VALUE_MAP::const_iterator iter = child_note->leaf_node_map_.find(leaf_key);
    //multimap�����ϲ������ʧ��
    if (child_note->leaf_node_map_.end() == iter)
    {
        return -1;
    }

    leaf_str_ptr = &iter->second;

    return 0;
}

int ZCE_Conf_PropertyTree::get_leafptr(const std::string &path_str,
                                       std::string *& leaf_str_ptr)
{
    int ret = 0;
    leaf_str_ptr = NULL;
    size_t last_key_pos = path_str.find_last_not_of(SEPARATOR_STRING);

    std::string tree_path, leaf_key;
    PROPERTY_TREE_NODE *child_note = NULL;
    //���û�зָ����ţ�
    if (std::string::npos != last_key_pos)
    {
        tree_path.assign(path_str.cbegin(), path_str.cbegin() + last_key_pos);
        leaf_key.assign(path_str.cbegin() + last_key_pos + 1, path_str.cend());

        ret = get_child(tree_path, child_note);
        if (0 != ret)
        {
            return ret;
        }
    }
    else
    {
        leaf_key = path_str;
        child_note = this;
    }

    KEY_VALUE_MAP::iterator iter = child_note->leaf_node_map_.find(leaf_key);
    //multimap�����ϲ������ʧ��
    if (child_note->leaf_node_map_.end() == iter)
    {
        return -1;
    }

    leaf_str_ptr = &iter->second;

    return 0;
}

//����һ��CHILD
int ZCE_Conf_PropertyTree::put_child(const std::string &path_str,
                                     const std::string &new_child_name,
                                     const PROPERTY_TREE_NODE &new_child_note)
{
    PROPERTY_TREE_NODE *child_note = NULL;
    int ret = get_child(path_str, child_note);

    if (0 != ret)
    {
        return ret;
    }

    PROPERTY_TREE_MAP::iterator iter = child_note->child_node_map_.insert(
                                           std::pair<std::string, PROPERTY_TREE_NODE>(new_child_name, new_child_note));

    //multimap�����ϲ������ʧ��
    if ( child_note->child_node_map_.end() == iter )
    {
        return -1;
    }

    return 0;
}

//����һ���µ�CHILD,��Ȼ����ȫ������ΪNULL
int ZCE_Conf_PropertyTree::new_child(const std::string &path_str,
                                     const std::string &new_child_name)
{
    PROPERTY_TREE_NODE null_node;
    return put_child(path_str, new_child_name, null_node);

}

//�������ģ�庯���ػ��ˣ�Ҳ������ʲô�Ҹ�����������أ�

template<>
int ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                    std::string &val) const
{

    const std::string *leaf_str_ptr = nullptr;
    int ret = get_leafptr(path_str, leaf_str_ptr);

    if (0 != ret)
    {
        return ret;
    }

    val = *leaf_str_ptr;
    return 0;
}

//ȡ��һ��Ҷ�ӽڵ�����ݣ�����ֵ��char *
template<>
int  ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                     std::pair<char *, size_t > &str_data) const
{
    std::string value_data;
    int ret = get_leaf<std::string>(path_str, value_data);

    if (0 != ret)
    {
        return ret;
    }

    strncpy(str_data.first, value_data.c_str(), str_data.second);
    return 0;
}

//ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������int,
template<>
int ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                    int32_t &val) const
{
    std::string value_str;
    int ret = get_leaf<std::string>(path_str, value_str);

    if (0 != ret)
    {
        return ret;
    }

    val = std::stoi(value_str);
    return 0;
}

template<>
int ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                    uint32_t &val) const
{
    std::string value_str;
    int ret = get_leaf<std::string>(path_str, value_str);

    if (0 != ret)
    {
        return ret;
    }

    val = static_cast<uint32_t>(std::stoul(value_str));
    return 0;
}



template<>
int ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                    int64_t &val) const
{
    std::string value_str;
    int ret = get_leaf<std::string>(path_str, value_str);

    if (0 != ret)
    {
        return ret;
    }

    val = std::stoll(value_str);
    return 0;
}

template<>
int ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                    uint64_t &val) const
{
    std::string value_str;
    int ret = get_leaf<std::string>(path_str, value_str);

    if (0 != ret)
    {
        return ret;
    }

    val = std::stoull(value_str);
    return 0;
}

//ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������bool
template<>
int ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                    bool &val) const
{
    val = false;

    std::string value_str;
    int ret = get_leaf<std::string>(path_str, value_str);

    if (0 != ret)
    {
        return ret;
    }

    if (0 == strcasecmp("TRUE", value_str.c_str()))
    {
        val = true;
    }
    else if (1 == atoi(value_str.c_str()))
    {
        val = true;
    }

    return ret;
}


///ȡ��IPV6�ĵ�ַ
template<>
int ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                    ZCE_Sockaddr_In &val) const
{
    std::string value_str;
    int ret = get_leaf<std::string>(path_str, value_str);

    if (0 != ret)
    {
        return ret;
    }
    val.set(value_str.c_str());
    return 0;
}

///IPV6�ĵ�ַ
template<>
int ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                    ZCE_Sockaddr_In6 &val) const
{
    std::string value_str;
    int ret = get_leaf<std::string>(path_str, value_str);

    if (0 != ret)
    {
        return ret;
    }
    val.set(value_str.c_str());
    return 0;
}


//ȡ��ʱ��
template<>
int ZCE_Conf_PropertyTree::get_leaf(const std::string &path_str,
                                    ZCE_Time_Value &val) const
{
    std::string value_str;
    int ret = get_leaf<std::string>(path_str, value_str);

    if (0 != ret)
    {
        return ret;
    }
    val.from_string(value_str.c_str(), false, ZCE_OS::TIME_STRFMT_US_SEC);
    return 0;
}



//����һ��Ҷ�ӽڵ㣬
template<>
int ZCE_Conf_PropertyTree::put_leaf(const std::string &path_str,
                                    const std::string &key_str,
                                    const std::string &value_data)
{
    PROPERTY_TREE_NODE *child_note = NULL;
    int ret = get_child(path_str, child_note);

    if (0 != ret)
    {
        return ret;
    }

    KEY_VALUE_MAP::iterator iter = child_note->leaf_node_map_.insert(
                                       std::pair<std::string, std::string>(key_str, value_data));

    //multimap�����ϲ������ʧ��
    if ( child_note->leaf_node_map_.end() == iter )
    {
        return -1;
    }

    return 0;
}

//
template<>
int ZCE_Conf_PropertyTree::put_leaf(const std::string &path_str,
                                    const std::string &key_str,
                                    int value_int)
{
    const size_t BUF_LEN = 24;
    char str_int[BUF_LEN + 1];
    str_int[BUF_LEN] = '\0';
    snprintf(str_int, BUF_LEN, "%d", value_int);
    return put_leaf<std::string>(path_str, key_str, str_int);
}

//
template<>
int ZCE_Conf_PropertyTree::put_leaf(const std::string &path_str,
                                    const std::string &key_str,
                                    bool value_bool)
{
    if (value_bool)
    {
        return put_leaf<std::string>(path_str, key_str, "TRUE");
    }
    else
    {
        return put_leaf<std::string>(path_str, key_str, "FALSE");
    }

}

//���÷ָ����
void ZCE_Conf_PropertyTree::set_separator(char separator_char)
{
    SEPARATOR_STRING[0] = separator_char;
}

