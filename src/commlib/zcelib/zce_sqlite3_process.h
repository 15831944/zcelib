/*!
* @copyright  2004-2013  Apache License, Version 2.0 FULLSAIL
* @filename   zce_sqlite3_process.h
* @author     Viviliu��Sailzeng <sailerzeng@gmail.com>
* @version
* @date       2008��3��12��
* @brief      ����SQLite��һЩ��������
* 
* @details    Ϊɶ����һ��3����ΪSQlite��ʱ����2�İ汾
*             ��3�İ汾�����ڼ䣬����SQlite���̵Ĳ�������2���ú޾�Ҫ�ݣ�ACE�����
*             ��������ģ�����
*             SQLite�Ĵ����࣬08���𣿸о�Ӧ�ø���һ��ѣ�Ӧ����07�ѣ�
*             ��ʱ��Լ����ҪѰ��һ�����ݿ���������ǵ�ʱ�����ݱ��������ACCESS�ģ�
*             ����ACCESSȴ���ܰ���΢���ħ�䣬
*             ����living����������RTX����SQLite���������Ϣ��������������������
*             ���ֵ�ȷ�ܺã���������������ݿ�һ��������ʰ�������ֻ��ϳ�Ϊ���ƶ�
*             �ն����ݿ������Ҳ����Ե������һ����Դ�ö����ĳɳ���
* 
*             6���Ժ���������SQLite����վ��������վ�����˲��٣�������һ����ֱ��
*             Դ�룬�������ô�档
* 
*             ����Sqlite���ٶȣ���ο�http://www.sqlite.org/speed.html��
* 
*/
#ifndef ZCE_LIB_SQLITE3_PRCOESS_H_
#define ZCE_LIB_SQLITE3_PRCOESS_H_

//Ŀǰ�汾����ֻ����һ��
#if SQLITE_VERSION_NUMBER >= 3003000

/*!
@brief      ���Ӵ���һ��SQLite3���ݿ�ģ���һ��SQLite3���ݿ�͵õ�Handler
            ��Handler��ɺ���������ݿ������
*/
class ZCELIB_EXPORT SQLite3_DB_Handler
{

public:

    ///���캯����
    SQLite3_DB_Handler();
    ///��������
    ~SQLite3_DB_Handler();

    /*!
    @brief      �����ݿ⣬ע���ļ����Ƶ�·��Ҫ��UTF8���룬������ò�Ҫ������?
    @return     int      �����ݿ��Ƿ�ɹ� 0 �ɹ�������ʧ��
    @param      db_file  ���ݿ��ļ������ƣ�
                         �����������Ĺ�ϵ�����������WINDOWS�㴫�����ANSI��
                         �ַ�������LINUX���㴫�����UTF8
    */
    int open_database(const char *db_file);

    ///�������Ҫ3.5�Ժ�İ汾�ſ�����
#if SQLITE_VERSION_NUMBER >= 3005000

    /*!
    @brief      ֻ����ʽ�����ݿ�,�������Ҫ3.5�Ժ�İ汾�ſ����á�
    @return     int      �����ݿ��Ƿ�ɹ� 0 �ɹ�������ʧ��
    @param      db_file  ���ݿ��ļ�������,ANSI �ַ���
    */
    int open_readonly_db(const char *db_file);
#endif

    ///�ر����ݿ�
    void close_database();

    ///ȡ�ô������Str
    const char *get_dbret_errstr();
    ///ȡ��DB���صĴ���ID
    unsigned int get_dbret_errid();

    ///ȡ��SQLite�ľ��
    inline sqlite3 *get_sqlite_handler()
    {
        return sqlite3_handler_;
    };

    ///��ʼһ������
    int begin_transaction();
    ///�ύһ�����񣬻���˵����һ������
    int commit_transction();

    ///��ͬ��ѡ��رգ����鲻Ҫʹ��
    int turn_off_synch();

    //ִ��INSERT��REPLACE�Ĳ�����䣬֧�ֶ�����¼һ���Բ���,
    //int execute_insert_sql(const char *sql_string);

protected:

    ///sqlite3�Ĵ���Handler
    sqlite3         *sqlite3_handler_;

};

#endif //SQLITE_VERSION_NUMBER >= 3003000

#endif //ZCE_LIB_SQLITE3_PRCOESS_H_

