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




//�õ������ڵ㣬
int ZCE_Conf_PropertyTree::path_get_child(const std::string &path_str,
                                          ZCE_Conf_PropertyTree *& child_data)
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

    CHILDREN_NOTE_TYPE::iterator iter_tmp = child_node_.begin();
    for (; iter_tmp != child_node_.end(); ++iter_tmp)
    {
        if (iter_tmp->first == start_str)
        {
            break;
        }
    }
    //���û���ҵ�
    if (child_node_.end() == iter_tmp)
    {
        return -1;
    }

    ZCE_Conf_PropertyTree *child_tree = &(iter_tmp->second);

    //����·�������еݹ��ѯ
    if (str_pos != std::string::npos)
    {
        std::string remain_str(path_str, str_pos + 1);
        return child_tree->path_get_child(remain_str, child_data);
    }
    else
    {
        //����޷�����ǰ������,
        child_data = child_tree;
        return 0;
    }
}

//�õ������ڵ㣬const
int ZCE_Conf_PropertyTree::path_get_child(const std::string &path_str,
                                          const ZCE_Conf_PropertyTree *& child_data) const
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

    CHILDREN_NOTE_TYPE::const_iterator iter_tmp = child_node_.begin();
    for (; iter_tmp != child_node_.end(); ++iter_tmp)
    {
        if (iter_tmp->first == start_str)
        {
            break;
        }
    }
    //���û���ҵ�
    if (child_node_.end() == iter_tmp)
    {
        return -1;
    }

    const ZCE_Conf_PropertyTree *child_tree = &(iter_tmp->second);

    //����·�������еݹ��ѯ
    if (str_pos != std::string::npos)
    {
        std::string remain_str(path_str, str_pos + 1);
        return child_tree->path_get_child(remain_str, child_data);
    }
    else
    {
        //����޷�����ǰ������,
        child_data = child_tree;
        return 0;
    }
}


//
int ZCE_Conf_PropertyTree::path_get_leafptr(const std::string &path_str,
                                            const std::string &key_str,
                                            std::string *& str_ptr)
{
    int ret = 0;

    ZCE_Conf_PropertyTree *child_note = NULL;
    ret = path_get_child(path_str, child_note);
    if (0 != ret)
    {
        return ret;
    }


    ret = child_note->get_leafptr(key_str, str_ptr);
    if (0 != ret)
    {
        return ret;
    }

    return 0;
}

//
int ZCE_Conf_PropertyTree::path_get_leafptr(const std::string &path_str,
                                            const std::string &key_str,
                                            const std::string *& str_ptr) const
{
    int ret = 0;

    const ZCE_Conf_PropertyTree *child_note = NULL;
    ret = path_get_child(path_str, child_note);
    if (0 != ret)
    {
        return ret;
    }


    ret = child_note->get_leafptr(key_str, str_ptr);
    if (0 != ret)
    {
        return ret;
    }

    return 0;
}


//�����Լ��������ͨ��key����Ҷ�ӵ�map��ȡ�ö�Ӧ��string��ָ��
int ZCE_Conf_PropertyTree::get_leafptr(const std::string &key_str,
    std::string *&str_ptr)
{
    LEAF_NOTE_TYPE::iterator iter = leaf_node_.find(key_str);
    if (leaf_node_.end() == iter)
    {
        return -1;
    }
    str_ptr = &(iter->second);
    return 0;
}

//ͬ�ϣ�ֻ��const��
int ZCE_Conf_PropertyTree::get_leafptr(const std::string &key_str,
    const std::string *&str_ptr) const
{
    LEAF_NOTE_TYPE::const_iterator iter = leaf_node_.find(key_str);
    if (leaf_node_.end() == iter)
    {
        return -1;
    }
    str_ptr = &(iter->second);
    return 0;
}


//�������ģ�庯���ػ��ˣ�Ҳ������ʲô�Ҹ�����������أ�

/*!
* @brief      ȡ��һ��Ҷ�ӽڵ������,ȡ��������srting
* @return     int == 0 ��ʾ�ɹ�
* @param      path_str ·������.��ʾһ������
* @param      val      ���ص���ֵ
*/
template<>
int ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                         const std::string &key_str,
                                         std::string &val) const
{

    const std::string *leaf_str_ptr = nullptr;
    int ret = path_get_leafptr(path_str, key_str, leaf_str_ptr);

    if (0 != ret)
    {
        return ret;
    }

    val = *leaf_str_ptr;
    return 0;
}


/*!
* @brief      ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������char *
* @return     int == 0 ��ʾ�ɹ�
* @param      path_str
* @param      val      ��һ��pair�������ʱ��first �ַ���ָ�룬second���ַ����Ŀռ䳤��
*/
template<>
int  ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                          const std::string &key_str,
                                          std::pair<char *, size_t > &str_data) const
{
    std::string value_data;
    int ret = path_get_leaf<std::string>(path_str, key_str, value_data);

    if (0 != ret)
    {
        return ret;
    }

    strncpy(str_data.first, value_data.c_str(), str_data.second);
    return 0;
}


