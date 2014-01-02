/*!
* @copyright  2004-2014  Apache License, Version 2.0 FULLSAIL
* @filename   soar_frame_malloc.cpp
* @author     Sailzeng <sailerzeng@gmail.com>,Scottxu,
* @version
* @date       2008��2��20��
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

#ifndef SOARING_LIB_APPFRAME_MALLOCOR_H_
#define SOARING_LIB_APPFRAME_MALLOCOR_H_

#include "soar_zerg_frame.h"

template <typename ZCE_LOCK >
class AppFrame_Mallocor_Mgr
{
protected:
    //Ͱ���еĸ���
    static const size_t  NUM_OF_FRAMELIST = 10;

    //ÿ��Ͱ��ʼ����FRAME�ĸ���
    static const size_t NUM_OF_ONCE_INIT_FRAME = 512;

    //������е�NUM_OF_ALLOW_LIST_IDLE_FRAME
    static const size_t NUM_OF_ALLOW_LIST_IDLE_FRAME = 1024;

    //
    typedef ZCE_LIB::lordrings <Zerg_App_Frame *>     LIST_OF_APPFRAME;
    //
    typedef std::vector< LIST_OF_APPFRAME > APPFRAME_MEMORY_POOL;

protected:

    //GCC�İ汾���С��4���᲻֧��ģ���е�static�����Ա�ĳ�����const static��Ա���塣
    //�����Ͽ�����#if (__GNUC__ < 4)���Σ�����ʵ��̫æ������������鳤��NUM_OF_FRAMELIST
    //����������Ҫ���塣
    size_t                  size_appframe_[NUM_OF_FRAMELIST];

    //FRAME���ڴ����
    APPFRAME_MEMORY_POOL    frame_pool_;
    //���ӵ���
    ZCE_LOCK                zce_lock_;

protected:

    //����ʵ��
    static AppFrame_Mallocor_Mgr    *instance_;

public:

    //���캯��
    AppFrame_Mallocor_Mgr();
    //��������
    ~AppFrame_Mallocor_Mgr();

public:
    //��ʼ��
    void initialize(size_t init_num = NUM_OF_ONCE_INIT_FRAME,
                    size_t max_frame_len = Zerg_App_Frame::MAX_LEN_OF_APPFRAME);

    //�ӳ��ӷ���һ��APPFRAME
    Zerg_App_Frame *alloc_appframe(size_t frame_len);
    //�ͷ�һ��APPFRAME������
    void free_appframe(Zerg_App_Frame *proc_frame);
    //����һ��APPFRAME
    void clone_appframe(const Zerg_App_Frame *model_freame, Zerg_App_Frame *&cloned_frame);

    //�����Է����FRAME�ĳ���
    size_t GetMaxFrameLength();

    //�������ӵ�����
    void AdjustPoolListCapacity();

public:

    //�õ�SINGLETON��ʵ��
    static AppFrame_Mallocor_Mgr *instance();
    //����SINGLETON��ʵ��
    static void clean_instance();

protected:

    //��չLIST������
    void ExtendFrameListCapacity(size_t list_no, size_t extend_num);
    //
    inline size_t GetRoundUp(size_t sz_frame);

};

/******************************************************************************************
Author          : Sail(ZENGXING)  Date Of Creation: 2008��9��18��
Function        : AppFrame_Mallocor_Mgr<ZCE_LOCK>::initialize
Return          : void
Parameter List  :
  Param1: size_t init_num      ÿ��POOL�ڲ���FRAME����
  Param2: size_t max_frame_len ����FRAME�ĳ��ȣ����������������
Description     : ��ʼ��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template <typename ZCE_LOCK >
void AppFrame_Mallocor_Mgr<ZCE_LOCK>::initialize(size_t init_num,
                                                 size_t max_frame_len)
{
    ZCE_ASSERT(max_frame_len > 2048 && init_num > 8);

    ZLOG_INFO("[framework] AppFrame_Mallocor_Mgr::AppFrame_Mallocor_Mgr init num=%u,max_frame_len=%u.",
              init_num,
              max_frame_len);

    //�õ�����
    size_t sz_frame = max_frame_len;

    for (size_t i = 0; i < NUM_OF_FRAMELIST; ++i)
    {
        size_appframe_[NUM_OF_FRAMELIST - i - 1] = sz_frame;
        sz_frame = sz_frame / 2;
    }

    frame_pool_.resize(NUM_OF_FRAMELIST);

    for (size_t i = 0; i < NUM_OF_FRAMELIST; ++i)
    {
        ExtendFrameListCapacity(i, init_num);
    }

}

//�����Է����FRAME�ĳ���
template <typename ZCE_LOCK >
size_t AppFrame_Mallocor_Mgr<ZCE_LOCK>::GetMaxFrameLength()
{
    return size_appframe_[NUM_OF_FRAMELIST - 1];
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��17��
Function        : AppFrame_Mallocor_Mgr<ZCE_LOCK>::GetRoundUp
Return          : inline size_t
Parameter List  :
  Param1: size_t sz_frame
Description     : ����Ҫ��ĵ�FRAME�ߴ��С������һ��FRAME
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template <typename ZCE_LOCK >
inline size_t AppFrame_Mallocor_Mgr<ZCE_LOCK>::GetRoundUp(size_t sz_frame)
{
    //Ҳ��ѭ���һ���
    //Ҳ��ѭ���һ���
    for (size_t i = 0; i < NUM_OF_FRAMELIST; ++i)
    {
        if (sz_frame <= size_appframe_[i])
        {
            return i;
        }
    }

    //���˺�
    ZCE_ASSERT(false);
    return static_cast<size_t>(-1);
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��17��
Function        : AppFrame_Mallocor_Mgr<ZCE_LOCK>::AppFrame_Mallocor_Mgr
Return          : template <typename ZCE_LOCK >
Parameter List  :
  Param1: size_t init_num
Description     : ���캯��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template <typename ZCE_LOCK >
AppFrame_Mallocor_Mgr<ZCE_LOCK>::AppFrame_Mallocor_Mgr()
{
    memset(size_appframe_, 0, sizeof(size_appframe_));
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��17��
Function        : AppFrame_Mallocor_Mgr<ZCE_LOCK>::~AppFrame_Mallocor_Mgr
Return          : NULL
Parameter List  : NULL
Description     : ��������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template <typename ZCE_LOCK >
AppFrame_Mallocor_Mgr<ZCE_LOCK>::~AppFrame_Mallocor_Mgr()
{

    //
    ZLOG_INFO("[framework] AppFrame_Mallocor_Mgr::~AppFrame_Mallocor_Mgr.");

    //���Ӧ��size == capacity , freesize==0
    for (size_t i = 0; i < NUM_OF_FRAMELIST; ++i)
    {
        //����ڴ�ȫ���黹
        if (frame_pool_[i].freesize() == 0)
        {
            //
            ZLOG_INFO("[framework] List %u(frame size:%u):,free node:%u,capacity node:%u,list node:%u.Ok.",
                      i,
                      size_appframe_[i],
                      frame_pool_[i].freesize(),
                      frame_pool_[i].capacity(),
                      frame_pool_[i].size());
        }
        //��������ڴ�
        else
        {
            //
            ZLOG_ERROR("[framework] List %u(frame size:%u):,free node:%u,capacity node:%u,list node:%u.Have memory leak.Please check your code.",
                       i,
                       size_appframe_[i],
                       frame_pool_[i].freesize(),
                       frame_pool_[i].capacity(),
                       frame_pool_[i].size());
        }

        //�ͷŵ�����Ŀռ�
        size_t frame_pool_len = frame_pool_[i].size();

        for (size_t j = 0; j < frame_pool_len; ++j)
        {
            Zerg_App_Frame *proc_frame = NULL;
            frame_pool_[i].pop_front(proc_frame);
            delete proc_frame;
        }
    }
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��17��
Function        : AppFrame_Mallocor_Mgr<ZCE_LOCK>::alloc_appframe
Return          : template <typename ZCE_LOCK > Zerg_App_Frame*
Parameter List  :
  Param1: size_t frame_len FRAME�ĳ���
Description     : �ӳ��ӷ���һ��APPFRAME
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template <typename ZCE_LOCK >
Zerg_App_Frame *AppFrame_Mallocor_Mgr<ZCE_LOCK>::alloc_appframe(size_t frame_len)
{
    typename ZCE_LOCK::LOCK_GUARD tmp_guard(zce_lock_);
    size_t hk = GetRoundUp(frame_len);

    //
    if (frame_pool_[hk].size() <= 0 )
    {
        ExtendFrameListCapacity(hk, NUM_OF_ONCE_INIT_FRAME);
    }

    //
    Zerg_App_Frame *new_frame = NULL;
    frame_pool_[hk].pop_front(new_frame);

    new_frame->init_framehead(static_cast<unsigned int>(frame_len));

    return new_frame;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��17��
Function        : AppFrame_Mallocor_Mgr<ZCE_LOCK>::clone_appframe
Return          : template <typename ZCE_LOCK > void
Parameter List  :
  Param1: const Zerg_App_Frame* model_freame ģ��FRAME
  Param2: Zerg_App_Frame*& cloned_frame      ����¡��FRAME
Description     : ��¡һ��APPFAME
Calls           :
Called By       :
Other           : �������û�м�������Ϊ�о�����Ҫ��alloc_appframe�������������������ظ�����
Modify Record   :
******************************************************************************************/
template <typename ZCE_LOCK >
void AppFrame_Mallocor_Mgr<ZCE_LOCK>::clone_appframe(const Zerg_App_Frame *model_freame, Zerg_App_Frame *&cloned_frame)
{
    //
    size_t frame_len = model_freame->frame_length_;
    cloned_frame = alloc_appframe(frame_len);
    memcpy(cloned_frame, model_freame, frame_len);
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��17��
Function        : AppFrame_Mallocor_Mgr<ZCE_LOCK>::free_appframe
Return          : template <typename ZCE_LOCK > void
Parameter List  :
  Param1: Zerg_App_Frame* proc_frame �ͷŵ��ڴ�
Description     : �ͷ�һ��APPFRAME������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template <typename ZCE_LOCK >
void AppFrame_Mallocor_Mgr<ZCE_LOCK>::free_appframe(Zerg_App_Frame *proc_frame)
{
    ZCE_ASSERT(proc_frame);
    typename ZCE_LOCK::LOCK_GUARD tmp_guard(zce_lock_);
    size_t hk = GetRoundUp(proc_frame->frame_length_);
    frame_pool_[hk].push_back(proc_frame);
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��17��
Function        : AppFrame_Mallocor_Mgr<ZCE_LOCK>::AdjustPoolListCapacity
Return          : void
Parameter List  : NULL
Description     : �������ӵ�����
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template <typename ZCE_LOCK >
void AppFrame_Mallocor_Mgr<ZCE_LOCK>::AdjustPoolListCapacity()
{
    typename ZCE_LOCK::LOCK_GUARD tmp_guard(zce_lock_);

    for (size_t i = 0; i < NUM_OF_FRAMELIST; ++i)
    {
        //���ʣ�������
        if (frame_pool_[i].size() > 2 * NUM_OF_ALLOW_LIST_IDLE_FRAME)
        {
            size_t free_sz = frame_pool_[i].size() - 2 * NUM_OF_ALLOW_LIST_IDLE_FRAME;

            for (size_t j = 0; j < free_sz; ++j)
            {
                Zerg_App_Frame *new_frame = NULL;
                frame_pool_[i].pop_front(new_frame);
                delete new_frame;
            }
        }
    }
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��3��17��
Function        : AppFrame_Mallocor_Mgr<ZCE_LOCK>::ExtendFrameListCapacity
Return          : void
Parameter List  :
Param1: size_t list_no
Param2: size_t extend_num
Description     : ��չLIST������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template <typename ZCE_LOCK >
void AppFrame_Mallocor_Mgr<ZCE_LOCK>::ExtendFrameListCapacity(size_t list_no, size_t extend_num)
{
    size_t old_capacity =  frame_pool_[list_no].capacity();
    frame_pool_[list_no].resize(old_capacity + extend_num);

    for (size_t j = 0; j < extend_num; ++j)
    {
        Zerg_App_Frame *proc_frame = new(size_appframe_[list_no] + 1) Zerg_App_Frame();
        frame_pool_[list_no].push_back(proc_frame);
    }
}

//�õ�SINGLETON��ʵ��
template <typename ZCE_LOCK >
AppFrame_Mallocor_Mgr<ZCE_LOCK> *AppFrame_Mallocor_Mgr<ZCE_LOCK>::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new AppFrame_Mallocor_Mgr();
    }

    return instance_;
}

//����SINGLETON��ʵ��
template <typename ZCE_LOCK >
void AppFrame_Mallocor_Mgr<ZCE_LOCK>::clean_instance()
{
    if (instance_)
    {
        delete instance_;
        instance_ = NULL;
    }
}

//
typedef AppFrame_Mallocor_Mgr<ZCE_Null_Mutex> NULLMUTEX_APPFRAME_MALLOCOR ;
typedef AppFrame_Mallocor_Mgr<ZCE_Thread_Light_Mutex> THREADMUTEX_APPFRAME_MALLOCOR ;

#endif //#ifndef SOARING_LIB_APPFRAME_MALLOCOR_H_

