/// @file       arbiter_type_define.h
/// @date       2012/07/19

/// @author     stefzhou

/// @brief      ���ݺ����Ͷ���
/// @details

#ifndef ARBITER_STAT_DEFINE_H_
#define ARBITER_STAT_DEFINE_H_

/******************************************************************************************
�����Ϣ
******************************************************************************************/
enum ArbiterMonitorFeatureID
{
    ARBITER_SERVICE_STAT_BEGIN = SVC_PROXY_SERVER * 10000, // proxysvr start
    ARBITER_TRANS_PKG_ERROR,                  // ת�������ݰ�����
    ARBITER_TRANS_SEND_UNBALANCE_RATE,        // ���Ͷ˸��ز��������
    ARBITER_SERVICE_STAT_END,
};

// �ϱ����������
static const unsigned int ARBITER_FRATURE_NUM = 
    ARBITER_SERVICE_STAT_END - ARBITER_SERVICE_STAT_BEGIN - 1;

// ͳ����Ϣ�ַ���
const ZEN_STATUS_ITEM_WITHNAME STR_ARBITER_STAT[ARBITER_FRATURE_NUM] =
{
    DEF_ZEN_STATUS_ITEM(ARBITER_TRANS_PKG_ERROR, STATICS_PER_FIVE_MINTUES),
    DEF_ZEN_STATUS_ITEM(ARBITER_TRANS_SEND_UNBALANCE_RATE, STATICS_PER_FIVE_MINTUES),
};

#endif // ARBITER_STAT_DEFINE_H_
