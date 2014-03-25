/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_shm_avltree.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2006��1��16��
* @brief      ϣ��AVLTree��Ҫ����ɿ��������MAP,SET,��MMAP��
*
* @details
*
* @note       ������������д��������2014��3��8����Ҳû���������ʣ�
*             ��Ҫԭ���Ƕ�AVL��ɾ������һֱû�����Ŀ������Խ����һֱ�����ˡ�
*             07���ʱ��scottxu����������ǹƥ��Ѻ�����㶨�ˣ��������
*             ��һֱû��������֮�أ��Ǻǡ�Ҳ�������һ��ǻ�ʵ�ֵġ�
*
*             һ�������ڰ칫�ң������װ�ᡣ���а����Լ���2B��
*
*             ��ʼ���ʱ�����濴��Scott�ĺ������ʵ�֣����ֵ��ʻ����ԣ�
*             ����ֱ��������ģ�ͬʱ����������룬Ӧ�ÿ��Բο��ã�
*             http://www.cnblogs.com/kiven-code/archive/2013/03/01/2938651.html
*
*/

#ifndef ZCE_LIB_SHM_AVL_TREE_H_
#define ZCE_LIB_SHM_AVL_TREE_H_

#include "zce_shm_predefine.h"

namespace ZCE_LIB
{




template<class _value_type, class _key_type, class _extract_key, class _compare_key> class shm_avl_tree;

///AVL TREE��ͷ��������
class _shm_avl_tree_head
{
protected:
    _shm_avl_tree_head()
        : size_of_mmap_(0)
        , num_of_node_(0)
        , sz_free_node_(0)
        , sz_use_node_(0)
    {
    }
    ~_shm_avl_tree_head()
    {
    }

public:
    //�ڴ����ĳ���
    size_t               size_of_mmap_;
    //NODE������
    size_t               num_of_node_;
    //FREE��NODE����
    size_t               sz_free_node_;
    //USE��NODE����
    size_t               sz_use_node_;
};


//AVL tree�������Ľڵ�
class _shm_avl_tree_index
{

public:
    _shm_avl_tree_index()
        : parent_(_shm_memory_base::_INVALID_POINT)
        , left_(_shm_memory_base::_INVALID_POINT)
        , right_(_shm_memory_base::_INVALID_POINT)
        , balanced_(0)
    {
    }

    _shm_avl_tree_index(const size_t &p, const size_t &l, const size_t &r, int8_t hb)
        : parent_(p)
        , left_(l)
        , right_(r)
        , balanced_(hb)
    {
    }

    ~_shm_avl_tree_index()
    {
    }

public:
    ///���ڵ�
    size_t       parent_;
    ///������
    size_t       left_;
    ///������
    size_t       right_;
    ///ƽ��
    int32_t      balanced_;
};


//AVL tree�ĵ�����
template < class _value_type,
         class _key_type,
         class _extract_key,
         class _compare_key >
class _shm_avl_tree_iterator
{
    typedef _shm_avl_tree_iterator<_value_type, _key_type, _extract_key, _compare_key> iterator;

    typedef shm_avl_tree<_value_type, _key_type, _extract_key, _compare_key> shm_avl_tree_t;


public:
    //���캯��
    _shm_avl_tree_iterator(size_t seq, shm_avl_tree_t *instance)
        : serial_(seq)
        , avl_tree_inst_(instance)
    {
    }

    _shm_avl_tree_iterator()
        : serial_(_shm_memory_base::_INVALID_POINT),
          avl_tree_inst_(NULL)
    {
    }

    ~_shm_avl_tree_iterator()
    {
    }

    //��ʼ��
    void initialize(size_t seq, shm_avl_tree_t *instance)
    {
        serial_ = seq;
        avl_tree_inst_ = instance;
    }

    //������žͿ����ٸ���ģ��ʵ���������ҵ���Ӧ����,����ʹ��ָ��
    size_t getserial() const
    {
        return serial_;
    }

    bool operator==(const iterator &x) const
    {
        return (serial_ == x.serial_ && avl_tree_inst_ == x.avl_tree_inst_);
    }
    bool operator!=(const iterator &x) const
    {
        return !(*this == x);
    }

    _value_type &operator*() const
    {
        return *(operator->());
    }

