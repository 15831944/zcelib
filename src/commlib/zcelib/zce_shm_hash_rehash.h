/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_shm_hash_rehash.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2010��04��6��
* @brief      ���HASH�����ͻ��HASHʵ�֣�ͬʱҲ�ǰ�ȫ��HASH,���Ŀ���������HASH
*             ӳ�乲���ڴ��ļ����Բ���COPY�ķ�ʽ����,
*             ����㷨�����ô�������ȫ����������ŵģ����Զ����ݽ���ֱ�ӵı��ݺͲ���,
*             �����Ҽ�����һ��SAFE��ʾ
*             HASHȡģ���ܴ����Ҳ���24��,Ϊʲô�����أ���Ϊ������һЩ���ԣ���HASH�Ĵ���
*             ��24������ʱ����һ�γ��ֳ�ͻ������ĸ����ʴ�Լ��85%����
*             http://blog.csdn.net/fullsail/article/details/6328702
* 
* @details    ����㷨�Ļ���˼·���Լ�ͨ�ļ���ͬ��,��лicezhuang��mikewei��spraydong��
*             ͬ�£�����������һ������HASH�ķ�ʽ�����ͻ���㷨��
*             ����������������ģ�壬�����������ݹ������з�װ,
*             ����㷨�Ĳ���֮�����ԣ��ڼ�������£�����ܳ����޷����ص������
*             ���Ǽ�ͨ��ͬ��˵���ǲ��ԣ������㷨�ĸ��������ܴﵽ95%,������������Ǳ�֤
*             ��80%�ĸ���������ʱ���������,�����ǵ��㷨��32�ף�
*             ����Թ���һ�£�����㷨���ֳ�ͻ�����Ӧ���Ƿǳ��͵�
*             ����Ҫע����ǣ�������˲�����д���������ݣ������ǻ���ֿ���������ֻ�а�ص�����,
*             ����޷�����,������������û�����������������ݻ��ǿ���ʹ�õģ�����Ǻô�,
* @note       �����һЩ��ʷ��
*             1.Date  : 2010��04��12��
*             Author  : Sailzeng
*             Modification  : ���뼶����ɣ���ʼ����,
* 
*             1.Date  : 2010��04��12��
*             Author  : Sailzeng
*             Modification  : ���ܲ��Խ���Ա�HASH�Ŀ�����ʽ�Ͷ��HASH��ʽ,���HASH������
*             Ҫ�����٣���1s�����ǧ��β�ѯ���ǿ��Ե�,
* 
*             2.Date  : 2010��04��20��
*             Author  : Sailzeng
*             Modification  :�������������һ����̭�㷨�ģ���Ϊ˼ǰ��󣬳����ڴ����ļ���ȡģ�Ĺ����н�����̭���������岻��,
*             ������kliuͬѧ��ս������˼����һ�£���ʹ���е�������Լ1�ڸ���ȫ��ɨ��Ҫ��̭�����ݵ�ʱ��Ҳ�ò���1s���������ô�ں��ɱ���
*             ��Ȼ����ս���Ҿ��ṩһ���ɡ�����Ҳ���ֵ�����������,���⣬Ϊʲô�Ҳ�����¯����,��Ϊ�����Ͷ�������,
* 
*             2.Date  : 2010��04��21��
*             Author  : Sailzeng
*             Modification  :˼���������һ��ǽ�_equal��ɾ���ˣ���ΪREHASH����insert_equal֧�ֲ����ã�Ϊʲô�أ��������֧����ȵĶ������
*             ���������б��ĳ���,�ṩ������������������к�YY����������һ��Ҫ�ã����ȸ�������������Լ��Ĵ������ǻ����ص�
*             ��ʵ��ʹ�ģ������Ĵ��뱻ɾ���ˣ�����ª�Ķ���������Ҳ���ǶԵ���
*/

#ifndef ZCE_SHM_HASH_TABLE_REHASH_H_
#define ZCE_SHM_HASH_TABLE_REHASH_H_

#include "zce_shm_predefine.h"

