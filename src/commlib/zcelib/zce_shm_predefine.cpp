#include "zce_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_shm_predefine.h"


namespace ZCE_LIB
{

//���ʵ��ķŴ���
static size_t zoomin_nodenum(size_t node_num)
{
    size_t zoomin_num = 0;
    //������������ݣ����ʵ��ķŴ��ٽ��д���
    //�������10�ڣ�����1�ڣ��ٿ�ʼ�������������
    if (node_num > 1000000000ul)
    {
        zoomin_num = node_num + 100000000ul;
    }
    else if (node_num > 100000000ul)
    {
        zoomin_num = node_num + 10000000ul;
    }
    else  if (node_num > 10000000ul)
    {
        zoomin_num = node_num + 1000000ul;
    }
    else  if (node_num > 1000000ul)
    {
        zoomin_num = node_num + 100000ul;
    }
    else  if (node_num > 660000ul)
    {
        zoomin_num = node_num + 30000ul;
    }
    else  if (node_num > 330000ul)
    {
        zoomin_num = node_num + 20000ul;
    }
    else  if (node_num > 100000ul)
    {
        zoomin_num = node_num + 15000ul;
    }
    else  if (node_num > 66000ul)
    {
        zoomin_num = node_num + 5000ul;
    }
    else  if (node_num > 33000ul)
    {
        zoomin_num = node_num + 3000ul;
    }
    else  if (node_num > 10000ul)
    {
        zoomin_num = node_num + 2000ul;
    }
    else
    {
        zoomin_num = node_num + 500ul;
    }
    return zoomin_num;
}

//
static size_t nearest_prime(const size_t node_num)
{
    ZCE_ASSERT(node_num > 3);

    size_t test_num = node_num;
    for (;; ++test_num)
    {
        size_t test_max = static_cast<size_t>(::sqrt(static_cast<double>(test_num)));
        bool bool_prime = true;
        for (size_t i = 2; i <= test_max + 1; ++i)
        {
            if (test_num % i == 0)
            {
                bool_prime = false;
                break;
            }
        }
        if (bool_prime)
        {
            break;
        }
    }
    return test_num;
}


};


//
void ZCE_LIB::hash_prime(const size_t node_num, size_t &real_num)
{
    real_num = 0;
#if SUPPORT_BIGBIG_WORLD == 0
    if (node_num > 4294967291ul - 100000000ul)
    {
        real_num = 4294967291ul;
        return;
    }
#else

#endif
    
    //��һ���ķŴ���
    size_t zoomin_num = ZCE_LIB::zoomin_nodenum(node_num);
    real_num = nearest_prime(zoomin_num);
    return;
}

//
void ZCE_LIB::hash_prime_ary(const size_t num_node,size_t &real_num, size_t row, size_t prime_ary[])
{
    //��һ���ķŴ���
    size_t zoomin_num = ZCE_LIB::zoomin_nodenum(num_node);
    size_t per_row_num = zoomin_num/row + 1;

    //����һ���������У�
    size_t test_num = per_row_num;
    for (size_t i = 0; i < row; ++i)
    {
        prime_ary[i] = nearest_prime(test_num);
        test_num = prime_ary[i];
    }

    //�ܼƷ������һ����
    real_num = 0;
    for (size_t i = 0; i < row; ++i)
    {
        real_num += prime_ary[i];
    }
}