    //�ڶ��̵߳Ļ������ṩ������ͷ����ǲ���ȫ��,û�м���,�ϲ��Լ���֤
    _value_type *operator->() const
    {
        return avl_tree_inst_->getdatabase() + serial_;
    }

    iterator &operator++()
    {
        increment();
        return *this;
    }
    iterator operator++(int)
    {
        iterator tmp = *this;
        increment();
        return tmp;
    }

    iterator &operator--()
    {
        decrement();
        return *this;
    }
    iterator operator--(int)
    {
        iterator tmp = *this;
        decrement();
        return tmp;
    }

    ///����ʵ��operator++������һ�����Լ���(�ȽϺ�������)�Ľڵ�
    void increment()
    {
        if ((avl_tree_inst_->index_base_ + serial_)->right_ != _shm_memory_base::_INVALID_POINT)
        {
            //��������ӽڵ㣬�������ߣ�Ȼ��һֱ���������ߵ��׼���
            serial_ = (avl_tree_inst_->index_base_ + serial_)->right_;

            while ((avl_tree_inst_->index_base_ + serial_)->left_ != _shm_memory_base::_INVALID_POINT)
            {
                serial_ = (avl_tree_inst_->index_base_ + serial_)->left_;
            }
        }
        else
        {
            //���û�����ӽڵ㣬�ҵ����ڵ㣬�����ǰ�ڵ���ĳ�����ӽڵ㣬��һֱ���ݵ���Ϊ���ӽڵ�Ϊֹ
            size_t y = (avl_tree_inst_->index_base_ + serial_)->parent_;

            while (serial_ == (avl_tree_inst_->index_base_ + y)->right_)
            {
                serial_ = y;
                y = (avl_tree_inst_->index_base_ + y)->parent_;
            }

            //����ʱ�����ӽڵ㲻���ڸ��ڵ㣬�򸸽ڵ㼴�ǣ�������ǵ�ǰ�ڵ�
            if ((avl_tree_inst_->index_base_ + serial_)->right_ != y)
            {
                serial_ = y;
            }
        }
    }

    //����ʵ��operator--������һ�����Լ�С(�ȽϺ�������)�Ľڵ�
    void decrement()
    {
        //�����END��serial_����head_index_���±꣬��ô��right_����endǰ����Ǹ�
        if (avl_tree_inst_->avl_tree_head_.num_of_node_ == serial_)
        {
            serial_ = (avl_tree_inst_->index_base_ + serial_)->right_;
        }
        //��������ӽڵ�
        else if ((avl_tree_inst_->index_base_ + serial_)->left_ != _shm_memory_base::_INVALID_POINT)
        {
            //��yָ�����ӽڵ㣬�ҵ�y�����ӽڵ㣬�����ߵ��׼���
            size_t y = (avl_tree_inst_->index_base_ + serial_)->left_;

            while ((avl_tree_inst_->index_base_ + y)->right_ != _shm_memory_base::_INVALID_POINT)
            {
                y = (avl_tree_inst_->index_base_ + y)->right_;
            }

            serial_ = y;
        }
        else
        {
            //�ҳ����ڵ㣬�����ǰ�ڵ��Ǹ����ӽڵ㣬��һֱ���ݣ�ֱ������Ϊ���ӽڵ㣬����ĸ��ڵ㼴��
            size_t y = (avl_tree_inst_->index_base_ + serial_)->parent_;

            while (serial_ == (avl_tree_inst_->index_base_ + y)->left_)
            {
                serial_ = y;
                y = (avl_tree_inst_->index_base_ + y)->parent_;
            }

            serial_ = y;
        }
    }


protected:
    //���к�
    size_t          serial_;
    //RBtree��ʵ��ָ��
    shm_avl_tree_t  *avl_tree_inst_;

};


/*!
* @brief      AVL Tree����������������Ĵ���ȣ�
*
* @tparam     _value_type   ��������
* @tparam     _key_type     KEY������
* @tparam     _extract_key  �����_value_type�л�ȡ_key_type�ķ���
* @tparam     _compare_key  �ȽϷ���
*/
template < class _value_type,
         class _key_type,
         class _extract_key = smem_identity<_value_type>,
         class _compare_key = std::less<_key_type> >
class shm_avl_tree : public _shm_memory_base
{
public:
    //�����Լ�
    typedef shm_avl_tree < _value_type,
            _key_type,
            _extract_key,
            _compare_key > self;