namespace ZCE_LIB
{

//Ϊ�˷�����룬Ԥ�ȶ���һ��
template < class _value_type,
         class _key_type,
         class _hash_fun,
         class _extract_key,
         class _equal_key,
         class _washout_fun > class shm_hash_rehash;

//SAFE HASH ��������ע�������SAFE HASH������SAFE iterator���Ұ�HASH��ǰ�洿���Ƿ��㼸���ļ�
//����һ��
template < class _value_type,
         class _key_type,
         class _hashfun,
         class _extract_key,
         class _equal_key,
         class _washout_fun >
class _hash_rehash_iterator
{
protected:

    //HASH TABLE�Ķ���
    typedef shm_hash_rehash < _value_type,
            _key_type,
            _hashfun,
            _extract_key,
            _equal_key ,
            _washout_fun > _hashtable_rehash;

    //���������
    typedef _hash_rehash_iterator < _value_type,
            _key_type,
            _hashfun,
            _extract_key,
            _equal_key,
            _washout_fun > iterator;

protected:
    //���к�
    size_t                 serial_;
    //
    _hashtable_rehash     *ht_rehash_instance_;

public:
    _hash_rehash_iterator():
        serial_(_shm_memory_base::_INVALID_POINT),
        ht_rehash_instance_(NULL)
    {
    }

    _hash_rehash_iterator(size_t serial, _hashtable_rehash *ht_safe_inst):
        serial_(serial),
        ht_rehash_instance_(ht_safe_inst)
    {
    }

    ~_hash_rehash_iterator()
    {
    }

    _value_type &operator*() const
    {
        return ht_rehash_instance_->value_base_ [serial_];
    }

    _value_type *operator->() const
    {
        return ht_rehash_instance_->value_base_ + serial_;
    }

    //����ֻ�ṩǰ���������������Ϊʹ�ÿ���ʹ��LIST��֤�����ĸ�Ч�����ֲ��У�
    //����Ҫ�ṩ����ĺ���
    //ǰ�������
    iterator &operator++()
    {
        _extract_key get_key;
        _equal_key   equal_key;
        size_t old_serial = serial_ + 1;

        for (; old_serial < ht_rehash_instance_->hash_safe_head_->num_of_node_ ; ++old_serial)
        {
            //�������Чֵ���󣬶�����Ԫ�ؽ��и�ֵ��Ϊʲôʹ��placement new����Ϊ��������һ��
            //�������һ����Чֵ
            if (false == equal_key( get_key (*(ht_rehash_instance_->value_base_ + old_serial)),
                                    get_key(ht_rehash_instance_->invalid_data_)) )
            {
                serial_ = old_serial;
                break;
            }
        }

        //���û�з�������
        if (old_serial == ht_rehash_instance_->hash_safe_head_->num_of_node_)
        {
            serial_ = _shm_memory_base::_INVALID_POINT;
        }

        return *this;
    }

    //ǰ�������
    iterator operator++(int)
    {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }
    //
    bool operator==(const iterator &it) const
    {
        if (ht_rehash_instance_ == it.ht_rehash_instance_ &&
            serial_ == it.serial_ )
        {
            return true;
        }

        return false;
    }
    //
    bool operator!=(const iterator &it) const
    {
        return !(*this == it);
    }

    //������žͿ����ٸ���ģ��ʵ���������ҵ���Ӧ����,����ʹ��ָ��
    size_t getserial() const
    {
        return serial_;
    }
};

//ͷ����LRU_HASH��ͷ���ṹ������LRUHASH�ڴ��ǰ��
class _hashtable_rehash_head
{
protected:
    //������������
    _hashtable_rehash_head():
        size_of_mmap_(0),
        num_of_node_(0),
        sz_freenode_(0),
        sz_usenode_(0),
        len_of_primes_list_(0),
        expire_start_(0)
    {
        //��0
        memset(primes_list_, 0, sizeof(primes_list_));
    }
    ~_hashtable_rehash_head()
    {
    }

public:
    //�ڴ����ĳ���
    size_t           size_of_mmap_;

    //NODE,INDEX������,INDEX�ĸ�����NODE�Ľڵ����Ϊ1:1,
    size_t           num_of_node_;

    //FREE��NODE����
    size_t           sz_freenode_;
    //USE��NODE����
    size_t           sz_usenode_;

    //�����б��ĸ���
    size_t           len_of_primes_list_;

