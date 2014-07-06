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

//
//
class ZCE_Sockaddr_In;
class ZCE_Sockaddr_In6;
class ZCE_Time_Value;

//�Լ���������һ���ڵ�
class ZCE_Conf_PropertyTree;
//self
typedef ZCE_Conf_PropertyTree                        PROPERTY_TREE_NODE;



/*!
* @brief      �����ļ���ȡ���ŵ���
*             ����������ʵ��Ŀ����ʵ������BOOST ��Property Tree �� ACE_Configuration_Heap
*             ���Ŀ�꣬��������Ϣ��֯��һ�������
*             ������ȡ���߶�ȡ�������ݺ����ɷ���ZCE_Conf_PropertyTree�У�
*             ˼·���Ǻ�Property Tree��һ�����
* @note
*/
class ZCE_Conf_PropertyTree
{
protected:

    //Ҷ�ӽڵ��MAP����
    typedef std::multimap<std::string, std::string>           KEY_VALUE_MAP;

    //������MAP����
    typedef std::multimap<std::string, PROPERTY_TREE_NODE>    PROPERTY_TREE_MAP;

    //
public:

    ///���캯��
    ZCE_Conf_PropertyTree();
    ~ZCE_Conf_PropertyTree();

    ///����·���õ�һ��CHILD
    int get_child(const std::string &path_str,
                  const PROPERTY_TREE_NODE *& const_child) const;

    //
    int get_child(const std::string &path_str,
                  PROPERTY_TREE_NODE *& child_data);

    ///����һ��CHILD
    int put_child(const std::string &path_str,
                  const std::string &new_child_name,
                  const PROPERTY_TREE_NODE &value_data);

    ///����һ���µ�CHILD,��Ȼ����ȫ������ΪNULL
    int new_child(const std::string &path_str,
                  const std::string &new_child_name);

    ///���������ػ���ģ��ߵ���һ�麯��,ģ�庯��,ֻ���岻ʵ��
    template<typename val_type>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 val_type &val) const;

    /*!
    * @brief      ȡ��һ��Ҷ�ӽڵ������,ȡ��������srting
    * @return     int == 0 ��ʾ�ɹ�
    * @param      path_str ·������|��ʾһ������
    * @param      key_data Ҷ�ӽڵ��key
    * @param      val      ���ص���ֵ
    */
    template<>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 std::string &val) const;

    /*!
    * @brief      ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������char *
    * @return     int == 0 ��ʾ�ɹ�
    * @param      path_str
    * @param      key_data
    * @param      val      ��һ��pair�������ʱ��first �ַ���ָ�룬second���ַ����Ŀռ䳤��
    */
    template<>
    int  get_leaf(const std::string &path_str,
                  const std::string &key_data,
                  std::pair<char *, size_t > &val) const;

    /*!
    * @brief      ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������int32_t,֧��16����,8����д��
    * @return     int == 0 ��ʾ�ɹ�
    * @param      path_str
    * @param      key_data
    * @param      val      ���ص���ֵ
    * @note       �����8���Ƶ�д��������0��ͷ�������16����д��������0x��ͷ
    */
    template<>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 int32_t &val) const;

    ///ͬ�ϣ������ǵõ�һ���޷���32λ����������
    template<>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 uint32_t &val) const;

    ///ͬ�ϣ������ǵõ�һ���з���64λ����������
    template<>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 int64_t &val) const;

    ///ͬ�ϣ������ǵõ�һ���޷���64λ����������
    template<>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 uint64_t &val) const;

    ///ȡ��һ��Ҷ�ӽڵ�����ݣ�ȡ��������bool
    template<>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 bool &val) const;

    /*!
    * @brief      ȡ��IPV6�ĵ�ַ
    * @return     int      == 0 ��ʾ�ɹ�
    * @param      path_str ·������|��ʾһ������
    * @param      key_data
    * @param      val      �õ���IP��ַ������ַ���������#��������϶˿ںţ�Ҳ�����ת��
    */
    template<>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 ZCE_Sockaddr_In &val) const;

    ///ȡ��IPV6�ĵ�ַ��
    template<>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 ZCE_Sockaddr_In6 &val) const;

    ///ʱ����ַ�����ʹ��ISO�ĸ�ʽ
    template<>
    int get_leaf(const std::string &path_str,
                 const std::string &key_data,
                 ZCE_Time_Value &val) const;


    ///���������ػ���ģ��ߵ���һ�麯��,ģ�庯��,ֻ���岻ʵ��
    template<typename val_type>
    int put_leaf(const std::string &path_str,
                 const std::string &key_data,
                 val_type val);


    //����һ��Ҷ�ӽڵ㣬string
    template<>
    int put_leaf(const std::string &path_str,
                 const std::string &key_data,
                 const std::string &value_data);

    //����һ��Ҷ�ӽڵ㣬int
    template<>
    int put_leaf(const std::string &path_str,
                 const std::string &key_data,
                 int value_int);

    //����һ��Ҷ�ӽڵ㣬bool
    template<>
    int put_leaf(const std::string &path_str,
                 const std::string &key_data,
                 bool value_bool);

protected:

    ///Ҷ�ӽڵ��MAP
    KEY_VALUE_MAP         leaf_node_map_;

    ///�����ڵ��MAP
    PROPERTY_TREE_MAP     child_node_map_;

};

#endif //ZCE_LIB_CONFIG_PROPERTY_H_