    //���������
    typedef _shm_avl_tree_iterator < _value_type,
            _key_type,
            _extract_key,
            _compare_key > iterator;

    //��������Ԫ
    friend class _shm_avl_tree_iterator<_value_type, _key_type, _extract_key, _compare_key>;



public:

    //����ڹ����ڴ�ʹ��,û��new,����ͳһ��initialize ��ʼ��
    //�������,��������,���ǲ�������
    shm_avl_tree<_value_type, _key_type, _extract_key, _compare_key >(size_t numnode, void *pmmap, bool if_restore)
        : _shm_memory_base(NULL)
        , index_base_(NULL)
        , data_base_(NULL)
    {
    }

    shm_avl_tree<_value_type, _key_type, _extract_key, _compare_key >()
        : _shm_memory_base(NULL)
    {
    }

    ~shm_avl_tree<_value_type, _key_type, _extract_key, _compare_key >()
    {
    }

    //ֻ����,��ʵ��,���ⷸ��
    const self &operator=(const self &others);

    //�õ������Ļ�����ַ
    inline _shm_avl_tree_index *getindexbase()
    {
        return index_base_;
    }

    //�õ��������Ļ�������
    inline  _value_type *getdatabase()
    {
        return data_base_;
    }

protected:
    //����һ��NODE,�����FREELIST��ȡ��
    size_t create_node(const _value_type &val)
    {
        //���û�пռ���Է���
        if (avl_tree_head_->sz_free_node_ == 0)
        {
            return _INVALID_POINT;
        }

        //������ȡ1������
        size_t new_node = free_index_->right_;
        free_index_->right_ = (index_base_ + new_node)->right_;
        avl_tree_head_->sz_free_node_--;
        avl_tree_head_->sz_use_node_++;

        //��ʼ��
        (index_base_ + new_node)->parent_ = _INVALID_POINT;
        (index_base_ + new_node)->left_ = _INVALID_POINT;
        (index_base_ + new_node)->right_ = _INVALID_POINT;
        (index_base_ + new_node)->balanced_ = 0;

        new (data_base_ + new_node)_value_type(val);

        return new_node;
    }

    //�ͷ�һ��NODE,����黹��FREELIST
    void destroy_node(size_t pos)
    {
        size_t freenext = free_index_->right_;
        (index_base_ + pos)->right_ = freenext;
        free_index_->right_ = pos;
        avl_tree_head_->sz_free_node_++;
        avl_tree_head_->sz_use_node_--;
    }

public:

    //�ڴ����Ĺ���Ϊ ͷ��������,index��,data��,��������Ҫ�ĳ���,
    static size_t getallocsize(const size_t numnode)
    {
        return  sizeof(_shm_avl_tree_head)
                + sizeof(_shm_avl_tree_index) * (numnode + ADDED_NUM_OF_INDEX)
                + sizeof(_value_type) * numnode;
    }

    //��ʼ��
    static self *initialize(const size_t numnode, char *pmmap, bool if_restore = false)
    {
        //assert(pmmap!=NULL && numnode >0 );
        _shm_avl_tree_head *avl_tree_head = reinterpret_cast<_shm_avl_tree_head *>(pmmap);

        //����ǻָ�,���ݶ����ڴ���,
        if (true == if_restore)
        {
            //���һ�»ָ����ڴ��Ƿ���ȷ,
            if (getallocsize(numnode) != avl_tree_head->size_of_mmap_ ||
                numnode != avl_tree_head->num_of_node_)
            {
                return NULL;
            }
        }

        //��ʼ���ߴ�
        avl_tree_head->size_of_mmap_ = getallocsize(numnode);
        avl_tree_head->num_of_node_ = numnode;

        self *instance = new self();

        //���е�ָ�붼�Ǹ��ӻ���ַ����õ���,���ڷ������,ÿ�γ�ʼ�������¼���
        instance->smem_base_ = pmmap;
        //ͷ��
        instance->avl_tree_head_ = avl_tree_head;
        //������
        instance->index_base_ = reinterpret_cast<_shm_avl_tree_index *>(
                                    pmmap +
                                    sizeof(_shm_avl_tree_head));
        //������
        instance->data_base_ = reinterpret_cast<_value_type *>(
                                   pmmap +
                                   sizeof(_shm_rb_tree_head) +
                                   sizeof(_shm_avl_tree_index) * (numnode + ADDED_NUM_OF_INDEX));

        //��ʼ��free_index_,head_index_
        instance->head_index_ = reinterpret_cast<_shm_avl_tree_index *>(
                                    pmmap +
                                    sizeof(_shm_avl_tree_head) +
                                    sizeof(_shm_avl_tree_index) * (numnode));
        instance->free_index_ = reinterpret_cast<_shm_avl_tree_index *>(
                                    pmmap +
                                    sizeof(_shm_avl_tree_head) +
                                    sizeof(_shm_avl_tree_index) * (numnode + 1));

        if (false == if_restore)
        {
            //�����ʼ�����е��ڴ�,���еĽڵ�ΪFREE
            instance->clear();
        }

        return instance;
    }

