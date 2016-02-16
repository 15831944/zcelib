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

    ///����,���ÿ�ʼλ�ú�good��־λ
    inline void reset()
    {
        write_pos_ = write_buf_;
        is_good_ = true;
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

    ///�����࣬���Ҫ�ø�����ʵ��һЩƫ�ػ�������
    template<typename val_type >
    void save(const typename std::enable_if<std::is_class<val_type>::value, val_type>::type &val)
    {
        ZCE_ClassSave_Help<val_type> ssave;
        ssave.save_help<val_type>(this, val);
        return;
    }

    ///д��һ������,ֻ���ػ���������
    template<typename val_type>
    void save(const val_type &/*val*/)
    {
        ZCE_ASSERT(false);
        return;
    }

    ///ʹ��& ��������д�����ݣ�
    template<typename val_type>
    ZCE_Serialized_Save & operator &(const val_type &val)
    {
        this->save<const val_type &>(val);
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
template<typename val_type>
void ZCE_ClassSave_Help<val_type>::save_help(ZCE_Serialized_Save *ssave,
    const val_type &val)
{
    val.serialize(ssave);
}



template<typename vector_type>
void ZCE_ClassSave_Help<std::vector<vector_type> >::save_help(ZCE_Serialized_Save *ssave, 
    const std::vector<vector_type> &val)
{
    size_t v_size = val.size();
    ssave->save_arithmetic<unsigned int>(v_size);
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
    ssave->save_arithmetic<unsigned int>(v_size);
    std::list<list_type>::iterator iter= val.begin();
    for (size_t i = 0; i < v_size && is_good_; ++i, ++iter)
    {
        this->save<list_type>(*iter);
    }
    return;
}


template<typename key_type, typename data_type >
void ZCE_ClassSave_Help<std::map<key_type, data_type> >::save_help(ZCE_Serialized_Save *ssave,
    const std::map<key_type, data_type> &val)
{
    size_t v_size = val.size();
    ssave->save_arithmetic<unsigned int>(v_size);
    std::map<key_type, data_type>::iterator iter = val.begin();
    for (size_t i = 0; i < v_size && is_good_; ++i, ++iter)
    {
        this->save<key_type>(iter->frist);
        this->save<data_type>(iter->sencond);
    }
    return;
}

//===========================================================================================================

class ZCE_Serialized_Load;
//���������ȡ���ݵ�һЩ��
template<typename val_type >
class ZCE_ClassLoad_Help
{
public:
    void load_help(ZCE_Serialized_Load *sload, val_type &val);
};

template<>
class ZCE_ClassLoad_Help<std::string>
{
public:
    void load_help(ZCE_Serialized_Load *sload, std::string &val);
};

template<typename vector_type >
class ZCE_ClassLoad_Help<std::vector<vector_type> >
{
public:
    void load_help(ZCE_Serialized_Load *sload, std::vector<vector_type> &val);
};

template<typename list_type >
class ZCE_ClassLoad_Help<std::list<list_type> >
{
public:
    void load_help(ZCE_Serialized_Load *sload, std::list<list_type> &val);
};

template<typename key_type, typename data_type >
class ZCE_ClassLoad_Help<std::map<key_type, data_type> >
{
public:
    void load_help(ZCE_Serialized_Load *sload, std::map<key_type, data_type> &val);
};

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
    inline bool is_good()
    {
        return is_good_;
    }

    ///���ô����־
    inline void set_bad()
    {
        is_good_ = false;
    }

    ///���ã����ÿ�ʼλ�ú�good��־λ
    inline void reset()
    {
        read_pos_ = read_buf_;
        is_good_ = true;
    }

    ///д��һ������,ֻ���ػ���������
    template<typename val_type>
    void load(val_type & /*val*/)
    {
        ZCE_ASSERT(false);
    }

    ///����ö��ֵ
    template<typename val_type  >
    void load(typename std::enable_if<std::is_enum<val_type>::value, val_type>::type &val)
    {
        return load_enum(val);
    }
    template<typename enum_type >
    void load_enum(const enum_type &val)
    {
        load_arithmetic(static_cast<int &>(val));
    }

    ///������ֵ����
    template<typename val_type >
    void load(typename std::enable_if<std::is_arithmetic<val_type>::value, val_type>::type &val)
    {
        return load_arithmetic(val);
    }
    void load_arithmetic(bool &val);
    void load_arithmetic(char &val);
    void load_arithmetic(unsigned char &val);
    void load_arithmetic(short &val);
    void load_arithmetic(unsigned short &val);
    void load_arithmetic(int &val);
    void load_arithmetic(unsigned int &val);
    void load_arithmetic(int64_t &val);
    void load_arithmetic(uint64_t &val);
    void load_arithmetic(float &val);
    void load_arithmetic(double &val);


    ///д������
    template<typename val_type >
    void load(typename std::enable_if<std::is_array<val_type>::value, val_type>::type &val)
    {
        // consider alignment
        std::size_t ary_count = sizeof(val) / (
            static_cast<const char *>(static_cast<const void *>(&val[1]))
            - static_cast<const char *>(static_cast<const void *>(&val[0]))
            );
        size_t load_count;
        return load_array(val, ary_count, load_count);
    }
    template<typename array_type >
    void load_array(array_type ary, size_t ary_count,size_t &load_count)
    {
        //��ȡ���鳤��
        unsigned int ui_load_count;
        this->load_arithmetic(ui_load_count);
        load_count = ui_load_count;
        //
        if (!is_good_ || load_count > ary_count || read_pos_ + load_count * sizeof(ary[0]) > end_pos_)
        {
            is_good_ = false;
            return;
        }
        for (size_t i = 0; i < load_count && is_good_; ++i)
        {
            this->load(*(ary + i));
        }
        return;
    }
    ///�ػ������ַ������м���
    template<>
    void load_array(char *ary, size_t ary_count, size_t &load_count)
    {
        unsigned int ui_load_count;
        this->load_arithmetic(ui_load_count);
        load_count = ui_load_count;

        if (!is_good_ || load_count > ary_count || read_pos_ + load_count > end_pos_)
        {
            is_good_ = false;
            return;
        }

        memcpy(ary, read_pos_, load_count);
        read_pos_ += load_count;
    }

    template<>
    void load_array(unsigned char *ary, size_t ary_count, size_t &load_count)
    {
        unsigned int ui_load_count;
        this->load_arithmetic(ui_load_count);
        load_count = ui_load_count;

        if (!is_good_ || load_count > ary_count || read_pos_ + load_count > end_pos_)
        {
            is_good_ = false;
            return;
        }

        memcpy(ary, read_pos_, load_count);
        read_pos_ += load_count;
    }

    ///�����࣬���Ҫ�ø�����ʵ��һЩƫ�ػ�������
    template<typename val_type >
    void load(typename std::enable_if<std::is_class<val_type>::value, val_type>::type &val)
    {
        ZCE_ClassLoad_Help<val_type> sload;
        sload.load_help<val_type>(this, val);
        return;
    }

    ///ʹ��&��������д�����ݣ�
    template<typename val_type>
    ZCE_Serialized_Load &operator &(val_type *val)
    {
        this->load<val_type>(val);
        return *this;
    }

public:

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

//�����࣬save_help ����
template<typename val_type>
void ZCE_ClassLoad_Help<val_type>::load_help(ZCE_Serialized_Load *sload,
    val_type &val)
{
    val.serialize(sload);
}

template<typename vector_type>
void ZCE_ClassLoad_Help<std::vector<vector_type> >::load_help(ZCE_Serialized_Load *sload,
    std::vector<vector_type> &val)
{
    unsigned int v_size = 0;
    sload->load_arithmetic(v_size);
    for (size_t i = 0; i < v_size && is_good_; ++i)
    {
        sload->load<vector_type>(val[i]);
    }
    return;
}
/*
template<typename list_type>
void ZCE_ClassLoad_Help<std::list<list_type> >::load_help(ZCE_Serialized_Load *sload,
    std::list<list_type> &val)
{
    size_t v_size = val.size();
    sload->save_arithmetic<unsigned int>(v_size);
    std::list<list_type>::iterator iter = val.begin();
    for (size_t i = 0; i < v_size && is_good_; ++i, ++iter)
    {
        sload->save<list_type>(*iter);
    }
    return;
}


template<typename key_type, typename data_type >
void ZCE_ClassLoad_Help<std::map<key_type, data_type> >::load_help(ZCE_Serialized_Load *sload,
    std::map<key_type, data_type> &val)
{
    size_t v_size = val.size();
    this->save_arithmetic<unsigned int>(v_size);
    std::map<key_type, data_type>::iterator iter = val.begin();
    for (size_t i = 0; i < v_size && is_good_; ++i, ++iter)
    {
        this->load<key_type>(iter->frist);
        this->load<data_type>(iter->sencond);
    }
    return;
}*/

#endif //ZCE_LIB_BYTES_SERIALIZATION_H_

