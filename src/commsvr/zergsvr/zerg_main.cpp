

#include "zerg_predefine.h"
#include "zerg_application.h"


int main(int argc, const char *argv[])
{
    // ���÷�����
    Zerg_Service_App::instance()->set_service_info("Zerg Svr", "");
    return Zerg_Service_App::instance()->run(argc, argv);
}