/*!
* @brief      ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������int32_t,֧��16����,8����д��
* @return     int == 0 ��ʾ�ɹ�
* @param      path_str
* @param      val      ���ص���ֵ
* @note       �����8���Ƶ�д��������0��ͷ�������16����д��������0x��ͷ
*/
template<>
int ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                         const std::string &key_str,
                                         int32_t &val) const
{
    std::string value_str;
    int ret = path_get_leaf<std::string>(path_str, key_str, value_str);

    if (0 != ret)
    {
        return ret;
    }

    val = std::stoi(value_str);
    return 0;
}

///ͬ�ϣ������ǵõ�һ���޷���32λ����������
template<>
int ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                         const std::string &key_str,
                                         uint32_t &val) const
{
    std::string value_str;
    int ret = path_get_leaf<std::string>(path_str, key_str, value_str);

    if (0 != ret)
    {
        return ret;
    }

    val = static_cast<uint32_t>(std::stoul(value_str));
    return 0;
}

///ͬ�ϣ������ǵõ�һ���з���64λ����������
template<>
int ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                         const std::string &key_str,
                                         int64_t &val) const
{
    std::string value_str;
    int ret = path_get_leaf<std::string>(path_str, key_str, value_str);

    if (0 != ret)
    {
        return ret;
    }

    val = std::stoll(value_str);
    return 0;
}

///ͬ�ϣ������ǵõ�һ���޷���64λ����������
template<>
int ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                         const std::string &key_str,
                                         uint64_t &val) const
{
    std::string value_str;
    int ret = path_get_leaf<std::string>(path_str, key_str, value_str);

    if (0 != ret)
    {
        return ret;
    }

    val = std::stoull(value_str);
    return 0;
}

///ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������bool
template<>
int ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                         const std::string &key_str,
                                         bool &val) const
{
    val = false;

    std::string value_str;
    int ret = path_get_leaf<std::string>(path_str, key_str, value_str);

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

/*!
* @brief      ȡ��IPV4�ĵ�ַ
* @return     int      == 0 ��ʾ�ɹ�
* @param      path_str ·������|��ʾһ������
* @param      val      �õ���IP��ַ������ַ���������#��������϶˿ںţ�Ҳ�����ת��
*/
template<>
int ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                         const std::string &key_str,
                                         ZCE_Sockaddr_In &val) const
{
    std::string value_str;
    int ret = path_get_leaf<std::string>(path_str, key_str, value_str);

    if (0 != ret)
    {
        return ret;
    }
    val.set(value_str.c_str());
    return 0;
}

///IPV6�ĵ�ַ
template<>
int ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                         const std::string &key_str,
                                         ZCE_Sockaddr_In6 &val) const
{
    std::string value_str;
    int ret = path_get_leaf<std::string>(path_str, key_str, value_str);

    if (0 != ret)
    {
        return ret;
    }
    val.set(value_str.c_str());
    return 0;
}

///ȡ��ʱ��
template<>
int ZCE_Conf_PropertyTree::path_get_leaf(const std::string &path_str,
                                         const std::string &key_str,
                                         ZCE_Time_Value &val) const
{
    std::string value_str;
    int ret = path_get_leaf<std::string>(path_str, key_str, value_str);

    if (0 != ret)
    {
        return ret;
    }
    val.from_string(value_str.c_str(), false, ZCE_OS::TIME_STRFMT_US_SEC);
    return 0;
}

//����һ���µ�CHILD,��Ȼ����ȫ������ΪNULL
void ZCE_Conf_PropertyTree::add_child(const std::string &key_str,
                                      ZCE_Conf_PropertyTree *&new_child_note)
{
    ZCE_Conf_PropertyTree null_node;
    CHILDREN_NOTE_TYPE::iterator iter =
        this->child_node_.insert(std::make_pair(key_str, null_node));

    new_child_note = &(iter->second);
    return;
}

//
template<>
void ZCE_Conf_PropertyTree::set_leaf(const std::string &key_str,
    const std::string &val_str)
{
    this->leaf_node_.insert(std::make_pair(key_str,val_str));
}

template<>
void ZCE_Conf_PropertyTree::set_leaf(const std::string &key_str,
    std::string &val_str)
{
    this->leaf_node_.insert(std::make_pair(key_str, val_str));
}

template<>
void ZCE_Conf_PropertyTree::set_leaf(const std::string &key_str,
    const char *val_str)
{
    this->leaf_node_.insert(std::make_pair(key_str, std::string(val_str)));
}

template<>
void ZCE_Conf_PropertyTree::set_leaf(const std::string &key_str,
    char *val_str)
{
    this->leaf_node_.insert(std::make_pair(key_str, std::string(val_str)));
}

//
template<>
void ZCE_Conf_PropertyTree::set_leaf(const std::string &key_str,
    int value_int)
{
    const size_t BUF_LEN = 24;
    char str_int[BUF_LEN + 1];
    str_int[BUF_LEN] = '\0';
    snprintf(str_int, BUF_LEN, "%d", value_int);
    return set_leaf(key_str,str_int);
}

//
template<>
void ZCE_Conf_PropertyTree::set_leaf(const std::string &key_str,
    bool value_bool)
{
    if (value_bool)
    {
        return set_leaf<const char *>(key_str, "TRUE");
    }
    else
    {
        return set_leaf<const char *>(key_str, "FALSE");
    }

}

//���÷ָ����
void ZCE_Conf_PropertyTree::set_separator(char separator_char)
{
    SEPARATOR_STRING[0] = separator_char;
}

