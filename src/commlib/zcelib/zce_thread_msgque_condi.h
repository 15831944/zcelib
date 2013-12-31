/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_thread_msgque_condi.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       2011��6��17��
* @brief      ���߳�ʹ�õ�Message Queue���ڲ���ͬ������ʹ������������
*             �������ʱ��Ҫʹ�ã�����������ϵ���ѽڵģ���ȫ��Ϊ�˲�����������д����
*             ͬʱ֧��list��deque,���Լ���һ������ZCE_LIB::lordrings ���ζ���
* @details    
*             
*             
*             
* @note       
*             
*/


#ifndef ZCE_LIB_THREAD_MESSAGE_QUEUE_CONDITION_H_
#define ZCE_LIB_THREAD_MESSAGE_QUEUE_CONDITION_H_

#include "zce_boost_non_copyable.h"
#include "zce_lock_thread_mutex.h"
#include "zce_lock_thread_condi.h"

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  :
  Param1: class _value_type ��Ϣ���з������������
  Param2: class _container_type =std::deque<_value_type> ��Ϣ�����ڲ���������
Class           : ZCE_Message_Queue_Condi
Inherit         : public boost::noncopyable ���ܸ��ƿ���
Description     : ����������+����ʵ�ֵ���Ϣ���У������Ҹ�����˵�����������е�֣�װB������condi����Mutex��Ŀ���ǣ�
Other           :
Modify Record   :
************************************************************************************************************/
template <typename _value_type, typename _container_type = std::deque<_value_type> >
class ZCE_Message_Queue_Condi : public ZCE_NON_Copyable
{
protected:

    //QUEUE�����ߴ�
    std::size_t                  queue_max_size_;

    //����LIST��size()�����ȽϺ�ʱ����������������˼���������
    std::size_t                  queue_cur_size_;

    //���е�LOCK,���ڶ�д������ͬ������
    ZCE_Thread_Light_Mutex       queue_lock_;

    //���뱣������������
    ZCE_Thread_Condition_Mutex   cond_enqueue_;

    //ȡ�����б�������������
    ZCE_Thread_Condition_Mutex   cond_dequeue_;

    //�������ͣ�������list,dequeue,
    _container_type              message_queue_;

public:

    //���캯������������
    ZCE_Message_Queue_Condi(size_t queue_max_size):
        queue_max_size_(queue_max_size),
        queue_cur_size_(0)
    {
    }

    ~ZCE_Message_Queue_Condi()
    {
    }

    //QUEUE�Ƿ�ΪNULL
    inline bool empty()
    {
        ZCE_Thread_Light_Mutex::LOCK_GUARD guard(queue_lock_);

        if (queue_cur_size_ == 0)
        {
            return true;
        }

        return false;
    }

    //QUEUE�Ƿ�Ϊ��
    inline bool full()
    {
        ZCE_Thread_Light_Mutex::LOCK_GUARD guard(queue_lock_);

        if (queue_cur_size_ == queue_max_size_)
        {
            return true;
        }

        return false;
    }

    //����
    int enqueue(const _value_type &value_data)
    {
        ZCE_Time_Value  nouse_timeout;
        return enqueue(value_data,
                       false,
                       nouse_timeout);
    }

    //�г�ʱ����
    int enqueue(const _value_type &value_data,
                const ZCE_Time_Value  &wait_time)
    {
        return enqueue(value_data,
                       true,
                       wait_time);
    }

    //ȡ��
    int dequeue(_value_type &value_data)
    {
        ZCE_Time_Value  nouse_timeout;
        return dequeue(value_data,
                       false,
                       nouse_timeout);
    }

    //�г�ʱ�����ȡ��
    int dequeue(_value_type &value_data,
                const ZCE_Time_Value  &wait_time)
    {
        return dequeue(value_data,
                       true,
                       wait_time);
    }

    void clear()
    {
        ZCE_Thread_Light_Mutex::LOCK_GUARD guard(queue_lock_);
        message_queue_.clear();
        queue_cur_size_ = 0;
    }

    size_t size()
    {
        ZCE_Thread_Light_Mutex::LOCK_GUARD guard(queue_lock_);
        return queue_cur_size_;
    }

protected:

    //����һ�����ݣ����ݲ���ȷ���Ƿ�ȴ�һ�����ʱ��
    int enqueue(const _value_type &value_data,
                bool if_wait_timeout,
                const timeval &wait_time)
    {
        //ע����δ��������{}��������Ϊ������ȱ�֤���ݷ��룬�ٴ���������
        //������������ʵ�ڲ��ǽ⿪�˱�����
        {
            ZCE_Thread_Light_Mutex::LOCK_GUARD guard(queue_lock_);
            bool bret = false;

            //cond�������Ƿǳ�����ģ�����ģ�����ط�������while������������
            //��ϸ��pthread_condi��˵����
            while (queue_cur_size_ == queue_max_size_)
            {
                if (if_wait_timeout)
                {
                    //timed_wait�����������Ŀ����Ϊ�˽⿪���˳���ʱ����ϣ������Ǽ�����
                    //���Ժ���ܺ���,WINDOWS�µ�ʵ��Ӧ�������źŵ�ģ���
                    bret = cond_enqueue_.duration_wait(&queue_lock_, wait_time);

                    //�����ʱ�ˣ�����false
                    if (!bret)
                    {
                        return -1;
                    }
                }
                else
                {
                    cond_enqueue_.wait(&queue_lock_);
                }

            }

            message_queue_.push_back(value_data);
            ++queue_cur_size_;

        }

        //֪ͨ���еȴ�����
        cond_dequeue_.broadcast();

        return 0;
    }

