#ifndef ZCE_LIB_BUS_ONE_WAY_H_
#define ZCE_LIB_BUS_ONE_WAY_H_

#include "zce_bus_mmap_pipe.h"

class dequechunk_node;
class shm_dequechunk;

//˫�е�
class ZCELIB_EXPORT ZCE_BusPipe_OneWay : public ZCE_Bus_MMAPPipe
{
public:

protected:
    //instance����ʹ�õĶ���
    static ZCE_BusPipe_OneWay *one_way_instance_;

public:

    //���캯��,
    ZCE_BusPipe_OneWay();
    //��������
    ~ZCE_BusPipe_OneWay();

public:

public:

    //ʵ���ĸ�ֵ
    static void instance(ZCE_BusPipe_OneWay *);
    //ʵ���Ļ��
    static ZCE_BusPipe_OneWay *instance();
    //���ʵ��
    static void clean_instance();

};

#endif //ZCE_LIB_BUS_ONE_WAY_H_