    //�����ʼ�����е��ڴ�,���еĽڵ�ΪFREE
    void clear()
    {
        //����2���ؼ�Node,�Լ���س���,��ʼ���е�������free.
        avl_tree_head_->sz_free_node_ = avl_tree_head_->num_of_node_;
        avl_tree_head_->sz_use_node_ = 0;

        //������ΪNULL,��ָ�붼ָ���Լ�
        head_index_->parent_ = _INVALID_POINT;
        head_index_->right_ = avl_tree_head_->num_of_node_;
        head_index_->left_ = avl_tree_head_->num_of_node_;
        head_index_->balanced_ = 0;

        //
        free_index_->left_ = _INVALID_POINT;
        free_index_->parent_ = _INVALID_POINT;
        free_index_->balanced_ = 0;

        //��right_������FREE NODE���б�
        free_index_->right_ = 0;

        //��ʼ��free������
        _shm_avl_tree_index *pindex = index_base_;
        for (size_t i = 0; i < avl_tree_head_->num_of_node_; ++i)
        {
            pindex->right_ = (i + 1);

            //������FREENODE������
            if (i == avl_tree_head_->num_of_node_ - 1)
            {
                pindex->right_ = avl_tree_head_->num_of_node_ + 1;
            }

            pindex++;
        }
    }

    //�ҵ���һ���ڵ�
    iterator begin()
    {
        return iterator(head_index_->left_, this);
    };

    //����Ӧ����ǰ�պ󿪵�,ͷ�ڵ���Ϊ���һ��index
    iterator end()
    {
        return iterator(avl_tree_head_->num_of_node_, this);
    }

    //���нڵ㶼��free���ϼ��ǿ�
    bool empty()
    {
        if (avl_tree_head_->sz_free_node_ == avl_tree_head_->num_of_node_)
        {
            return true;
        }

        return false;
    }

    //�ڲ�������ǰ����,����������
    bool full()
    {
        if (avl_tree_head_->sz_free_node_ == 0)
        {
            return true;
        }

        return false;
    };

    size_t size() const
    {
        return avl_tree_head_->sz_use_node_;
    }

    size_t capacity() const
    {
        return avl_tree_head_->num_of_node_;
    }

    //���еĽڵ����
    size_t sizefree()
    {
        return avl_tree_head_->sz_free_node_;
    }

protected:
    inline size_t  &header() const
    {
        return avl_tree_head_->num_of_node_;
    }

    inline size_t  &root() const
    {
        return head_index_->parent_;
    }

    inline size_t  &leftmost() const
    {
        return head_index_->left_;
    }

    inline size_t  &rightmost() const
    {
        return head_index_->right_;
    }

    inline size_t  &left(size_t x)
    {
        return (index_base_ + x)->left_;
    }

    inline size_t  &right(size_t x)
    {
        return (index_base_ + x)->right_;
    }

    inline size_t  &parent(size_t x)
    {
        return (index_base_ + x)->parent_;
    }

    inline int32_t &balanced(size_t x)
    {
        return (index_base_ + x)->balanced_;
    }

    inline const _value_type  &value(size_t x)
    {
        return *(data_base_ + x);
    }

