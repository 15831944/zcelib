/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_thread_msgque_nonlock.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       Saturday, December 28, 2013
* @brief      
*             
*             
* @details    
*             
*             
*             
* @note       
*             
*/

#ifndef ZCE_LIB_THREAD_MESSAGE_QUEUE_NONLOCK_H_
#define ZCE_LIB_THREAD_MESSAGE_QUEUE_NONLOCK_H_

#include "zce_lock_synch_traits.h"
#include "zce_thread_msgque_template.h"

//ʹ��ƫ�ػ��õ�һ��ZCE_Message_Queue
template < typename _value_type,
         typename _container_type >
class ZCELIB_EXPORT ZCE_Message_Queue<ZCE_NULL_SYNCH, _value_type, _container_type> : public ZCE_NON_Copyable
{
protected:

    //    ZCE_NULL_SYNCH��ʵû��ʹ�ã���Ϊû�и���

    //QUEUE�����ߴ�
    std::size_t                                    queue_max_size_;

    //����LIST��size()�����ȽϺ�ʱ����������������˸�������������ֱ��ʹ��_container_type.size()
    std::size_t                                    queue_cur_size_;

    //�������ͣ�������list,dequeue,
    _container_type                       message_queue_;

public:

    //
    ZCE_Message_Queue(size_t queue_max_size):
        queue_max_size_(queue_max_size),
        queue_cur_size_(0)
    {
    }

    ~ZCE_Message_Queue()
    {
    }

    //QUEUE�Ƿ�ΪNULL
    inline bool empty()
    {
        if (queue_cur_size_ == 0)
        {
            return true;
        }

        return false;
    }

    //QUEUE�Ƿ�Ϊ��
    inline bool full()
    {
        if (queue_cur_size_ == queue_max_size_)
        {
            return true;
        }

        return false;
    }

    //��������
    int enqueue(const _value_type &value_data)
    {
        if (queue_cur_size_ >= queue_max_size_)
        {
            return -1;
        }

        message_queue_.push_back(value_data);
        ++queue_cur_size_;

        return 0;
    }

    //����һ�����ݣ����г�ʱ�ȴ�
    int enqueue(const _value_type &value_data,
                ZCE_Time_Value & )
    {
        return enqueue(value_data);
    }

    //
    int dequeue(_value_type &value_data)
    {
        if (queue_cur_size_ == 0)
        {
            return -1;
        }

        //
        value_data = *message_queue_.begin();
        message_queue_.pop_front();
        --queue_cur_size_;
        return 0;
    }

    int dequeue(_value_type &value_data,
                ZCE_Time_Value & )
    {
        return dequeue(value_data);
    }

    //������Ϣ����
    void clear()
    {
        message_queue_.clear();
        queue_cur_size_ = 0;
    }

    //������Ϣ����ĳߴ�
    size_t size()
    {
        return queue_cur_size_;
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��10��8��
Template Param  :
  Param1: typename _value_type
Class           : ZCE_Message_Queue_List <ZCE_NULL_SYNCH,_value_type>
Inherit         : public ZCE_Message_Queue<ZCE_NULL_SYNCH,_value_type,std::list<_value_type> >
Description     : ��LIST��Ϊ������
Other           :
Modify Record   :
************************************************************************************************************/
template <typename _value_type >
class ZCE_Message_Queue_List <ZCE_NULL_SYNCH, _value_type>  : public ZCE_Message_Queue<ZCE_NULL_SYNCH, _value_type, std::list<_value_type> >
{
public:
    //
    ZCE_Message_Queue_List(size_t queue_max_size):
        ZCE_Message_Queue<ZCE_NULL_SYNCH, _value_type, std::list<_value_type> >(queue_max_size)
    {
    }

    ~ZCE_Message_Queue_List()
    {
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��10��8��
Template Param  :
  Param1: typename _value_type
Class           : ZCE_Message_Queue_Deque <ZCE_NULL_SYNCH,_value_type>
Inherit         : public ZCE_Message_Queue<ZCE_NULL_SYNCH,_value_type,std::deque<_value_type> >
Description     :
Other           :
Modify Record   :
************************************************************************************************************/
template <typename _value_type >
class ZCE_Message_Queue_Deque <ZCE_NULL_SYNCH, _value_type>: public ZCE_Message_Queue<ZCE_NULL_SYNCH, _value_type, std::deque<_value_type> >
{
public:
    //
    ZCE_Message_Queue_Deque(size_t queue_max_size):
        ZCE_Message_Queue<ZCE_NULL_SYNCH, _value_type, std::deque<_value_type> >(queue_max_size)
    {
    }

    ~ZCE_Message_Queue_Deque()
    {
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  :
  Param1: typename _value_type ��Ϣ���б������������
Class           : zce_condi_msgqueue_ring
Inherit         : public ZCE_Message_Queue_Semaphore<_value_type,boost::circular_buffer<_value_type> >
Description     : �ڲ���circular_bufferʵ�ֵ���Ϣ���У����ܷǳ���,�߽籣�����źŵƣ�����Ϣ���У����ռ�ȽϷ�
Other           : �����װ����Ҫ��������Ϊ�˸����﷨�ǣ�������Ϊ�˼�������
Modify Record   :
************************************************************************************************************/
template <typename _value_type >
class ZCE_Message_Queue_Rings<ZCE_NULL_SYNCH, _value_type>  : public ZCE_Message_Queue<ZCE_NULL_SYNCH, _value_type, ZCE_LIB::lordrings<_value_type> >
{
public:
    //
    ZCE_Message_Queue_Rings(size_t queue_max_size):
        ZCE_Message_Queue<ZCE_NULL_SYNCH, _value_type, ZCE_LIB::lordrings<_value_type> >(queue_max_size)
    {
        ZCE_Message_Queue<ZCE_NULL_SYNCH, _value_type, ZCE_LIB::lordrings<_value_type> >::message_queue_.resize(queue_max_size);
    }

    ~ZCE_Message_Queue_Rings()
    {
    }
};

#endif // ZCE_LIB_THREAD_MESSAGE_QUEUE_NONLOCK_H_

