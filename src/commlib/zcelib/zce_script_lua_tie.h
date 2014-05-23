//��������ǲο�Tinkerʵ�ֵģ���Ȼ��лԭ����

#ifndef ZCE_LIB_SCRIPT_LUA_H_
#define ZCE_LIB_SCRIPT_LUA_H_

#if defined  lua_h




//LUA Ь�������ڰ�����C++��
class ZCE_Lua_Tie
{
    //�ܶ������ģ�溯��
public:

    ///��C11������Ч�����ʵ��
    template<typename ret_type, typename... args>
    struct functor
    {
        static int invoke(lua_State *state)
        {
            //push�ǽ���������ջ
            void *upvalue_1 = lua_touserdata(state,lua_upvalueindex(1));
            ret_type(*)((args)...) fun_ptr = (ret_type(*)(args)...) (upvalue_1);

            if (std::is_void<ret_type>::value)
            {
                int para_idx = 0;
                fun_ptr(read_stack(state, para_idx++)...);
                return 0;
            }
            else
            {
                int para_idx = 0;
                push_stack<ret_type>(fun_ptr(read_stack(state, para_idx++)...));
                return 1;
            }

        }
    };

    //���������long�ı�������Ϊlong�޷���ֲ��

    ///PUSHһ�����ݵ���ջ��
    template<typename val_type> static void push_stack(lua_State * /*state*/, val_type /*val*/)
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

    ///
    template<typename val_type> static  val_type read_stack(lua_State *L, int index)
    {
        return *(val_type*)(L, index);
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

    ///
    template<typename val_type>
    val_type pop_stack(lua_State *state)
    {
        val_type t = read_stack<val_type>(state, -1);
        lua_pop(L, 1);
        return t;
    }


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


    ///��LUAע��int64_t�����ͣ���ΪLUA�ڲ���numberĬ����double��������ʵ�޷���ʾ������Ҫע�����
    void reg_int64();
    ///��LUAע��uint64_t������
    void reg_uint64();
    ///��LUAע��std::string������
    void reg_stdstring();

    ///��LUAע��ö��ֵ
    void reg_enum(const char *name, size_t item_num, ...);

    ///��LUA����һ��ȫ�ֱ����������ƶ�Ӧֵ�ÿ�����
    template<typename val_type>
    void set_tolua(const char *name, typename val_type val)
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        //ģ�庯��������val_type�������push
        push_stack(lua_state_, val);
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }
    ///
    template<typename val_type>
    typename val_type get_fromlua(const char *name)
    {
        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);
        return pop_stack<val_type>(lua_state_);
    }

    ///ע��һ��ȫ�ֺ�����������ľ�̬������lua����
    template<typename ret_type, typename... args>
    void reg_fun(const char *name, ret_type(*func)(args...))
    {
        //��������
        lua_pushstring(lua_state_, name);
        //������ָ��ת��Ϊvoid * ����Ϊlightuserdata �����ջ����Ϊclosure��upvalue����
        lua_pushlightuserdata(lua_state_, (void *)func);
        //functorģ�庯��������closure,
        lua_pushcclosure(lua_state_, functor<ret_type, args...>::invoke, 1);
        //�������ȫ�ֻ�������
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

protected:

    //
    lua_State   *lua_state_;
};



#endif

#endif // ZCE_LIB_SCRIPT_LUA_H_