    inline const _key_type  &key(size_t x)
    {
        return _extract_key()(value(x));
    }

    //ȡ����ֵ
    size_t minimum(size_t x)
    {
        while (left(x) != _INVALID_POINT)
        {
            x = left(x);
        }

        return x;
    }

    //ȡ��Сֵ
    size_t maximum(size_t x)
    {
        while (right(x) != _INVALID_POINT)
        {
            x = right(x);
        }

        return x;
    }

protected:


    /*!
    * @brief      �����Ĳ����������������ɵ�
    * @return     std::pair<iterator, bool> ���صĲ���ṹ�������������ͽ��
    * @param      x   �����,�󲿷�ʱ��Ϊ_INVALID_POINT
    * @param      y   �����ĸ��ڵ�
    * @param      val ���������
    */
    std::pair<iterator, bool> _insert(size_t x, size_t y, const _value_type &val)
    {
        //����һ���ռ�
        size_t z = create_node(val);
        //�չ��ռ䲻�㣬�޷����룬����end,false��pair
        if (_INVALID_POINT == z)
        {
            return std::pair<iterator, bool>(iterator(_INVALID_POINT, this), false);
        }

        //�Ѵ˶��������ȥ�����ҵ������ֶ���

        //���1.�������root�ڵ㣬2.�������ڵ㲻�ǿսڵ㣬3.����Ƚ�ΪTRUE
        if (y == header() || x != _INVALID_POINT || _compare_key()(_extract_key()(val), key(y)))
        {
            left(y) = z;

            if (y == header())
            {
                root() = z;
                rightmost() = z;
            }
            //���Y����Сֵ�������Сֵ��ΪY
            else if (y == leftmost())
            {
                leftmost() = z;
            }
        }
        else
        {
            right(y) = z;

            if (y == rightmost())
            {
                rightmost() = z;
            }
        }

        parent(z) = y;
        left(z) = _INVALID_POINT;
        right(z) = _INVALID_POINT;

        //������Ǹ��ڵ㣬���ǽ���ƽ�����
        if (y != header())
        {
            _balance_adjust(z,true);
        }

        return   std::pair<iterator, bool>(iterator(z, this), true);
    }



    /*!
    * @brief      ����ƽ��������ڲ�������
    * @param[in]  z ����Ľڵ�λ��
    * @param[in]  if_inssert �Ƿ��ǲ���������е����������ɾ����������дfalse
    */
    void _balance_adjust(size_t z,bool if_inssert)
    {
        //��ʵ����ط�ֱ��ʹ�ó�������������һ��,������û����ö�ٻ��ߺ�

        //�ҵ���С�Ĳ�ƽ��ĵ�,
        size_t s = z;
        size_t t = _INVALID_POINT;
        int32_t mod_balance = 0;
        while (s != header())
        {
            t = s;
            s = parent(s);
            if (if_inssert)
            {
                mod_balance = (t == left(s)) ? 1 : -1;
            }
            else
            {
                mod_balance = (t == left(s)) ? -1 : 1;
            }

            //�����ƽ��ģ��޸�ƽ��������������ϸɻ�
            if (0  == balanced(s) )
            {
                balanced(s) += mod_balance;
                continue;
            }
            //�������ԭ���Ͳ�ƽ�⣬�ҵ���С�Ĳ�ƽ������������ת������ƽ��
            else
            {
                balanced(s) += mod_balance;
                //���ݲ�ƽ����������������ʲô������ת
                if (2 == balanced(s) )
                {
                    if (1 == balanced(t))
                    {
                        _ll_rotate(s, t);
                    }
                    else
                    {
                        _lr_rotate(s, t, right(t));
                    }
                }
                else if (-2 == balanced(s))
                {
                    if (1 == balanced(t))
                    {
                        _rl_rotate(s, t, left(t));
                    }
                    else
                    {
                        _rr_rotate(s, t);
                    }
                }
                break;
            }
        }
        return ;
    }

