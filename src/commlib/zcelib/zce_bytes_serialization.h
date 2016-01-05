#ifndef ZCE_LIB_BYTES_SERIALIZATION_H_
#define ZCE_LIB_BYTES_SERIALIZATION_H_

#include "zce_bytes_base.h"

//===========================================================================================================
//�����봦�����

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
    inline bool is_good();

    ///���ô����־
    inline void set_bad();

    ///����
    void reset();



    ///д��һ������,ֻ���ػ���������
    template<typename val_type>
    bool save(const val_type /*val*/)
    {
        return false;
    }

    ///д��һ��vector,ע�⣬ֻ֧�ֵ�32λ�ĸ�����vector
    template<typename  vector_type>
    bool save(std::vector<vector_type> &val)
    {
        size_t v_size = vector_data.size();
        this->save<unsigned int>(v_size);
        for (size_t i = 0; i < v_size; ++i)
        {
            this->save<vector_type>(vector_data[i]);
        }
        if (false == is_good_)
        {
            return false;
        }
        return true;
    }

    template<> bool save(int val)
    {
        const size_t SIZE_OF_VALUE = sizeof(int);
        if (write_pos_ + SIZE_OF_VALUE > end_pos_)
        {
            is_good_ = false;
            return is_good_;
        }
        ZBEUINT32_TO_BYTE(write_pos_, val);
        write_pos_ += SIZE_OF_VALUE;

        return is_good_;
    }
    template<> bool save(unsigned int val)
    {
        const size_t SIZE_OF_VALUE = sizeof(unsigned int);
        if (write_pos_ + SIZE_OF_VALUE > end_pos_)
        {
            is_good_ = false;
            return is_good_;
        }
        ZBEUINT32_TO_BYTE(write_pos_, val);
        write_pos_ += SIZE_OF_VALUE;

        return is_good_;
    }
    template<> bool save(float val)
    {
        const size_t SIZE_OF_VALUE = sizeof(float);
        if (write_pos_ + SIZE_OF_VALUE > end_pos_)
        {
            is_good_ = false;
            return is_good_;
        }
        ZFLOAT_TO_BYTE(write_pos_, val);
        write_pos_ += SIZE_OF_VALUE;

        return is_good_;
    }
    template<> bool save(double val)
    {
        const size_t SIZE_OF_VALUE = sizeof(double);
        if (write_pos_ + SIZE_OF_VALUE > end_pos_)
        {
            is_good_ = false;
            return is_good_;
        }
        ZDOUBLE_TO_BYTE(write_pos_, val);
        write_pos_ += SIZE_OF_VALUE;

        return is_good_;
    }

    ///ʹ��<<��������д�����ݣ���Ҫ�Ǳ����ⲿ����ͳһʹ��<<������.�ⲿ�����������ĺ���
    ///ZCE_Serialized_Save& operator <<(ZCE_Serialized_Save &dr_encode,const val_type &val);
    ///bool operator <<(ZCE_Serialized_Save &dr_encode,const val_type &val);
    template<typename val_type>
    ZCE_Serialized_Save &operator <<(val_type val);

    ///д��һ������
    template<typename ary_type>
    bool save_array(const ary_type *ary, size_t ary_size);

    
    template<typename vector_type>
    bool save_vector(const std::vector<vector_type> &vector_data);

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


//���ص�ǰ���Ƿ�������
inline bool ZCE_Serialized_Save::is_good()
{
    return is_good_;
}

inline void ZCE_Serialized_Save::set_bad()
{
    is_good_ = false;
}


template<typename vector_type>
bool ZCE_Serialized_Save::save_vector(const std::vector<vector_type> &vector_data)
{
    size_t v_size = vector_data.size();
    this->save<unsigned int>(v_size);
    for (size_t i = 0; i < v_size; ++i)
    {
        this->save<vector_type>(vector_data[i]);
    }
    if (false == is_good_)
    {
        return false;
    }
    return true;
}

//
template<typename val_type>
ZCE_Serialized_Save &ZCE_Serialized_Save::operator << (val_type val)
{
    this->save<val_type>(val);
    return *this;
}


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

