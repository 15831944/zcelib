// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// ��Ŀ�ض��İ����ļ�

#pragma once


class Illusion_ExcelFile
{

public:

    //
    Illusion_ExcelFile();
    virtual ~Illusion_ExcelFile();

protected:
    //�򿪵�EXCEL�ļ�����
    CString       open_excel_file_;

    //
    CWorkbooks    excel_books_; 
    //
    CWorkbook     excel_work_book_; 
    //
    CWorksheets   excel_sheets_; 
    //
    CWorksheet    excel_work_sheet_; 
    //
    CRange        excel_current_range_; 

    ///�Ƿ����Ԥ�ȼ��أ�Ԥ���ظ��������ڴ棬����ȡЧ����߼�ʮ��
    BOOL          already_preload_;

    //Create the SAFEARRAY from the VARIANT ret.
    COleSafeArray ole_safe_array_;
    

protected:
    //
    static CApplication excel_application_;
public:
    //
    void show_in_excel(BOOL bShow);

    //���һ��CELL�Ƿ����ַ���
    BOOL    is_cell_string(long iRow, long iColumn);
    //���һ��CELL�Ƿ�����ֵ
    BOOL    is_cell_number(long iRow, long iColumn);

    //�õ�һ��CELL��CString
    CString get_cell_cstring(long iRow, long iColumn);
    //�õ�����
    int     get_cell_int(long iRow, long iColumn);
    //�õ�double������
    double  get_celldouble(long iRow, long iColumn);

    //ȡ���е�����
    int row_count();
    //ȡ���е�����
    int column_count();

    //ʹ��ĳ��shet��shit��shit
    BOOL load_sheet(long table_index,BOOL pre_load = FALSE);
    //ͨ������ʹ��ĳ��sheet��
    BOOL load_sheet(const CString& sheet,BOOL pre_load = FALSE);
    //ͨ�����ȡ��ĳ��Sheet������
    CString sheet_name(long table_index);

    //�õ�Sheet������
    int sheets_count();

    //���ļ�
    BOOL open_excelfile(const CString &file_name);
    //�رմ򿪵�Excel �ļ�����ʱ���EXCEL�ļ���Ҫ
    void close_excelfile(BOOL if_save = FALSE);
    //���Ϊһ��EXCEL�ļ�
    void saveas_excelfile(const CString &xls_file);
    //ȡ�ô��ļ�������
    CString open_filename();
    
    //
    void set_cell_int(long irow, long icolumn,int new_int);
    //
    void set_cell_string(long irow, long icolumn,const CString &new_string);
    
public:
    //��ʼ��EXCEL OLE
    static BOOL init_excel();
    //�ͷ�EXCEL�� OLE
    static void release_excel();
    //ȡ���е����ƣ�����27->AA
    static TCHAR *column_name(long iColumn);
    
protected:

    //Ԥ�ȼ���
    void preload_sheet();
};
