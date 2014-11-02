#include "stdafx.h"
#include "illusion_excel_file.h"
#include "illusion_excel_config.h"


//����ĵ���ʵ��
Illusion_Excel_Config *Illusion_Excel_Config::instance_ = NULL;

//
Illusion_Excel_Config::Illusion_Excel_Config()
{
}


Illusion_Excel_Config::~Illusion_Excel_Config()
{
}


//�������еĶ�ȡ����
void Illusion_Excel_Config::ClearAllReadConf()
{
    map_enum_conf_.clear();
}



//��ȡ���е�ö��ֵ
int Illusion_Excel_Config::ReadEnumTable()
{
    ////������Ϣ
    //CMainFrame::Instance()->GetOutputView()->FillOutputInfo(
    //    CIllusionOutView::OUT_TYPE_DEBUG,
    //    _T("Start process file[%s] ENUM_DESC sheet.\n"),
    //    illusion_excel_file_.GetOpenFileName());

    //ǰ�������
    illusion_excel_file_.load_sheet("ENUM_DESC",TRUE);

    //��Ӧ����
    long row_count = illusion_excel_file_.GetRowCount();
    long col_count = illusion_excel_file_.GetColumnCount();
    TRACE("col_count = %u row_count =%u\n",col_count,row_count);

    //��ӡ
    //CMainFrame::Instance()->GetOutputView()->FillOutputInfo(
    //    CIllusionOutView::OUT_TYPE_INFO,
    //    _T("ENUM_DESC sheet have row_count =%u col_count = %u\n"),
    //    row_count,
    //    col_count);

    //ע�����е��±궼�Ǵ�1��ʼ��
    const long COL_ENUM_KEY = 1;
    const long COL_ENUM_VALUE = 2;
    const char ENUM_FIRST_CHAR = '[';

    size_t read_enum = 0;
    //��ȡ���е���
    for (long i=1;i<=row_count;++i)
    {

        long   row_no = i;
        //�ַ���
        CString enum_key = illusion_excel_file_.GetCellString(row_no,COL_ENUM_KEY);

        //�����һ���ַ���[
        if (enum_key[0] == ENUM_FIRST_CHAR )
        {
            int enum_vlaue = illusion_excel_file_.GetCellInt(row_no,COL_ENUM_VALUE);
            map_enum_conf_[enum_key] = enum_vlaue;

            ++read_enum;
        }
    }

    ////��ӡ
    //CMainFrame::Instance()->GetOutputView()->FillOutputInfo(
    //    CIllusionOutView::OUT_TYPE_INFO,
    //    _T("ENUM_DESC sheet have enum count = %u\n"),
    //    read_enum);
    //
    return 0;
}