    //
    size_t           primes_list_[NUM_PRIMES_LIST_ELEMENT];

    //��̭��ʵ��λ�ã�Ϊ�˱���ÿ����̭����һ���ط���ʼ
    size_t           expire_start_;

};

template < class _value_type,
         class _key_type,
         class _hash_fun = smem_hash<_key_type>,
         class _extract_key = smem_identity<_value_type>,
         class _equal_key = std::equal_to<_key_type>,
         class _washout_fun = _default_washout_fun<_value_type> >
class shm_hash_rehash : public  _shm_memory_base
{
public:

    //���������
    typedef _hash_rehash_iterator < _value_type,
            _key_type,
            _hash_fun,
            _extract_key,
            _equal_key,
            _washout_fun > iterator;

    //��������������Ԫ
    friend class _hash_rehash_iterator < _value_type,
        _key_type,
        _hash_fun,
        _extract_key,
        _equal_key ,
            _washout_fun >;

protected:

    //��Ч�����ݣ�����������������KEY����Զ������ֵģ�һ���Ǳ���0������-1
    //����ط���ʵ��͵���ı��֣����ڼ򻯴���ĳ���ռ䣬�Ƿ���Ч
    _value_type              invalid_data_;

    //���ڹ����ڴ�ͷ����ָ��
    _hashtable_rehash_head  *hash_safe_head_;

    //������ָ��
    _value_type             *value_base_;

    //���ȼ�������ָ��,��32λ�����ݱ������ȼ�
    unsigned int            *priority_base_;

protected:

    shm_hash_rehash():
        hash_safe_head_(NULL),
        value_base_(NULL),
        priority_base_(NULL)
    {
    }

public:

    ~shm_hash_rehash()
    {
    }

protected:

    //���ش���N��һ������,(���һ������),ʹ��������Ϊȡģ��
    static size_t get_next_prime(const size_t n)
    {
        //Ϊʲô��ֱ����ģ������дstatic���飿�������Ǹ���������֧��,�Ѿ����ǹ�����
        //���һ�����ܺ�
        const size_t primes_matrix[NUM_HASH_SAFE_PRIMES_LIST][NUM_PRIMES_LIST_ELEMENT + 1] = __HASH_SAFE_PRIME_LIST_BODY;

        size_t i = 0;

        //
        for (; i < NUM_HASH_SAFE_PRIMES_LIST ; ++i)
        {
            if (primes_matrix[i][NUM_PRIMES_LIST_ELEMENT] >= n)
            {
                break;
            }
        }

        //����������һ������һ��,
        return (i == NUM_HASH_SAFE_PRIMES_LIST) ? primes_matrix[i - 1][NUM_PRIMES_LIST_ELEMENT] : primes_matrix[i][NUM_PRIMES_LIST_ELEMENT];
    }

    //��value��ȡֵ
    size_t bkt_num_value(const _value_type &obj, size_t one_primes) const
    {
        _extract_key get_key;
        return static_cast<size_t>(bkt_num_key(get_key(obj), one_primes));
    }

    //Ϊʲô������������ĺ���,�Լ�����һ��,
    //���صĻ������_value_type��_key_typeһ�����͵��ſް� ---inmore
    size_t bkt_num_key(const _key_type &key, size_t one_primes) const
    {
        _hash_fun hash_fun;
        return static_cast<size_t>(hash_fun(key) % one_primes);
    }

    //ȡ����Ч���ݣ��ڵ�����ʹ�õ�ʱ����Ҫ
    _value_type get_invalid_data()
    {
        return invalid_data_;
    }