    //ȡ��һ�����ݣ����ݲ���ȷ���Ƿ�ȴ�һ�����ʱ��
    int dequeue(_value_type &value_data,
                bool if_wait_timeout,
                const ZCE_Time_Value  &wait_time)
    {
        //ע����δ��������{}��������Ϊ������ȱ�֤����ȡ��
        {
            ZCE_Thread_Light_Mutex::LOCK_GUARD guard(queue_lock_);
            bool bret = false;

            //cond�������Ƿǳ�����ģ�����ģ�����ط�������while��
            //��ϸ��pthread_condi��˵����
            while (queue_cur_size_ == 0)
            {
                //�ж��Ƿ�Ҫ���г�ʱ�ȴ�
                if (if_wait_timeout)
                {
                    //timed_wait�����������Ŀ����Ϊ�˽⿪���˳���ʱ����ϣ������Ǽ�����
                    //���Ժ���ܺ���
                    bret = cond_dequeue_.duration_wait(&queue_lock_, wait_time);

                    //�����ʱ�ˣ�����false
                    if (!bret)
                    {
                        return -1;
                    }
                }
                else
                {
                    cond_dequeue_.wait(&queue_lock_);
                }
            }

            //
            value_data = *message_queue_.begin();
            message_queue_.pop_front();
            --queue_cur_size_;
        }

        //֪ͨ���еȴ�����
        cond_enqueue_.broadcast();

        return 0;
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  :
  Param1: class _value_type ��Ϣ���б������������
Class           : ZCE_Msgqueue_List_Condi
Inherit         : public ZCE_Message_Queue_Condi<_value_type,std::list>
Description     : �ڲ���LISTʵ�ֵ���Ϣ���У����ܵ�,�߽籣���õ�������������һ��ʼռ���ڴ治��
Other           : ��Ҫ����Ϊ�˸���һЩ�﷨��
Modify Record   :
************************************************************************************************************/
template <typename _value_type >
class ZCE_Msgqueue_List_Condi : public ZCE_Message_Queue_Condi<_value_type, std::list<_value_type> >
{
public:
    //
    explicit ZCE_Msgqueue_List_Condi(size_t queue_max_size):
        ZCE_Message_Queue_Condi<_value_type, std::list<_value_type> >(queue_max_size)
    {
    }

    ~ZCE_Msgqueue_List_Condi()
    {
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  :
  Param1: class _value_type ��Ϣ���б������������
Class           : ZCE_Msgqueue_Deque_Condi
Inherit         : public ZCE_Message_Queue_Condi<_value_type,std::deque<_value_type> >
Description     : �ڲ���DQUEUEʵ�ֵ���Ϣ���У����ܽϺ�,�߽籣���õ�����������
Other           : ��װ����Ҫ����Ϊ�˸���һЩ�﷨��
Modify Record   :
************************************************************************************************************/
template <class _value_type >
class ZCE_Msgqueue_Deque_Condi : public ZCE_Message_Queue_Condi<_value_type, std::deque<_value_type> >
{
public:
    //
    explicit ZCE_Msgqueue_Deque_Condi(size_t queue_max_size):
        ZCE_Message_Queue_Condi<_value_type, std::deque<_value_type> >(queue_max_size)
    {
    }

    ~ZCE_Msgqueue_Deque_Condi()
    {
    }
};

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��17��
Template Param  :
  Param1: class _value_type ��Ϣ���б������������
Class           : zce_condi_msgqueue_ring
Inherit         : public ZCE_Message_Queue_Condi<_value_type,boost::circular_buffer<_value_type> >
Description     : �ڲ���circular_bufferʵ�ֵ���Ϣ���У����ܷǳ���,�߽籣���õ�����������
Other           : ��װ����Ҫ��������Ϊ�˸����﷨�ǣ�������Ϊ�˼�������
Modify Record   :
************************************************************************************************************/
//template <class _value_type >
//class zce_condi_msgqueue_ring : public ZCE_Message_Queue_Condi<_value_type,boost::circular_buffer<_value_type> >
//{
//    //
//    zce_condi_msgqueue_ring(size_t queue_max_size):
//        queue_max_size_(queue_max_size),
//        queue_cur_size_(0)
//    {
//        message_queue_.resize(queue_max_size);
//    }
//
//    ~zce_condi_msgqueue_ring()
//    {
//    }
//};

#endif //#ifndef ZCE_LIB_THREAD_MESSAGE_QUEUE_CONDITION_H_

