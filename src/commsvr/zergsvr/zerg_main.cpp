/******************************************************************************************
Copyright           : 2000-2004, Tencent Technology (Shenzhen) Company Limited.
FileName            : zerg_main.cpp
Author              : Sail(ZENGXING)//Author name here
Version             :
Date Of Creation    : 2005��11��27��
Description         :

Others              :
Function List       :
    1.  ......
Modification History:
    1.Date  :
      Author  :
      Modification  :
******************************************************************************************/

#include "zerg_predefine.h"
#include "zerg_application.h"
#include "comm_service_info.h"
#include "comm_enum_define.h"

int main(int argc, const char *argv[])
{
    // ���÷�����
    Zerg_Service_App::instance()->set_service_info("Tencent Zerg Svr", "��Ѷ��Ϸ��ȫ����ͨѶ�������");
    return Zerg_Service_App::instance()->run(argc, argv);
}