    //�����Լ����ڲ��ĳ�ʼ������
    static shm_hash_rehash < _value_type,
           _key_type,
           _hash_fun,
           _extract_key,
           _equal_key > *
           initialize_i(size_t primes_number,
                        const size_t primes_list[],
                        size_t num_node,
                        size_t sz_alloc,
                        char *pmmap,
                        const _value_type &invalid_data,
                        bool if_expire,
                        bool if_restore)
    {

        shm_hash_rehash< _value_type, _key_type , _hash_fun, _extract_key, _equal_key >* instance
            = new shm_hash_rehash< _value_type, _key_type , _hash_fun, _extract_key, _equal_key >();

        instance->smem_base_ = pmmap;
        char *tmp_base = instance->smem_base_;
        instance->hash_safe_head_ = reinterpret_cast<_hashtable_rehash_head *>(tmp_base);
        tmp_base = tmp_base + sizeof(_hashtable_rehash_head);

        instance->value_base_ = reinterpret_cast<_value_type *>(tmp_base);
        tmp_base = tmp_base + (sizeof(_value_type) * (num_node));

        //��ʼ���ߴ�,����ǻָ����ղ��Ѿ��ȽϹ���
        instance->hash_safe_head_->size_of_mmap_ = sz_alloc;
        instance->hash_safe_head_->num_of_node_ = num_node;

        //ÿ�ζ�������̭��ʼ�ĵط���¼Ϊ0
        instance->hash_safe_head_->expire_start_ = 0;

        if (if_expire)
        {
            instance->priority_base_ = reinterpret_cast<unsigned int *>(tmp_base);
            tmp_base = tmp_base + (sizeof(unsigned int) * (num_node));
        }
        else
        {
            //����ǿ��һ�γ�NULL
            instance->priority_base_ = NULL;
        }

        instance->invalid_data_ = invalid_data;

        //��������д��ȥ
        instance->hash_safe_head_->len_of_primes_list_ = primes_number;

        for (size_t y = 0; y < primes_number; ++y)
        {
            instance->hash_safe_head_->primes_list_[y] = primes_list[y];
        }

        //��������лָ�����
        if (if_restore == false)
        {
            //������ʼ�����е��ڴ�,�еĽڵ�ΪFREE
            instance->clear(if_expire);
        }

        return instance;
    }

public:

    //�ڴ����Ĺ���Ϊ define��,index��,data��,��������Ҫ�ĳ���,

    //����ֵΪ��Ҫ����Ŀռ��С
    //��һ��������ʾ����Ҫ�Ľڵ������ע�ⷵ�ص���ʵ��INDEX����,������һЩ
    //�ڶ���������ʾ�Ƿ�ʹ�ó�ʱ����
    static size_t getallocsize(size_t &num_node, bool if_expire)
    {
        num_node = get_next_prime(num_node);
        size_t sz_alloc =  0;
        //
        sz_alloc += sizeof(_hashtable_rehash_head);

        sz_alloc += sizeof(size_t) * NUM_PRIMES_LIST_ELEMENT;
        //�����ṹ������
        sz_alloc += sizeof(_value_type) * (num_node);

        if (if_expire)
        {
            sz_alloc += (sizeof(unsigned int) * (num_node));
        }

        return sz_alloc;
    }

    //����ֵΪ��Ҫ����Ŀռ��С
    //����������������У�����������õ��ռ�,
    //��һ�������������г���,
    //�ڶ��������������У�
    //�������������Ƿ�ʱ��
    //�������������ز���,�ܽڵ����,
    static size_t getallocsize(size_t primes_number, const size_t primes_list[], bool if_expire, size_t &node_count)
    {
        //�б�����󳤶Ȳ��ܴ���MAX_PRIMES_LIST_ELEMENT
        assert(primes_number <= NUM_PRIMES_LIST_ELEMENT);

        //����������
        node_count = 0;

        for (size_t i = 0; i < primes_number; ++i)
        {
            node_count += primes_list[i];
        }

        assert(node_count > 0);

        //
        size_t sz_alloc =  0;

        //����ռ䣬����ͷ����������,�ṹ���������ȼ��ռ�
        sz_alloc += sizeof(_hashtable_rehash_head);

        sz_alloc += sizeof(_value_type) * (node_count);

        if (if_expire)
        {
            sz_alloc += sizeof(unsigned int) * (node_count);
        }

        return sz_alloc;
    }

