#ifndef ZCE_LIB_BYTES_SERIALIZATION_H_
#define ZCE_LIB_BYTES_SERIALIZATION_H_

#include "zce_bytes_base.h"
#include "zce_trace_log_debug.h"

//===========================================================================================================
//�����봦�����

class ZCE_Serialized_Save;

//�������������ݵ�һЩ��
template<typename val_type >
class ZCE_ClassSave_Help
{
public:
    void save_help(ZCE_Serialized_Save *ssave, const val_type &val);
};

template<>
class ZCE_ClassSave_Help<std::string>
{
public:
    void save_help(ZCE_Serialized_Save *ssave, const std::string &val);
};

template<typename vector_type >
class ZCE_ClassSave_Help<std::vector<vector_type> >
{
public:
    void save_help(ZCE_Serialized_Save *ssave, const std::vector<vector_type> &val);
};

template<typename list_type >
class ZCE_ClassSave_Help<std::list<list_type> >
{
public:
    void save_help(ZCE_Serialized_Save *ssave, const std::list<list_type> &val);
};

template<typename key_type,typename data_type >
class ZCE_ClassSave_Help<std::map<key_type,data_type> >
{
public:
    void save_help(ZCE_Serialized_Save *ssave, const std::map<key_type, data_type> &val);
};


/*!
* @brief      �����ݽ��б��봦����࣬�����ݱ������
*
* @note       û�п��Ƕ�������⣬
*             BTW������д�룬���ǲ������������������Ŭ���������㸺�������
*/
class ZCE_Serialized_Save  
{
public:

    ///���캯��
    ZCE_Serialized_Save(char *write_buf, size_t buf_len);

    ~ZCE_Serialized_Save();

public:

    ///���ص�ǰ���Ƿ�������
    inline bool is_good()
    {
        return is_good_;
    }

    ///���ô����־
    inline void set_bad()
    {
        is_good_ = false;
    }

    ///����
    void reset();

    ///д��һ������,ֻ���ػ���������
    template<typename val_type>
    bool save(const val_type &/*val*/)
    {
        return false;
    }

    ///����ö��ֵ
    template<typename val_type  >
    void save(const typename std::enable_if<std::is_enum<val_type>::value, val_type>::type &val)
    {
        return save_enum(val);
    }
    template<typename enum_type >
    void save_enum(const enum_type &val)
    {
        save_arithmetic(static_cast<const int &>(val));
    }

    ///������ֵ����
    template<typename val_type >
    void save(const typename std::enable_if<std::is_arithmetic<val_type>::value, val_type>::type &val)
    {
        return save_arithmetic(val);
    }
    void save_arithmetic(const bool &val);
    void save_arithmetic(const char &val);
    void save_arithmetic(const unsigned char &val);
    void save_arithmetic(const short &val);
    void save_arithmetic(const unsigned short &val);
    void save_arithmetic(const int &val);
    void save_arithmetic(const unsigned int &val);
    void save_arithmetic(const int64_t &val);
    void save_arithmetic(const uint64_t &val);
    void save_arithmetic(const float &val);
    void save_arithmetic(const double &val);

