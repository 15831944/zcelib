

#include "ogre_predefine.h"
#include "ogre_application.h"

int main(int argc, const char *argv[])
{
    CfgSvrSdk::instance()->add_config("zergsvrd.xml");
    CfgSvrSdk::instance()->add_config("framework.xml");
    // ���÷�����
    Ogre4a_Service_App::instance()->set_service_info("Tencent Zerg Svr", "��Ѷ��Ϸ��ȫ����ͨѶ�������");
    return Ogre4a_Service_App::instance()->run(argc, argv);
}