    /*!
    * @brief      LL��ת��
    * @param      p   ���ڵ㣬��С�Ĳ�ƽ�����ĸ��ڵ�
    * @param      lc  ��ߵ��ӽڵ�
    */
    void _ll_rotate(size_t p, size_t lc)
    {
        size_t gf = parent(p);
        parent(p) = lc;
        left(p) = right(lc);
        right(lc) = p;
        parent(lc) = gf;

        //����ƽ������
        balanced(p) = 0;
        balanced(lc) = 0;

        //����p�ĸ��ڵ����������������ָ���µ������¸�
        if (left(gf) == p)
        {
            left(gf) = lc;
        }
        else
        {
            right(gf) = lc;
        }
    }

    /*!
    * @brief      LR��ת
    * @param      p   ���ڵ㣬��С�Ĳ�ƽ�����ĸ��ڵ�
    * @param      lc  p�����ӽڵ�
    * @param      rgs lc�����ӽڵ�
    */
    void _lr_rotate(size_t p, size_t lc, size_t rgs)
    {
        size_t gf = parent(p);
        parent(p) = rgs;
        left(p) = right(rgs);
        parent(lc) = rgs;
        right(lc) = left(rgs);
        left(rgs) = lc;
        right(rgs) = p;

        //����ƽ������
        balanced(p) = -1;
        balanced(lc) = 0;
        balanced(rgs) = 0;

        //����p�ĸ��ڵ����������������ָ���µ������¸�
        if (left(gf) == p)
        {
            left(gf) = rgs;
        }
        else
        {
            right(gf) = rgs;
        }
    }

    /*!
    * @brief      RR��ת��
    * @param      p   ���ڵ㣬��С�Ĳ�ƽ�����ĸ��ڵ�
    * @param      rc  �ұߵ��ӽڵ�
    */
    void _rr_rotate(size_t p, size_t rc)
    {
        size_t gf = parent(p);
        parent(p) = rc;
        right(p) = left(rc);
        left(rc) = p;
        parent(rc) = gf;

        //����ƽ������
        balanced(p) = 0;
        balanced(rc) = 0;

        //����p�ĸ��ڵ����������������ָ���µ������¸�
        if (left(gf) == p)
        {
            left(gf) = rc;
        }
        else
        {
            right(gf) = rc;
        }
    }

    void _rl_rotate(size_t p, size_t rc, size_t lgs)
    {
        size_t gf = parent(p);
        parent(p) = lgs;
        right(p) = left(lgs);
        parent(rc) = lgs;
        left(rc) = right(lgs);
        left(lgs) = p;
        right(lgs) = rc;


        //����ƽ������
        balanced(p) = 0;
        balanced(rc) = -1;
        balanced(lgs) = 0;

        //����p�ĸ��ڵ����������������ָ���µ������¸�
        if (left(gf) == p)
        {
            left(gf) = lgs;
        }
        else
        {
            right(gf) = lgs;
        }
    }

public:

    /*!
    * @brief      �����ظ�key����Ĳ��뺯����Multimap��Multimap�����
    *             ��Ϊ�ռ�����������Է��ص������л�����bool��pair
    * @return     std::pair<iterator, bool>  ���ص�iteratorΪ��������boolΪ�Ƿ����ɹ�
    * @param      v        �����_value_type������
    */
    std::pair<iterator, bool> insert_equal(const _value_type &v)
    {
        //����������ˣ�Ҳ����ʧ��
        if (avl_tree_head_->sz_free_node_ == 0)
        {
            return std::pair<iterator, bool>(iterator(_INVALID_POINT, this), false);
        }

        size_t y = header();
        size_t x = root();

        //���뵽һ���սڵ���
        while (x != _INVALID_POINT)
        {
            y = x;
            x = _compare_key()(_extract_key()(v), key(x)) ? left(x) : right(x);
        }

        return _insert(x, y, v);
    }

    /*!
    * @brief      �ظ�key������ʧ�ܵĲ��뺯����Map��Sap�����
    * @return     std::pair<iterator, bool> ���ص�iteratorΪ��������boolΪ�Ƿ����ɹ�
    * @param      v �����_value_type������
    */
    std::pair<iterator, bool> insert_unique(const _value_type &v)
    {
        //����������ˣ�Ҳ����ʧ��
        if (avl_tree_head_->sz_free_node_ == 0)
        {
            return std::pair<iterator, bool>(iterator(_INVALID_POINT, this), false);
        }

        size_t y = header();
        size_t x = root();
        bool comp = true;

        //����Ƚ�(������<)����true,�����󣬷���(>=)�����ң�
        while (x != _INVALID_POINT)
        {
            y = x;
            comp = _compare_key()(_extract_key()(v), key(x));
            x = comp ? left(x) : right(x);
        }

        iterator j = iterator(y, this);

        if (comp)
        {
            if (j == begin())
            {
                return _insert(x, y, v);
            }
            else
            {
                --j;
            }
        }

        if (_compare_key()(key(j.getserial()), _extract_key()(v)))
        {
            return _insert(x, y, v);
        }

        //����Ȳ���>,�ֲ���<����ô����==,��ô���ش���
        return std::pair<iterator, bool>(j, false);
    }