    ///��������
    template<typename val_type >
    void save(const typename std::enable_if<std::is_array<val_type>::value, val_type>::type &val)
    {
        // consider alignment
        std::size_t count = sizeof(val) / (
            static_cast<const char *>(static_cast<const void *>(&val[1]))
            - static_cast<const char *>(static_cast<const void *>(&val[0]))
            );
        return save_array(val, count);
    }
    template<typename array_type >
    void save_array(const array_type *ary, size_t count)
    {
        //ZCE_ASSERT(count < std::numeric_limits<unsigned int>::max());
        ZCE_ASSERT(count < 0xFFFFFFFFll);
        this->save_arithmetic(static_cast<unsigned int>(count));
        for (size_t i = 0; i < count && is_good_;++i)
        {
            this->save(*(ary+i));
        }
        return;
    }
    ///�ػ������ַ������м���
    template<>
    void save_array(const char *ary, size_t count)
    {
        ZCE_ASSERT(count < 0xFFFFFFFFll);
        this->save_arithmetic(static_cast<unsigned int>(count));
        if (is_good_)
        {
            if (write_pos_ + count < end_pos_)
            {
                is_good_ = false;
                return;
            }
            memcpy(write_pos_, ary, count);
            write_pos_ += count;
        }
    }
    template<>
    void save_array(const unsigned char *ary, size_t count)
    {
        ZCE_ASSERT(count < 0xFFFFFFFFll);
        this->save_arithmetic(static_cast<unsigned int>(count));
        if (is_good_)
        {
            if (write_pos_ + count < end_pos_)
            {
                is_good_ = false;
                return;
            }
            memcpy(write_pos_, ary, count);
            write_pos_ += count;
        }
    }

    ///�����࣬���Ҫ��һЩƫ�ػ�������
    template<typename val_type >
    void save(const typename std::enable_if<std::is_class<val_type>::value, val_type>::type &val)
    {
        ZCE_ClassSave_Help<val_type> ssave;
        ssave.save_help<val_type>(this, val);
        return;
    }

    ///ʹ��<<��������д�����ݣ���Ҫ�Ǳ����ⲿ����ͳһʹ��<<������.�ⲿ�����������ĺ���
    ///ZCE_Serialized_Save& operator <<(ZCE_Serialized_Save &dr_encode,const val_type &val);
    ///bool operator <<(ZCE_Serialized_Save &dr_encode,const val_type &val);
    template<typename val_type>
    ZCE_Serialized_Save & operator &(val_type val)
    {
        this->save<val_type>(val);
        return *this;
    }

protected:

    ///״̬�Ƿ���ȷ�����д��λ�ó����������Ľ�β������Ϊfalse
    bool is_good_;

    ///�������ݣ���д��buffer��
    char *write_buf_;
    ///buf�ĳ���
    size_t buf_len_;

    ///buf������λ�ã������жϼӿ촦��
    char *end_pos_;
    ///��ǰд���posλ�ã�
    char *write_pos_;
};

//�����࣬save_help ����
//��������д���黯
template<typename val_type>
void ZCE_ClassSave_Help<val_type>::save_help(ZCE_Serialized_Save *ssave,
    const val_type &val)
{
    val.serialize(ssave);
}

void ZCE_ClassSave_Help<std::string>::save_help(ZCE_Serialized_Save *ssave, 
    const std::string &val)
{
    ssave->save_array(val.c_str(), val.length());
}

template<typename vector_type>
void ZCE_ClassSave_Help<std::vector<vector_type> >::save_help(ZCE_Serialized_Save *ssave, 
    const std::vector<vector_type> &val)
{
    size_t v_size = val.size();
    this->save_arithmetic<unsigned int>(v_size);
    for (size_t i = 0; i < v_size && is_good_; ++i)
    {
        this->save<vector_type>(val[i]);
    }
    return;
}

template<typename list_type>
void ZCE_ClassSave_Help<std::list<list_type> >::save_help(ZCE_Serialized_Save *ssave,
    const std::list<list_type> &val)
{
    size_t v_size = val.size();
    this->save_arithmetic<unsigned int>(v_size);
    for (size_t i = 0; i < v_size && is_good_; ++i)
    {
        this->save<list_type>(val[i]);
    }
    return;
}


template<typename key_type, typename data_type >
void ZCE_ClassSave_Help<std::map<key_type, data_type> >::save_help(ZCE_Serialized_Save *ssave,
    const std::map<key_type, data_type> &val)
{
    size_t v_size = val.size();
    this->save_arithmetic<unsigned int>(v_size);
    for (size_t i = 0; i < v_size && is_good_; ++i)
    {
        this->save<list_type>(val[i]);
    }
    return;
}