    //�Ƽ�ʹ���������,����������Ҫ�ٺܶ�
    //��ʼ�������غ��������ָ�룬�Ժ��ͨ�����ָ�������Ϊʲô��ֱ���ù��캯���أ��Һ��ѻش𣬿�����ͨ��getallocsizeһ����У�
    //��1���������ߴ磬����Ҫ�ż���Ԫ�ظ��㣬ע����������᷵��һ��ʵ���ҷ�����ٳߴ����
    //��2�����������ݽ����Ŀռ�ָ�룬�ռ�Ĵ�Сͨ��getallocsize�õ�.
    //��3��������һ����Ч����������ֵ����Ϊ�����ø��㿪��һ���ط���¼ĳ�������Ƿ�ʹ����.�����һὫ���е����ݶ���ʼ����Ч�ṹ����Ч�ṹ���һ��¼������ռ�û��ʹ��
    //��4���������Ƿ�Ҫʹ����̭����,��������ã��ռ���Ը���СһЩ
    //��5���������Ƿ��Ǵ�һ���ڴ��лָ��ռ䣬���繲���ڴ�֮��Ļָ�
    static shm_hash_rehash < _value_type,
    _key_type,
    _hash_fun,
    _extract_key,
    _equal_key > *
    initialize(size_t &num_node, char *pmmap, const _value_type &invalid_data, bool if_expire, bool if_restore = false)
    {
        assert(pmmap != NULL && num_node > 0 );

        //����,������ĳߴ磬������һ�����ʵĿռ�
        num_node = get_next_prime(num_node);

        size_t sz_alloc = getallocsize(num_node, if_expire);

        _hashtable_rehash_head *hashhead =  reinterpret_cast< _hashtable_rehash_head * >(pmmap);

        const size_t primes_matrix[NUM_HASH_SAFE_PRIMES_LIST][NUM_PRIMES_LIST_ELEMENT + 1] = __HASH_SAFE_PRIME_LIST_BODY;

        size_t primes_list_no = 0;

        //�ҵ�ѡ���HASH�У�ֱ�ӽ���һ�е����ݷ����ڴ棬������洦��
        for (size_t x = 0; x < NUM_HASH_SAFE_PRIMES_LIST; ++x)
        {
            //���һ��Ԫ����ǰ�����������б��ĺͣ���NODE�ĸ���ҲӦ���������б��ĺ�
            if (num_node == primes_matrix[x][NUM_PRIMES_LIST_ELEMENT] )
            {
                primes_list_no = x;
            }
        }

        //����ǻָ�,���ݶ����ڴ���,�����ݽ��м��
        if (if_restore == true)
        {
            //�Ƿ�Ҫ��������������ڵı��Ƿ�һ����?����ʱѡ���˽��м�飬
            //�������������������ʹ��������ľ���ı��ˣ����ǿ����ù����ڴ��м�����ݼ����ܣ���ѡ�����Ͻ���������

            //���һ�»ָ����ڴ��Ƿ���ȷ,
            if ( sz_alloc != hashhead->size_of_mmap_ ||
                 num_node != hashhead->num_of_node_ )
            {
                return NULL;
            }

            //�����б��ĸ���Ӧ��һ��
            if ( hashhead->len_of_primes_list_ != NUM_PRIMES_LIST_ELEMENT )
            {
                return NULL;
            }

            //HASH�б��ĸ�����Ӧ�õ���Ԫ�ص��ܺͣ��ǲ���Ӧ�ý�ÿ�����ݶ��ó����Ƚ�һ����
            size_t num_node_count = 0;

            for (size_t p = 0; p < hashhead->len_of_primes_list_; ++p)
            {
                num_node_count += hashhead->primes_list_[p];
            }

            if (num_node_count != hashhead->num_of_node_)
            {
                return NULL;
            }

            //������������Ƿ�һ��
            for (size_t y = 0; y < NUM_PRIMES_LIST_ELEMENT; ++y)
            {
                if (hashhead->primes_list_[y] != primes_matrix[primes_list_no][y])
                {
                    return NULL;
                }
            }

        }

        //�����չ�
        return initialize_i(NUM_PRIMES_LIST_ELEMENT,
                            primes_matrix[primes_list_no],
                            num_node,
                            sz_alloc,
                            pmmap,
                            invalid_data,
                            if_expire,
                            if_restore);

    }

