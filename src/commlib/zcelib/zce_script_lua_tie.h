//��������ǲο�Tinkerʵ�ֵģ���Ȼ��лԭ����

#ifndef ZCE_LIB_SCRIPT_LUA_H_
#define ZCE_LIB_SCRIPT_LUA_H_

#if defined  lua_h




//LUA Ь�������ڰ�����C++��
class ZCE_Lua_Tie
{
public:
    ZCE_Lua_Tie();
    ~ZCE_Lua_Tie();

    ///��lua state
    int open();
    ///�ر�lua state
    void close();


    ///ִ��һ��lua�ļ�
    int do_file(const char *filename);
    ///ִ��һ��LUA��buffer
    int do_buffer(const char *buff, size_t sz);

    ///dump C����lua�Ķ�ջ��
    void dump_clua_stack();
    ///dump lua���еĵĶ�ջ�����ڼ��lua����ʱ�����⣬�������
    void dump_luacall_stack();
    

    ///ע��int64_t�����ͣ���ΪLUA�ڲ���numberĬ����double��������ʵ�޷���ʾ������Ҫע�����
    void reg_int64();
    ///ע��uint64_t������
    void reg_uint64();
    ///ע��std::string������
    void reg_stdstring();

    //ע��ö��ֵ
    void reg_enum(const char *name, size_t item_num, ...);

    void reg_gobal_val(const char *name, );


public:

    //���������long�ı�������Ϊlong�޷���ֲ��

    ///PUSHһ�����ݵ���ջ��
    template<typename T> static void push_stack(lua_State * /*state*/, T /*val*/)
    {
    }
    template<> static void push_stack(lua_State *state, char val);
    template<> static void push_stack(lua_State *state, unsigned char val);
    template<> static void push_stack(lua_State *state, short val);
    template<> static void push_stack(lua_State *state, unsigned short val);
    template<> static void push_stack(lua_State *state, int val);
    template<> static void push_stack(lua_State *state, unsigned int val);
    template<> static void push_stack(lua_State *state, float val);
    template<> static void push_stack(lua_State *state, double val);
    template<> static void push_stack(lua_State *state, char *val);
    template<> static void push_stack(lua_State *state, const char *val);
    template<> static void push_stack(lua_State *state, bool val);
    template<> static void push_stack(lua_State *state, int64_t val);
    template<> static void push_stack(lua_State *state, uint64_t val);
    template<> static void push_stack(lua_State *state, std::string val);

    template<typename T> static  T read_stack(lua_State *L, int index)
    {
        return *(T*)(L, index);
    }
    template<> static char               *read_stack(lua_State *state, int index);
    template<> static const char         *read_stack(lua_State *state, int index);
    template<> static char                read_stack(lua_State *state, int index);
    template<> static unsigned char       read_stack(lua_State *state, int index);
    template<> static short               read_stack(lua_State *state, int index);
    template<> static unsigned short      read_stack(lua_State *state, int index);
    template<> static int                 read_stack(lua_State *state, int index);
    template<> static unsigned int        read_stack(lua_State *state, int index);
    template<> static float               read_stack(lua_State *state, int index);
    template<> static double              read_stack(lua_State *state, int index);
    template<> static bool                read_stack(lua_State *state, int index);
    template<> static int64_t             read_stack(lua_State *state, int index);
    template<> static uint64_t            read_stack(lua_State *state, int index);
    template<> static std::string         read_stack(lua_State *state, int index);

protected:

    //
    lua_State   *lua_state_;
};



#endif

#endif // ZCE_LIB_SCRIPT_LUA_H_