//===========================================================================================================

/*!
* @brief      �����ݽ��н����봦����࣬����������ݣ�
*
* @note       ��ȡ���ڱ߽�����һЩ��ȫ���������������ݾ�����������
*/
class ZCE_Serialized_Load
{
public:


    /*!
    * @brief      ���캯��
    * @param      read_buf ��������ݣ���������ݽ��иĶ�
    * @param      buf_len  ���ݵĳ���
    */
    ZCE_Serialized_Load(const char *read_buf, size_t buf_len);

    ///��������
    ~ZCE_Serialized_Load();

public:
    ///���ص�ǰ���Ƿ�������BTW�����ǲ��������������Ŭ���������㸺�������
    inline bool is_good();

    ///���ô����־
    inline void set_bad();

    ///���ÿ�ʼλ�ú�good��־λ
    void reset();

    ///д��һ������,ֻ���ػ���������
    template<typename val_type>
    bool read(val_type *val);

    ///ʹ��>>��������д�����ݣ���Ҫ�Ǳ����ⲿ����ͳһʹ��<<������,�ⲿ�����������ĺ���
    ///ZCE_Serialized_Load& operator >>(ZCE_Serialized_Load &dr_encode,val_type *val);
    ///bool operator >>(ZCE_Serialized_Load &dr_encode,val_type *val);
    template<typename val_type>
    ZCE_Serialized_Load &operator >>(val_type *val);

    /*!
    * @brief      ��ȡһ������
    * @tparam        ary_type ������������
    * @return        ZCE_Serialized_Save& �����Լ�������
    * @param[out]    ary       ��������
    * @param[in,out] ary_size  ���鳤��,������������������ʾary�ĳ��ȣ������ʾʵ�ʶ�ȡ�ĳ���
    */
    template<typename ary_type>
    bool read_array(ary_type *ary, size_t *ary_size);


    /*!
    * @brief      ����һ��vector
    * @tparam     vector_type vector��������
    * @return     ZCE_Serialized_Load& �����Լ�������
    * @param[out]    vector_data  ��ȡ���ص�vector
    * @param[in,out] ary_size     �����ʾԤ�ڵ����vector size�������ʾʵ�ʳߴ�
    */
    template<typename vector_type>
    bool read_vector(std::vector<vector_type> *vector_data, size_t *ary_size);

protected:

    ///״̬�Ƿ���ȷ�����д��λ�ó����������Ľ�β������Ϊfalse
    bool is_good_;

    ///��ȡ����BUFFER
    const char *read_buf_;
    ///BUF�ĳ���
    size_t buf_len_;

    ///BUFFER������λ��
    const char *end_pos_;
    ///��ǰ��ȡ��λ��
    const char *read_pos_;

};

inline bool ZCE_Serialized_Load::is_good()
{
    return is_good_;
}

inline void ZCE_Serialized_Load::set_bad()
{
    is_good_ = false;
}

template<typename vector_type>
bool ZCE_Serialized_Load::read_vector(std::vector<vector_type> *vector_data, size_t *vector_size)
{
    unsigned int read_ary_size = 0;
    if (!this->read<unsigned int>(&read_ary_size))
    {
        return false;
    }
    if (read_ary_size > *vector_size)
    {
        is_good_ = false;
        return false;
    }
    vector_size = read_ary_size;
    vector_data->resize(read_ary_size);

    vector_type read_data;
    for (unsigned int i = 0; i < read_ary_size; ++i)
    {
        this->read<vector_type>(read_data);
        (*vector_data)[i] = read_data;
    }
    if (false == is_good_)
    {
        return false;
    }
    return true;
}


template<typename val_type>
ZCE_Serialized_Load &ZCE_Serialized_Load::operator >> (val_type *val)
{
    this->read<val_type>(val);
    return *this;
}


#endif //ZCE_LIB_BYTES_SERIALIZATION_H_