    //������ʼ�����е��ڴ�,���еĽڵ�ΪFREE
    void clear(bool if_expire)
    {
        //�����ؼ�Node,�Լ���س���,��ʼ���е�������free.
        hash_safe_head_->sz_freenode_ = hash_safe_head_->num_of_node_;
        hash_safe_head_->sz_usenode_ = 0;

        //��ʼ��free�������������е����ݳ�ʼ������Чֵ�����ڱ�ʾһ����¼�Ƿ�������
        for (size_t i = 0; i < hash_safe_head_->num_of_node_ ; ++i )
        {
            //�������Чֵ���󣬶�����Ԫ�ؽ��и�ֵ��Ϊʲôʹ��placement new����Ϊ��������һ��
            new (value_base_ + i)_value_type(invalid_data_);
        }

        //���Ҫ��¼��̭��Ϣ
        if (if_expire)
        {
            memset(priority_base_, 0, (sizeof(unsigned int) * (hash_safe_head_->num_of_node_)));
        }
    }

    //��Ҳ���Դ���һ���������У���Ϊ���ж���HASHȡģ����������,
    static shm_hash_rehash < _value_type,
    _key_type,
    _hash_fun,
    _extract_key,
    _equal_key > *
    initialize(size_t primes_number, size_t primes_list[], char *pmmap, _value_type invalid_data, bool if_expire, bool if_restore = false)
    {
        assert(pmmap != NULL );

        _hashtable_rehash_head *hashhead =  reinterpret_cast< _hashtable_rehash_head * >(pmmap);

        size_t node_count = 0;
        size_t sz_alloc = getallocsize(primes_number, primes_list, if_expire, node_count);

        //����ǻָ�,���ݶ����ڴ���,�����ݽ��м��
        if (if_restore == true)
        {
            //�Ƿ�Ҫ��������������ڵı��Ƿ�һ����?����ʱѡ���˽��м�飬
            //�������������������ʹ��������ľ���ı��ˣ����ǿ����ù����ڴ��м�����ݼ����ܣ���ѡ�����Ͻ���������

            //���һ�»ָ����ڴ��Ƿ���ȷ,
            if (sz_alloc = hashhead->size_of_mmap_ ||
                           node_count != hashhead->num_of_node_ )
            {
                return NULL;
            }

            //�����б��ĸ���Ӧ��һ��
            if ( hashhead->len_of_primes_list_ != primes_number )
            {
                return NULL;
            }

            //������������Ƿ�һ��
            for (size_t y = 0; y < primes_number; ++y)
            {
                if (hashhead->primes_list_[y] != primes_list[y])
                {
                    return NULL;
                }
            }
        }

        //�����չ�
        return initialize_i(primes_number,
                            primes_list,
                            node_count,
                            sz_alloc,
                            pmmap,
                            invalid_data,
                            if_expire,
                            if_restore);
    }

public:

    //�õ���ʼ�ĵ�������λ�ã���ʵ������Ч������ѽ�����ǣ�
    iterator begin()
    {
        _extract_key get_key;
        _equal_key   equal_key;

        //�ҵ���һ���Ѿ���д���ݵĶ�����Ϊbegin
        for (size_t i = 0; i < hash_safe_head_->num_of_node_; ++i)
        {
            //�������һ����Чֵ
            if (false == equal_key(get_key(*(value_base_ + i)), get_key(invalid_data_)) )
            {
                return iterator( i, this);
            }
        }

        return end();
    }

    //�õ�����λ��
    iterator end()
    {
        return iterator(_INVALID_POINT, this);
    }
    //��ǰʹ�õĽڵ�����
    size_t size() const
    {
        return hash_safe_head_->sz_usenode_;
    }
    //�õ�����
    size_t capacity() const
    {
        return hash_safe_head_->num_of_node_;
    }
    //�Ƿ�Ϊ��
    bool empty() const
    {
        return (hash_safe_head_->sz_freenode_ == hash_safe_head_->num_of_node_);
    }
    //�Ƿ�ռ��Ѿ�����
    bool full() const
    {
        return (hash_safe_head_->sz_freenode_ == 0);
    }

