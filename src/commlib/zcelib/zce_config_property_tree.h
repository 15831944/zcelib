/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_config_property_tree.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       2011��10��5��
* @brief      ����IPhone 5û�з��������Ƿ�����һ��Iphone 4S,�������е�ʧ��
*             
*             
* @details    
*             
*             
*             
* @note       
*             
*/


#ifndef ZCE_LIB_CONFIG_PROPERTY_TREE_H_
#define ZCE_LIB_CONFIG_PROPERTY_TREE_H_

//����������ʵ��Ŀ����ʵ������BOOST ��Property Tree �� ACE_Configuration_Heap ���Ŀ�꣬
//��������Ϣ��֯��һ�������
//������ȡ���߶�ȡ�������ݺ����ɷ���ZenConfigPropertyTree�У�
//˼·���Ǻ�Property Tree��һ�����
//�Լ���������һ���ڵ�
class ZCE_Conf_PropertyTree;
//self
typedef ZCE_Conf_PropertyTree                        PROPERTY_TREE_NODE;

/******************************************************************************************
class ZCE_INI_Implemention �����ļ���ȡ���ŵĵط�
******************************************************************************************/
class ZCE_Conf_PropertyTree
{
protected:

    //
    typedef std::multimap<std::string, std::string>           KEY_VALUE_MAP;

    //
    typedef std::multimap<std::string, PROPERTY_TREE_NODE>    PROPERTY_TREE_MAP;

protected:

    //Ҷ�ӽڵ��
    KEY_VALUE_MAP         leaf_node_map_;

    //�����ڵ��
    PROPERTY_TREE_MAP     child_node_map_;

    //
public:

    //���캯��
    ZCE_Conf_PropertyTree();
    ~ZCE_Conf_PropertyTree();

    //����·���õ�һ��CHILD
    int get_child(const std::string &path_str,
                  const PROPERTY_TREE_NODE *& const_child) const;

    //
    int get_child(const std::string &path_str,
                  PROPERTY_TREE_NODE *& child_data);

    //����һ��CHILD
    int put_child(const std::string &path_str,
                  const std::string &new_child_name,
                  const PROPERTY_TREE_NODE &value_data);

    //����һ���µ�CHILD,��Ȼ����ȫ������ΪNULL
    int new_child(const std::string &path_str,
                  const std::string &new_child_name);

    //����������ģ�溯��,���������һ�û��BOOST�������ۡ�

    //ȡ��һ��Ҷ�ӽڵ������,ȡ��������srting
    int get_leaf_str(const std::string &path_str,
                     const std::string &key_data,
                     std::string &value_data) const;

    //ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������char *
    int  get_leaf_str(const std::string &path_str,
                      const std::string &key_data,
                      char *&str_data,
                      size_t max_str_len) const;

    //ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������int,
    int get_leaf_int(const std::string &path_str,
                     const std::string &key_data,
                     int32_t &value_int) const;

    int get_leaf_uint(const std::string &path_str,
                      const std::string &key_data,
                      uint32_t &value_uint) const;

    //ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������bool
    int get_leaf_bool(const std::string &path_str,
                      const std::string &key_data,
                      bool &value_bool) const;

    //����һ��Ҷ�ӽڵ㣬string
    int put_leaf_str(const std::string &path_str,
                     const std::string &key_data,
                     const std::string &value_data);

    //����һ��Ҷ�ӽڵ㣬int
    int put_leaf_int(const std::string &path_str,
                     const std::string &key_data,
                     int value_int);

    //����һ��Ҷ�ӽڵ㣬bool
    int put_leaf_bool(const std::string &path_str,
                      const std::string &key_data,
                      bool value_bool);

    ////ȡ��һ��Ҷ�ӽڵ�����ݣ����û�����ݣ�����Ĭ��ֵ
    //int get_leaf(const std::string &path_str,
    //    std::string &value_data,
    //    const std::string &defualt_value);

    //ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������int,���û�����ݣ�����Ĭ��ֵ
    //int get_leaf(const std::string &path_str,
    //    int &value_int,
    //    const int defualt_int);

    //ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������char *,���û�����ݣ�����Ĭ��ֵ
    //int get_leaf(const std::string &path_str,
    //    char *&str_data,
    //    size_t str_len,
    //    const char *defualt_str);
};

#endif //ZCE_LIB_CONFIG_PROPERTY_H_