    //ͨ��������ɾ��һ���ڵ�
    iterator erase(const iterator &pos)
    {
        //x,Ϊɾ����λ�ã�yΪX�ĸ��ڵ㣬z����Ϊ�滻x�Ľڵ�
        size_t x = pos.getserial();
        size_t y = parent(x);
        size_t z = _INVALID_POINT;

        //Ĭ����������ֵ
        if (right(x) != _INVALID_POINT)
        {
            z = maximum(right(x));
        }
        else if (left(x) != _INVALID_POINT)
        {
            z = minimum(left(x));

        }
        else
        {
            z = _INVALID_POINT;
        }

        //
        if (left(y) == x)
        {
            left(y) = z;
            left(z) = left(x);
            right(z) = right(x);
            if (y == header())
            {
                root() = z;
                rightmost() = z;
            }
            //���Y����Сֵ�������Сֵ��ΪY
            else if (x == leftmost())
            {
                leftmost() = z;
            }
        }
        else
        {
            right(y) = z;
            left(z) = left(x);
            right(z) = right(x);
            if (x == rightmost())
            {
                rightmost() = z;
            }
        }

        //������Ǹ��ڵ㣬���ǽ���ƽ�����
        if (y != header())
        {
            _balance_adjust(z, false);
        }
        destroy_node(x);

        return iterator(z,this);
    }

    //ͨ��keyɾ���ڵ㣬Map��Set��
    size_t erase_unique(const _key_type &k)
    {

        iterator find_iter = find(k);
        //û���ҵ���Ӧ�Ľڵ㣬ɾ��ʧ��
        if (find_iter == end())
        {
            return 0;
        }

        erase(find_iter);

        return 1;
    }

    //ͨ��valueɾ���ڵ㣬Map��Set��
    size_t erase_unique_value(const _value_type &v)
    {
        _extract_key get_key;
        return erase_unique(get_key(v));
    }

    //ͨ��keyɾ���ڵ㣬Multimap��Multiset��
    size_t erase_equal(const _key_type &k)
    {

        iterator find_iter = find(k);
        //û���ҵ���Ӧ�Ľڵ㣬ɾ��ʧ��
        if (find_iter == end())
        {
            return 0;
        }
        
        iterator j = erase(find_iter);
        if (j == end())
        {
            return 1;
        }
        
        size_t erase_count = 1;
        while (_compare_key()(k, key(j.getserial())) &&
            _compare_key()(key(j.getserial()), k))
        {
            j = erase(j);
            ++erase_count;
        }
        return erase_count;
    }

    //ͨ��ֵɾ���ڵ㣬Multimap��Multiset��
    size_t erase_equal_value(const _value_type &v)
    {
        _extract_key get_key;
        return erase_equal(get_key(v));
    }

    //��key��ͬ�Ľڵ�
    iterator find(const _key_type &k)
    {
        size_t y = header();
        size_t x = root();

        while (x != _INVALID_POINT)
        {
            if (!_compare_key()(key(x), k))
            {
                y = x;
                x = left(x);
            }
            else
            {
                x = right(x);
            }
        }

        iterator j = iterator(y, this);
        //ע�����ε���_compare_key�ıȽϲ���˳���
        return (j == end() || _compare_key()(k, key(j.getserial()))) ? end() : j;
    }

    //��value��ͬ�Ľڵ�
    iterator find_value(const _value_type &v)
    {
        _extract_key get_key;
        return find(get_key(v));
    }