    //����ڵ�
    std::pair<iterator, bool> insert(const _value_type &val )
    {
        //ʹ�ú�������,һ���൥������һ���Ƿ����?
        _extract_key get_key;
        _equal_key   equal_key;

        size_t idx_count = 0;
        size_t idx_no_use = _INVALID_POINT;

        //ѭ������N��ȡģ������
        for (size_t i = 0; i < hash_safe_head_->len_of_primes_list_; ++i )
        {
            size_t idx = bkt_num_value(val, hash_safe_head_->primes_list_[i]);
            idx_count += idx;

            //����ҵ���ͬ��Key����,������������岻������
            if (equal_key((get_key(value_base_[idx_count])), (get_key(val))) )
            {
                return std::pair<iterator, bool>(iterator(idx_count, this), false);
            }

            //�����һ����Ч���ݣ���ʾһ����λ��
            if (equal_key((get_key(value_base_[idx_count])), (get_key(invalid_data_))) == true)
            {
                if (idx_no_use == _INVALID_POINT)
                {
                    idx_no_use = idx_count;
                    break;
                }
            }

            //׼������һ��������������
            idx_count += (hash_safe_head_->primes_list_[i] - idx);
        }

        //���ÿһ�ж�Ӧ��λ�ö�����åռ����,����һ������ĵ�����end,����ǰ�Σ��ռ�Σ����
        if (_INVALID_POINT == idx_no_use )
        {
            return std::pair<iterator, bool>(end(), false);
        }

        //ʹ��placement new���и�ֵ
        new (value_base_ + idx_no_use)_value_type(val);

        ++(hash_safe_head_->sz_usenode_);
        --(hash_safe_head_->sz_freenode_);

        return std::pair<iterator, bool>(iterator(idx_no_use, this), true);
    }

    //�����ȼ��Ĳ��룬��ʼ��ʼ����ʱ�����if_expire == true
    //@const _value_type &val ���������
    //@unsigned int priority  �����������ȼ���
    //@unsigned int expire_priority = static_cast<unsigned int>(-1)����̭�����ȼ���Ĭ��Ϊ���ֵ�������в���̭�������������djiang�ĺý���
    std::pair<iterator, bool> insert(const _value_type &val,
                                     unsigned int priority,
                                     unsigned int expire_priority = 0)
    {

        //ʹ�ú�������,һ���൥������һ���Ƿ����?
        _extract_key get_key;
        _equal_key   equal_key;

        size_t idx_count = 0;
        size_t idx_no_use = _INVALID_POINT;

        //ѭ������N��ȡģ������
        for (size_t i = 0; i < hash_safe_head_->len_of_primes_list_; ++i )
        {
            size_t idx = bkt_num_value(val, hash_safe_head_->primes_list_[i]);
            idx_count += idx;

            //����ҵ���ͬ��Key����,������������岻������
            if (equal_key((get_key(value_base_[idx_count])), (get_key(val))) )
            {
                return std::pair<iterator, bool>(iterator(idx_count, this), false);
            }

            //�����һ����Ч���ݣ���ʾһ����λ��
            if (equal_key((get_key(value_base_[idx_count])), (get_key(invalid_data_))) == true)
            {
                if (idx_no_use == _INVALID_POINT)
                {
                    idx_no_use = idx_count;
                    break;
                }
            }
            //������λ���Ѿ������ݣ����Լ��һ����������Ƿ����
            else
            {
                //����Ǹ���djiang�Ľ�����е�һ�����书�ܣ��ڲ����ʱ�����̭�����ڵ����ݣ�
                //������ȼ�ʱ���Ѿ����ڣ��Ϳ���ֱ���������λ��
                if (priority_base_[idx_count] <= expire_priority)
                {
                    idx_no_use = idx_count;
                    break;
                }
            }

            //׼������һ��������������
            idx_count += (hash_safe_head_->primes_list_[i] - idx);
        }

        //���ÿһ�ж�Ӧ��λ�ö�����åռ����,����һ������ĵ�����end,����ǰ�Σ��ռ�Σ����
        if (_INVALID_POINT == idx_no_use )
        {
            return std::pair<iterator, bool>(end(), false);
        }

        //ʹ��placement new���и�ֵ
        new (value_base_ + idx_no_use)_value_type(val);

        ++(hash_safe_head_->sz_usenode_);
        --(hash_safe_head_->sz_freenode_);

        //�������ɹ��˼�¼���ȼ�
        priority_base_[idx_no_use] = priority;

        return std::pair<iterator, bool>(iterator(idx_no_use, this), true);
    }

