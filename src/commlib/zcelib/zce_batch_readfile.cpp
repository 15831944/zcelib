
#include "zce_predefine.h"
#include "zce_batch_readfile.h"

//PetBatchReadFile::PetBatchReadFile(const char *file_name, BATCH_READ_FILE_TYPE batch_filetype):
//    batch_filename_(file_name),
//    batch_filetype_(batch_filetype)
//{
//    ZCE_ASSERT(batch_filetype == FILE_TYPE_QQUIN_LIST || batch_filetype ==  FILE_TYPE_PETLIST_LIST);
//}
//
//
//PetBatchReadFile::~PetBatchReadFile()
//{
//}
//
//int PetBatchReadFile::ProcessBatchFile()
//{
//    //
//    std::ifstream read_file(batch_filename_.c_str(),std::ios::in|std::ios::binary);
//    std::string str_readbuf;
//    std::istringstream  read_stream;
//
//
//    //�ȶ�ȡ�ļ���ȫ����ȡ
//    if (read_file)
//    {
//
//        ZLOG_INFO("[framework] Batch process file [%s] open success.file .",
//            batch_filename_.c_str());
//
//        read_file.seekg(0,std::ios::end);
//        std::streamsize size_of_file = read_file.tellg();
//        read_file.seekg(0,std::ios::beg);
//
//        //�Բ�ס��,�����������1G���ļ�����Ϊ���Ƕ�����ԣ��
//        ZCE_ASSERT (size_of_file < (0x40000000));
//
//        //�����е����ݶ��뻺��
//        char *outbuf = new char[static_cast<size_t>(size_of_file+16)];
//        //��ȡ���е�����,�ļ��Ķ�ȡ��ʽ�Ƕ����ƶ�ȡ��ʽ
//        read_file.read(outbuf,size_of_file);
//        outbuf[size_of_file] ='\0';
//
//        //�ر��ļ�
//        read_file.close();
//
//        str_readbuf.assign(outbuf,static_cast<size_t>(size_of_file));
//        //�������ͷŵ�
//        delete[] outbuf;
//
//        //+1����Ҫ,��֤�ַ����Ľ�β��'\0'
//        read_stream.str(str_readbuf);
//
//        ZLOG_INFO("[framework] Batch process file [%s] read success.file size [%u].",
//            batch_filename_.c_str(),
//            size_of_file);
//
//        //Ԥ����Ӧ�Ŀռ�
//        if (FILE_TYPE_QQUIN_LIST == batch_filetype_)
//        {
//            ary_qq_uin_.reserve(static_cast<size_t>(size_of_file)/7);
//        }
//        else if (FILE_TYPE_PETLIST_LIST == batch_filetype_)
//        {
//            ary_comm_id_.reserve(static_cast<size_t>(size_of_file)/12);
//        }
//    }
//    else
//    {
//        ZLOG_ERROR("[framework] Batch process file read fail[%s].",batch_filename_.c_str());
//        return SOAR_RET::ERR_READ_BATCH_FILE_FAIL;
//    }
//
//
//    const size_t BUFF_LEN = 1024;
//    char outbufline[BUFF_LEN+1];
//    outbufline[BUFF_LEN]= '\0';
//
//    //����ļ�״̬��ȷ,
//    while(read_stream)
//    {
//        //
//        read_stream.getline(outbufline,BUFF_LEN);
//        std::string str_line(outbufline,strlen(outbufline));
//
//        std::istringstream  line_stream;
//        line_stream.str(str_line);
//
//        //������������ļ����ͣ�����������
//        if (FILE_TYPE_QQUIN_LIST == batch_filetype_)
//        {
//            unsigned int qq_uin;
//            line_stream>>qq_uin;
//        }
//        else if (FILE_TYPE_PETLIST_LIST == batch_filetype_ )
//        {
//            PET_ID comm_id;
//            line_stream>>comm_id.qq_uin_;
//            line_stream.ignore();
//            line_stream>>comm_id.comm_type_;
//        }
//    }
//    return SOAR_RET::SOAR_RET_SUCC;
//}
//
////�õ�����OK��UIN�б�
//void PetBatchReadFile::GetQQUinAry(const ARRAY_OF_QQ_UIN *&ary_qq_uin)
//{
//    ary_qq_uin = &ary_qq_uin_;
//}
////�õ�����OK�ĳ���ID
//void PetBatchReadFile::GetPetIDAry(const ARRAY_OF_COMM_LIB_ID *&ary_comm_id)
//{
//    ary_comm_id = &ary_comm_id_;
//}
//
//
//

