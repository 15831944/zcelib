
#ifndef SOARING_LIB_SVRD_APP_MAIN_H_
#define SOARING_LIB_SVRD_APP_MAIN_H_

//�����ú�ʵ���ˣ�����д���ˣ���΢��ļ��У��ͷ��־���ʹ�ࡣ������ģ��ѡ�

namespace TSS_LIB
{

template <class application_class, class  config_class>
int tss_svrd_main(int argc, char *argv[])
{
    //�������쳣����Ϊ�����˲��õ���,�ر�����Win32�µ��ԡ�

    application_class::instance();
    config_class::instance();

    const size_t ERROR_INFO_LEN = 512;
    char err_str_info[ERROR_INFO_LEN];

    //��ʼ��־���޷������
    //ZLOG_INFO("[framework] App init_instance start");

    //��ʼ��
    int ret = application_class::instance()->init_instance(argc, argv, config_class::instance());

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        SOAR_RET::ZLoadString(ret, err_str_info);
        ZLOG_ERROR("[framework] App name [%s] class [%s] init_instance fail:%d|%s",
                   application_class::instance()->get_app_runname(),
                   typeid(*application_class::instance()).name(),
                   ret,
                   err_str_info);
        return ret;
    }

    ZLOG_INFO("[framework] App name [%s] class [%s] init_instance sucess.",
              application_class::instance()->get_app_runname(),
              typeid(*application_class::instance()).name());

    ZLOG_INFO("[framework] App name [%s] class [%s] run_instance start.",
              application_class::instance()->get_app_runname(),
              typeid(*application_class::instance()).name());
    //����
    ret = application_class::instance()->run_instance();

    //��ʾ����ʧ��
    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        SOAR_RET::ZLoadString(ret, err_str_info);
        ZLOG_ERROR("[framework] App name [%s] class [%s] run_instance fail:%d|%s .",
                   application_class::instance()->get_app_runname(),
                   typeid(*application_class::instance()).name(),
                   ret,
                   err_str_info);
        //������˳�,���ǽ���exit_instance,����һ�����⣬����
        //return ret;
    }
    else
    {
        ZLOG_ERROR("[framework] App name [%s] class [%s] run_instance sucess.",
                   application_class::instance()->get_app_runname(),
                   typeid(*application_class::instance()).name());
    }

    ZLOG_INFO("[framework] App name [%s] class [%s] exit_instance start.",
              application_class::instance()->get_app_runname(),
              typeid(*application_class::instance()).name());
    //�˳�����
    application_class::instance()->exit_instance();

    ZLOG_INFO("[framework] App name [%s] class [%s] exit_instance start.",
              application_class::instance()->get_app_runname(),
              typeid(*application_class::instance()).name());

    //����APP �����õ�instance
    config_class::clean_instance();
    application_class::clean_instance();

    //�ر���־
    ZCE_Trace_LogMsg::clean_instance();

    return SOAR_RET::SOAR_RET_SUCC;

}

};

#endif //#ifndef SOARING_LIB_SVRD_APP_NONCTRL_H_