    //��value��ͬ�Ľڵ㣬��δ�ҵ������
    _value_type &find_or_insert(const _value_type &v)
    {
        iterator iter = find_value(v);

        if (iter == end())
        {
            std::pair<iterator, bool> pair_iter = insert(v);
            return (*(pair_iter.first));
        }

        return *iter;
    }

protected:
    //index��Ҫ������������,һ����ͷָ�룬һ���ǿսڵ��ͷָ��
    static const size_t ADDED_NUM_OF_INDEX = 2;

protected:
    ///RBTreeͷ��
    _shm_avl_tree_head            *avl_tree_head_;

    ///���е�ָ�붼�Ǹ��ݻ���ַ����õ���,���ڷ������,ÿ�γ�ʼ�������¼���
    ///����������,
    _shm_avl_tree_index            *index_base_;

    ///��������ʼָ��,
    _value_type                   *data_base_;

    ///ͷ�ڵ��ͷָ��,N+1������λ��ʾ
    _shm_avl_tree_index            *head_index_;

    ///�սڵ��ͷָ��,N+2������λ��ʾ����������right�ڵ������ӣ��ѿսڵ㴮������
    _shm_avl_tree_index            *free_index_;

};

//��AVL Treeʵ��SET��������multiset��set��ͨ����ͨ��insert�Լ�����
template < class _value_type,
         class _compare_key = std::less<_value_type> >
class mmap_avl_set :
    public shm_avl_tree< _value_type, _value_type, smem_identity<_value_type>, _compare_key >
{
protected:
    //����ڹ����ڴ�ʹ��,û��new,����ͳһ��initialize ��ʼ��
    //�������,��������,���ǲ�������
    mmap_avl_set<_value_type, _compare_key >(size_t numnode, void *pmmap, bool if_restore) :
        shm_avl_tree<_value_type, _value_type, smem_identity<_value_type>, _compare_key>(numnode, pmmap, if_restore)
    {
        initialize(numnode, pmmap, if_restore);
    }

    ~mmap_avl_set<_value_type, _compare_key >()
    {
    }

public:

    static mmap_avl_set< _value_type, _compare_key  >*
    initialize(size_t &numnode, char *pmmap, bool if_restore = false)
    {
        return reinterpret_cast<mmap_set< _value_type, _compare_key  >*>(
                   shm_avl_tree < _value_type,
                   _value_type,
                   smem_identity<_value_type>,
                   _compare_key >::initialize(numnode, pmmap, if_restore));
    }
};

//��AVL Treeʵ��MAP��������multiset��set��ͨ����ͨ��insert�Լ�����
template < class _key_type,
         class _value_type,
         class _extract_key = mmap_select1st <std::pair <_key_type, _value_type> >,
         class _compare_key = std::less<_value_type>  >
class mmap_avl_map :
    public shm_avl_tree< std::pair <_key_type, _value_type>, _key_type, _extract_key, _compare_key  >
{
protected:
    //����ڹ����ڴ�ʹ��,û��new,����ͳһ��initialize ��ʼ��
    //�������,��������,���ǲ�������
    mmap_avl_map<_key_type, _value_type, _extract_key, _compare_key >(size_t numnode, void *pmmap, bool if_restore) :
        shm_avl_tree< std::pair <_key_type, _value_type>, _key_type, _extract_key, _compare_key  >(numnode, pmmap, if_restore)
    {
        initialize(numnode, pmmap, if_restore);
    }

    ~mmap_avl_map<_key_type, _value_type, _extract_key, _compare_key >()
    {
    }
public:
    static mmap_avl_map< _key_type, _value_type, _extract_key, _compare_key  >*
    initialize(size_t &numnode, char *pmmap, bool if_restore = false)
    {
        return reinterpret_cast < mmap_avl_map < _key_type,
               _value_type,
               _extract_key,
               _compare_key  > * > (
                   shm_avl_tree < std::pair < _key_type,
                   _value_type > ,
                   _key_type,
                   _extract_key,
                   _compare_key >::initialize(numnode, pmmap, if_restore));
    }
    //[]�����������ŵ��ȱ�㣬����ʹ��
    _value_type &operator[](const _key_type &key)
    {
        return (find_or_insert(std::pair<_key_type, _value_type >(key, _value_type()))).second;
    }
};


};





#endif //ZCE_LIB_SHM_AVL_TREE_H_