    //��ѯ��Ӧ��Key�Ƿ���,���ص�����
    //����ط���һ������,����ط����صĵ�����++�����ܸ����ҵ���ͬ��key������,��������HASHʵ�����������
    iterator find(const _key_type &key)
    {
        //ʹ������������,һ���൥������һ���Ƿ����?
        _extract_key get_key;
        _equal_key   equal_key;

        size_t idx_count = 0;

        //ѭ������N��ȡģ������
        for (size_t i = 0; i < hash_safe_head_->len_of_primes_list_; ++i )
        {
            //��valȡ��key��ȡģ
            size_t idx = bkt_num_key(key, hash_safe_head_->primes_list_[i]);
            idx_count += idx;

            //����ҵ���ͬ��Key����,������������岻������
            if (equal_key((get_key(value_base_[idx_count])), key) )
            {
                return iterator(idx_count, this);
            }

            //׼������һ��������������
            idx_count += (hash_safe_head_->primes_list_[i] - idx);
        }

        return end();
    }

    //
    iterator find_value(const _value_type &val)
    {
        _extract_key get_key;
        return find(get_key(val));
    }

    bool erase(const _key_type &key)
    {
        iterator iter_temp = find(key);

        if (iter_temp == end())
        {
            return false;
        }

        return erase(iter_temp);
    }

    //ʹ�õ�����ɾ��,������Ч���Բ��ü�д��
    bool erase(const iterator &it_del)
    {
        //�Ҳ�������ļ��
        if (it_del != end())
        {
            //������������,ע���������ֻ��ͨ��ָ����У�����int��֮�������
            (value_base_ + it_del.getserial())->~_value_type();
            //�ָ�����Ч����
            new (value_base_ + it_del.getserial() )_value_type(invalid_data_);
            --(hash_safe_head_->sz_usenode_);
            ++(hash_safe_head_->sz_freenode_);

            return true;
        }

        return false;
    }

    //ɾ��ĳ��ֵ
    bool erase_value(const _value_type &val )
    {
        _extract_key get_key;
        return erase( get_key(val));
    }

    //����,����������ݹҵ�LIST���ʼ,��̭ʹ��expire,disuse
    //���ȼ���������ʹ�õ�ǰ��ʱ��
    bool active(const _key_type &key,
                unsigned int priority /*=static_cast<unsigned int>(time(NULL))*/ )
    {
        iterator  iter_tmp = find(key);

        //����ҵ��˶�Ӧ��KEY
        if ( end()  != iter_tmp )
        {
            priority_base_[iter_tmp.getserial()] = priority;
            return true;
        }

        return false;
    }

    //
    bool active_value(const _value_type &val,
                      unsigned int priority /*=static_cast<unsigned int>(time(NULL))*/ )
    {
        _extract_key get_key;
        return active(get_key(val), priority);
    }

    //��̭���ڵ�����,����LIST�м�������ǰ��չ���ʵ������ģ���Ҫ���㴫������ȼ������ʱ��
    //С�ڵ���������ȼ������ݽ�����̭
    //hope_expire_num��ʾ��ϣ��ɾ�����ٸ�ֵ��Ĭ��Ϊ���ֵ,ȫ����̭
    size_t expire(unsigned int expire_time, size_t hope_expire_num = static_cast<size_t>(-1))
    {
        //��β����ʼ��飬
        size_t expire_num = 0;
        size_t i = 0;

        for (; i < hash_safe_head_->num_of_node_ && expire_num < hope_expire_num ; ++i)
        {
            size_t del_iter = ( hash_safe_head_->expire_start_ + i) % hash_safe_head_->num_of_node_;

            //С�ڵ���
            if (priority_base_[del_iter] <= expire_time)
            {

                _washout_fun wash_fun;
                wash_fun(value_base_[del_iter]);
                ++expire_num;
                //
                iterator iter_tmp(del_iter, this);
                erase(iter_tmp);
            }
        }

        //��һ������ĵط��Ͽ�ʼ
        hash_safe_head_->expire_start_ = ( hash_safe_head_->expire_start_ + i) % hash_safe_head_->num_of_node_;

        return expire_num;
    }

};

};

#endif //#ifndef ZCE_SHM_HASH_TABLE_REHASH_H_
