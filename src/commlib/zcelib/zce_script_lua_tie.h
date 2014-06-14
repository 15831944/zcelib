//��������ǲο�Tinkerʵ�ֵģ���Ȼ��лԭ����
//The sun shone, having no alternative, on the nothing new. --Murphy ̫���ճ�����һ�ж�û�иı䡣���� ��ī�ơ�


#ifndef ZCE_LIB_SCRIPT_LUA_H_
#define ZCE_LIB_SCRIPT_LUA_H_




//LUAĿǰ�İ�װ����ʹ��C11������Ч���������µı�����
#if defined  ZCE_USE_LUA && defined ZCE_SUPPORT_CPP11

//LUA 5.02�汾Ŀǰ���ԣ��󲿷����������֧�֣����ԡ���
#if LUA_VERSION_NUM != 501
#error "[Error] please check your lua libary version,only support 5.1,Lua 5.2 is not mature. LUA_VERSION_NUM != 501."
#endif

#if defined (ZCE_OS_WINDOWS)
#pragma warning ( push )
#pragma warning ( disable : 4127)
#endif

//�����ʹ������ZCE_LUA�����ֿռ䣬��Ϊ��ZCE_LIB�о�������ȫ�����ͻ��
namespace ZCE_LUA
{
///ֻ����table��newdindex
int newindex_onlyread(lua_State *state);

///dump lua���еĵĶ�ջ�����ڼ��lua����ʱ�����⣬�������
int dump_luacall_stack(lua_State *state);

///dump C����lua�Ķ�ջ��
int dump_clua_stack(lua_State *state);

///
int on_error(lua_State *state);


//��ģ�庯��������æʵ��һ������������ͨ��class �ҵ���Ӧ�������ƣ�ע�ᵽLUA�����ƣ���
template<typename class_type>
struct class_name
{
    //�����¼һ��ȫ�ֵ�����
    static const char *name(const char *name = NULL)
    {
        const size_t MAX_LEN = 255;
        static char cname[MAX_LEN + 1] = "";
        if (name)
        {
            strncpy(cname, name, MAX_LEN);
            cname[MAX_LEN] = '\0';
        }
        return cname;
    }
};

//USER DATA�Ļ��࣬����һ��void *��ָ�룬���ڼ�¼���ݵ�ָ�룬ͨ��
//�̳�ʵ�ֶ���val��ptr��ref�Ķ������
struct lua_udat_base
{
    lua_udat_base(void *p)
        : obj_ptr_(p)
    {
    }
    //������virtual
    virtual ~lua_udat_base()
    {
    }
    void *obj_ptr_;
};

///��һ��val����LUA USER DATA�ķ�װ
template<typename val_type>
struct val_2_udat : lua_udat_base
{
    val_2_udat() 
        : lua_udat_base(new val_type) 
    {
    } 

    //args_type�ǹ��캯���Ĳ���,args_type1 ���ڵ�Ŀ���Ǻ�Ĭ�Ϲ��캯���ֿ���
    template<typename args_type1, typename ...args_tlist>
    val_2_udat(args_type1 arg1, args_tlist ...arg) : lua_udat_base(new val_type(arg1,args_tlist ...))
    {
    }

    template<typename args_type>
    val_2_udat(args_type arg) : lua_udat_base(new val_type(arg))
    {
    }


    //�����������,��,�ͷŷ���Ŀռ�,ԭ����������virtual
    virtual ~val_2_udat()
    {
        delete ((val_type *)obj_ptr_);
    }
};

///
template<typename val_type>
struct ptr_2_udat : lua_udat_base
{
    ptr_2_udat(val_type *t)
        : lua_udat_base((void *)t)
    {
    }
};

///
template<typename val_type>
struct ref_2_udat : lua_udat_base
{
    //ע���һ��&t��ʾ�����ò������ڶ����Ǳ�ʾ����ָ���lua_udat_base����
    ref_2_udat(val_type &t) :
        lua_udat_base(&t)
    {
    }
};


template<typename val_type  >
void push_stack(lua_State * /*state*/, typename val_type /*val*/)
{
    ZCE_LOGMSG(RS_ERROR, "[LUATIE]Type[%s] not support in this code?", typeid(val_type).name());
    return;
}

///PUSHһ�����ݵ���ջ��
template<typename val_type  >
void push_stack(lua_State *state,
    typename std::enable_if<std::is_object<val_type>::value>::type val)
{
    new(lua_newuserdata(state,
        sizeof(val_2_udat<val_type>))) val_2_udat<val_type>(val);
    
    //����������ƣ�����metatable
    lua_pushstring(state, class_name<val_type >::name());
    lua_gettable(state, LUA_GLOBALSINDEX);
    if (lua_istable(state, -1))
    {
        ZCE_LOGMSG(RS_ERROR, "[LUATIE][%s][%s] is not tie to lua,[%d][%s]? May be you don't register or name conflict? "
            typeid(val_type).name(),
            class_name<val_type >::name());
        lua_pop(state, 1);
        return;
    }
    lua_setmetatable(state, -2);
    return;

}

///Ϊ���ģ�庯���ݹ�׼���ĺ���
template<typename val_type, typename... val_tlist>
void push_stack(lua_State *state, val_type val, val_tlist ... val_s)
{
    push_stack(state, val, val_s...);
    return;
}

//����һ������
template<typename val_type  >
void push_stack(lua_State *state,
    typename std::enable_if<std::is_reference<val_type>::value>::type val)
{
    new(lua_newuserdata(state,
        sizeof(ref_2_udat<val_type>))) ref_2_udat<val_type>(val);

    //����п���Ϊconst����һЩ���
    //if (std::is_const<val_type>::value)

    //����������ƣ�����metatable
    lua_pushstring(state, class_name<val_type >::name());
    lua_gettable(state, LUA_GLOBALSINDEX);
    if (lua_istable(state, -1))
    {
        ZCE_LOGMSG(RS_ERROR, "[LUATIE][%s][%s] is not tie to lua,[%d][%s]? May be you don't register or name conflict? "
            typeid(val_type).name(),
            class_name<val_type >::name());
        lua_pop(state, 1);
        return;
    }
    lua_setmetatable(state, -2);
    return;
}

//����һ��ָ��
template<typename val_type  >
void push_stack(lua_State *state,
    typename  std::enable_if<std::is_object<val_type>::value>::type *val)
{
    if (val)
    {
        new(lua_newuserdata(state, sizeof(ptr_2_udat<val_type>))) ptr_2_udat<val_type>(val);

        lua_pushstring(state, class_name<val_type >::name());
        lua_gettable(state, LUA_GLOBALSINDEX);
        if (!lua_istable(state, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE][%s][%s] is not tie to lua,[%d][%s]? May be you don't register or name conflict? "
                typeid(val_type).name(),
                class_name<val_type >::name());
            lua_pop(state, 1);
            return;
        }
        lua_setmetatable(state, -2);
    }
    else
    {
        lua_pushnil(state);
    }
    return;
}

template<> void push_stack(lua_State *state, char val);
template<> void push_stack(lua_State *state, unsigned char val);
template<> void push_stack(lua_State *state, short val);
template<> void push_stack(lua_State *state, unsigned short val);
template<> void push_stack(lua_State *state, int val);
template<> void push_stack(lua_State *state, unsigned int val);
template<> void push_stack(lua_State *state, float val);
template<> void push_stack(lua_State *state, double val);
template<> void push_stack(lua_State *state, char *val);
template<> void push_stack(lua_State *state, const char *val);
template<> void push_stack(lua_State *state, bool val);
template<> void push_stack(lua_State *state, int64_t val);
template<> void push_stack(lua_State *state, uint64_t val);
template<> void push_stack(lua_State *state, const std::string &val);

//���������long�ı�������Ϊlong�޷���ֲ��ֻ���壬���Բ�ʵ��
template<> void push_stack(lua_State *state, long val);
template<> void push_stack(lua_State *state, unsigned long val);


//ö��
template<typename val_type  >
int push_stack(lua_State *state,
                typename  std::enable_if<std::is_enum<val_type>::value>::type val)
{
    lua_pushnumber(state, val);
    return 0;
}

//���ڷ�object���͵����ݣ����ܷ������ú�ָ�룬�����ע�⣡����

template<typename val_type>
static
typename val_type read_stack(lua_State *state, int index)
{
    return *(val_type *)(((lua_udat_base *)lua_touserdata(state, index))->obj_ptr_);
}

///��ȡö��ֵ
template<typename val_type> 
static 
typename std::enable_if<std::is_enum<val_type>::value, val_type>::type 
read_stack(lua_State *state, int index)
{
    return (val_type)lua_tonumber(state, index);
}

///�Ӷ�ջ�ж�ȡĳ������
template<> char               *read_stack(lua_State *state, int index);
template<> const char         *read_stack(lua_State *state, int index);
template<> char                read_stack(lua_State *state, int index);
template<> unsigned char       read_stack(lua_State *state, int index);
template<> short               read_stack(lua_State *state, int index);
template<> unsigned short      read_stack(lua_State *state, int index);
template<> int                 read_stack(lua_State *state, int index);
template<> unsigned int        read_stack(lua_State *state, int index);
template<> float               read_stack(lua_State *state, int index);
template<> double              read_stack(lua_State *state, int index);
template<> bool                read_stack(lua_State *state, int index);
template<> int64_t             read_stack(lua_State *state, int index);
template<> uint64_t            read_stack(lua_State *state, int index);
template<> std::string         read_stack(lua_State *state, int index);


///
template<typename val_type>
val_type pop_stack(lua_State *state)
{
    val_type t = read_stack<val_type>(state, -1);
    lua_pop(state, 1);
    return t;
}

///��LUA��һ���󶨵����飨ָ�룩����õ�����ĵ�ֵ
template<typename array_type>
int array_meta_get(lua_State *state)
{
    //�������
    int index = static_cast<int>( lua_tonumber(state, -1));
    array_type *ptr = ((array_type *)lua_touserdata(state, -2));
    push_stack<array_type>(state, ptr[index]);
    //index Ӧ���������
    //
    lua_remove(state, -2);
    return 1;
}

///��LUA��������һ���󶨵����飨ָ�룩��ĳ��ֵ
template<typename array_type>
int array_meta_set(lua_State *state)
{
    array_type *ptr = ((array_type *)lua_touserdata(state, -3));
    int index = static_cast<int>(lua_tonumber(state, -2));

    ptr[index] = read_stack<array_type>(state, -1);

    lua_remove(state, -2);
    lua_remove(state, -1);

    return 0;
}



///
int class_meta_get(lua_State *state);

///
int class_meta_set(lua_State *state);


///��C++11������Ч�����ʵ�ֵ�ȫ��(����static)�����Ž�
template<typename ret_type, typename... args_type>
struct g_cfunctor
{
    static int invoke(lua_State *state)
    {
        //push�ǽ���������ջ
        void *upvalue_1 = lua_touserdata(state, lua_upvalueindex(1));
        ret_type(*fun_ptr)(args_type...) = (ret_type( *)(args_type...)) (upvalue_1);

        if (std::is_void<ret_type>::value)
        {
            int para_idx = 0;
            fun_ptr(read_stack<args_type>(state, ++para_idx)...);
            return 0;
        }
        else
        {
            int para_idx = 0;
            push_stack<ret_type>(state, fun_ptr(read_stack<args_type>(state, para_idx++)...));
            return 1;
        }
    }
};


///��C++11������Ч�����ʵ�ֵ��ຯ���Ž�
template<typename class_type,typename ret_type, , typename... args_type>
struct mem_cfunctor
{
    static int invoke(lua_State *state)
    {
        //push�ǽ���������ջ
        void *upvalue_1 = lua_touserdata(state, lua_upvalueindex(1));
        ret_type(*fun_ptr)(args_type...) = (ret_type(class_type::*)(args_type...)) (upvalue_1);

        if (std::is_void<ret_type>::value)
        {
            int para_idx = 0;
            fun_ptr(read_stack<args_type>(state, ++para_idx)...);
            return 0;
        }
        else
        {
            int para_idx = 0;
            push_stack<ret_type>(state, fun_ptr(read_stack<args_type>(state, para_idx++)...));
            return 1;
        }
    }
};


///��Ա�����Ļ��࣬���ڳ�ʼ��
///memvar_base ע���䲻��һ����ģ��ĺ�����
///�����Ϳ��Ա�֤ͨ��LUA user data�����void * ת��Ϊ memvar_base *
///��ͨ��memvar_base *�ĵ��� get, set ��æǡǡ��ֱ�ӵ��õ�������
///member_var <class_type,var_type>
struct memvar_base
{
    virtual void get(lua_State *state) = 0;
    virtual void set(lua_State *state) = 0;
};


//class_type Ϊclass ���ͣ�
//var_type Ϊ��������
template<typename class_type, typename var_type>
struct member_var : memvar_base
{
    //�����ĳ�Աָ��
    var_type class_type::*var_ptr_;

    member_var(var_type class_type::*val) :
        var_ptr_(val)
    {
    }

    ///�������ĳ��
    void set(lua_State *state)
    {
        //��ο������ǲ����е���Σ���ʵ���ɵ��������������Σ�����������ĳ�Ա����ָ����ɳ�Ա��д��
        //float A::*pfl = &A::float_var;
        //A a1;  a1.*pfl = 0.5f;
        //�����Ҫ��лairfu GG�ľ��Ľ���
        read_stack<class_type *>(state, 1)->*(var_ptr_) = read_stack<var_type>(state, 3);
    }

    //get��LUA��ȡ�Ĳ�����Ҳ���ǰ�C++�����ݶ�ȡ��LUA���棬������PUSH
    void get(lua_State *state)
    {
        //read_stack��ʵ���ǰ���Ķ����ָ���ȡ������
        //Ȼ��ͨ�����Աָ�룬�ѳ�Ա��ȡ����
        push_stack<var_type>(state,
                             read_stack<class_type *>(state, 1)->*(var_ptr_) );
    }
};



//��װ��Ĺ��캯����LUAʹ��
template<typename class_type, typename... args_type>
int constructor(lua_State *state)
{
    //new һ��user data����<T>�Ĵ�С,ͬʱ��ͬʱ��placement new �ķ�ʽ��
    //��ָ��ʽlua_newuserdata����ģ���ɹ��캯��
    int para_idx = 1;
    new(lua_newuserdata(state,
                        sizeof(val_2_udat<class_type> )))
    val_2_udat<class_type>(read_stack<args_type>(state, ++para_idx)...);

    lua_pushstring(state, class_name<class_type>::name());
    lua_gettable(state, LUA_GLOBALSINDEX);

    lua_setmetatable(state, -2);

    return 1;
}

//����USER_DATA�Ļ��������,����userdata_base��ʵ��һ��LUAʹ�õ�userdata����Ļ��࣬
//���������3�֣�val,ptr,ref,����val���������ͷŶ���ptr��ref�Ķ���ʲô����������
//�����ͱ�֤�����㴫�ݸ�LUAʲô�����ǵ��������ڶ�����ȷ�ģ�
int destroyer(lua_State *state);



};

//=======================================================================================================
//LUA Ь�������ڰ�����C++��
class ZCE_Lua_Tie
{

public:
    ZCE_Lua_Tie();
    ~ZCE_Lua_Tie();


    /*!
    * @brief      ��lua state
    * @return     int
    * @param      open_libs  �Ƿ�򿪳��õ�һЩLUA��
    * @param      reg_common �Ƿ�ע��һЩ����
    */
    int open(bool open_libs,
             bool reg_common);
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


    /*!
    * @brief      ��LUA����һ������LUA���ԣ�ȫ�ֱ��������ƺͱ�����Ӧֵ�Ŀ�����
    * @tparam     val_type ������������ͣ������val������LUA���汣�������������ptr��ref��lua�ڲ�����ֱ���޸��������
    * @param      name ����
    * @param      val  ����ı�����ע�����Ҫ�������ã���Ҫset_gvar<var_type &>(ref)������д
    */
    template<typename var_type>
    void set_gvar(const char *name, typename var_type var)
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        //ģ�庯��������val_type�������push
        ZCE_LUA::push_stack(lua_state_, var);
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

    ///�������ƣ���LUA��ȡһ������
    template<typename var_type>
    typename var_type get_gvar(const char *name)
    {
        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);
        return ZCE_LUA::pop_stack<var_type>(lua_state_);
    }


    ///��LUA����һ������,��LUA�ڲ�����һ��table��
    template<typename val_type>
    void set_garray(const char *name,
                    size_t ary_num,
                    typename val_type ary_data[],
                    bool use_reference = false,
                    bool read_only = false)
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        //�������ʹ�����÷�ʽʹ�ã���ô����lua�б���һ�ݿ���
        if (!use_reference)
        {
            lua_createtable(lua_state_, static_cast<int>( ary_num), 0);
            for (size_t i = 0; i < ary_num; ++i)
            {
                //�൱��lua_rawseti.ֻ��lua_rawseti���ڲ���ʵ�����˶�ջ˳�����Ӵ��֣�
                //����,����ȷ��֪��lua_rawseti�Ƿ���һЩ�Ż�������Ϊ�о�lua��hashtable����һЩ���⴦��ģ�
                lua_pushnumber(lua_state_, static_cast<int>(i));
                ZCE_LUA::push_stack(lua_state_, ary_data[i]);
                lua_rawset(lua_state_, -3);
            }
            //���ϣ����ֻ��
            if (read_only)
            {
                //��������ֻ��
                lua_newtable(lua_state_);

                lua_pushstring(lua_state_, "__newindex");
                lua_pushcclosure(lua_state_, ZCE_LUA::newindex_onlyread, 0);
                lua_rawset(lua_state_, -3);

                lua_setmetatable(lua_state_, -2);
            }
        }
        //���÷�ʽ��ֱ��ʹ��ָ�뵱��user data,ͬʱ����meta table
        else
        {
            new (lua_newuserdata(lua_state_, sizeof(val_type *)))(val_type *)(ary_data);
            lua_newtable(lua_state_);

            lua_pushstring(lua_state_, "__array_size");
            lua_pushnumber(lua_state_, static_cast<int>( ary_num));
            lua_rawset(lua_state_, -3);
            
            lua_pushstring(lua_state_, "__index");
            lua_pushcclosure(lua_state_, ZCE_LUA::array_meta_get<val_type>, 0);
            lua_rawset(lua_state_, -3);

            //��ֻ��
            if (!read_only)
            {
                lua_pushstring(lua_state_, "__newindex"); 
                lua_pushcclosure(lua_state_, ZCE_LUA::array_meta_set<val_type>, 0);
                lua_rawset(lua_state_, -3);
            }
            //���ֻ����__newindex
            else
            {
                lua_pushstring(lua_state_, "__newindex");
                lua_pushcclosure(lua_state_, ZCE_LUA::newindex_onlyread, 0);
                lua_rawset(lua_state_, -3);
            }
            //Ҫ��Ҫ����__gc ?
        }

        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

    ///��LUA�л�ȡһ��ȫ�ֵ�����
    template<typename ary_type>
    int get_garray(const char *name,
                 size_t ary_num,
                 typename const ary_type ary_data[]  )
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        //�������һ��table������Ŷ
        if (!lua_istable(lua_state_, -1))
        {
            lua_pop(lua_state_, 1);
            return -1;
        }

        for (size_t i = 0; i < ary_num; ++i)
        {
            lua_pushnumber(lua_state_, static_cast<int>(i));
            lua_gettable(lua_state_, -2)
            if (lua_isnil(lua_state_, -1))
            {
                return -1;
            }
            ary_data[i] = pop_stack<ary_type>(lua_state_ );
        }
    }

    /*!
    * @brief      ��LUAע��һ��ȫ�ֺ�����������ľ�̬������lua����
    * @tparam     ret_type  ���ز�������
    * @tparam     args_type �����Ĳ������ͣ����
    * @param      name      ��LUAע��ĺ�������
    * @param      func      ע���C����
    */
    template<typename ret_type, typename... args_type>
    void tie_gfun(const char *name, ret_type(*func)(args_type...))
    {
        //��������
        lua_pushstring(lua_state_, name);
        //������ָ��ת��Ϊvoid * ����Ϊlightuserdata �����ջ����Ϊclosure��upvalue����
        lua_pushlightuserdata(lua_state_, (void *)func);
        //functorģ�庯��������closure,
        lua_pushcclosure(lua_state_, ZCE_LUA::g_cfunctor<ret_type, args_type...>::invoke, 1);
        //�������ȫ�ֻ�������
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

    /*!
    * @brief      ����LUA�ĺ�����û�з���ֵ���������ݲ�ȡ��һ������ֵ��
    * @tparam     args_type ���������б�
    * @return     int == 0 ��ʾ�ɹ�
    * @param      name ��������
    * @param      args �����б�
    */
    template<typename... args_type>
    int call_luafun_0(const char *name, args_type... args)
    {
        int ret = 0;
        //���������ĺ��������Ҽ�¼��ջ�ĵ�ַ
        lua_pushcclosure(lua_state_, ZCE_LUA::on_error, 0);
        int errfunc = lua_gettop(lua_state_);

        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);
        //������Ƿ��Ǻ���
        if (!lua_isfunction(lua_state_, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] call_luafun() attempt to call global `%s' (not a function)", name);
            lua_pop(lua_state_, 1);
            return -1;
        }

        //�����ջ������args
        ZCE_LUA::push_stack(lua_state_, args...);

        size_t arg_num = sizeof...(args);
        //����lua�ĺ�����Ĭ��ֻ��һ������ֵ��
        ret = lua_pcall(lua_state_,
                        static_cast<int>(arg_num),
                        ,
                        errfunc);
        if (ret != 0)
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] lua_pcall ret = %d", ret);
        }

        //�ڶ�ջɾ����������ĺ���
        lua_remove(lua_state_, errfunc);

        //ע���������û��ȡ������ֵ���������������û�з���ֵ

        return 0;
    }

    ///����LUA�ĺ�����ֻ��һ������ֵ
    template<typename ret_type1, typename... args_type>
    int call_luafun_1(const char *name, ret_type1 ret_val1, args_type... args)
    {
        int ret = 0;
        ret = call_luafun_0(args...);
        if (ret != 0)
        {
            return ret;
        }
        //�ڶ�ջ��������ֵ
        ret_val1 = ZCE_LUA::pop_stack<ret_type1>(lua_state_);
        return 0;
    }

    ///����LUA�ĺ���������������ֵ
    template<typename ret_type1, typename ret_type2, typename... args_type>
    int call_luafun_2(const char *name, ret_type1 ret_val1, ret_type2 ret_val2, args_type... args)
    {
        int ret = 0;
        ret = call_luafun_0(args...);
        if (ret != 0)
        {
            return ret;
        }
        //�ڶ�ջ��������ֵ
        ret_val1 = ZCE_LUA::pop_stack<ret_type1>(lua_state_);
        ret_val2 = ZCE_LUA::pop_stack<ret_type2>(lua_state_);
        return 0;
    }

    // class init
    //��ĳ�ʼ������class����lua��ʹ��
    //�������metatable�ı�����˵ԭ�͵ı�
    template<typename class_type>
    void tie_class(const char *name,
        bool read_only)
    {
        //��T������
        ZCE_LUA::class_name<class_type>::name(name);

        //�������
        lua_pushstring(lua_state_, name);
        //new һ��table�����table����Ϊ���������metatable�ģ�ĳ�̶ֳ���Ҳ����˵��ԭ�ͣ���
        lua_newtable(lua_state_);

        //__name���Ǳ�׼��Ԫ������������������ʹ��
        lua_pushstring(lua_state_, "__name");
        lua_pushstring(lua_state_, name);
        lua_rawset(lua_state_, -3);

        //��meta_get������Ϊ__index����
        lua_pushstring(lua_state_, "__index");
        lua_pushcclosure(lua_state_, ZCE_LUA::class_meta_get, 0);
        lua_rawset(lua_state_, -3);

        
        if (!read_only)
        { 
            //��ֻ�������meta_set������Ϊ__newindex����
            lua_pushstring(lua_state_, "__newindex");
            lua_pushcclosure(lua_state_, ZCE_LUA::class_meta_set, 0);
            lua_rawset(lua_state_, -3);
        }
        else
        {
            lua_pushstring(lua_state_, "__newindex");
            lua_pushcclosure(lua_state_, ZCE_LUA::newindex_onlyread, 0);
            lua_rawset(lua_state_, -3);
        }

        //�������պ���
        lua_pushstring(lua_state_, "__gc");
        lua_pushcclosure(lua_state_, ZCE_LUA::destroyer, 0);
        lua_rawset(lua_state_, -3);

        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }



    // class_type ����
    // constructor_fun �ǹ��캯���ķ�װ��ZCE_LUA::constructor
    template<typename class_type, typename constructor_fun>
    int class_constructor(constructor_fun func)
    {
        //����������ƣ�ȡ�����metatable�ı�����˵ԭ�͡�
        lua_pushstring(lua_state_, ZCE_LUA::class_name<class_type>::name());
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        //���ջ���ǲ���һ�������д�����
        if (!lua_istable(lua_state_, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] class name[%s] is not tie to lua.",
                       ZCE_LUA::class_name<class_type>::name());
            ZCE_ASSERT(false);
            lua_pop(lua_state_, 1);
            return -1;
        }

        //��������metatable�ı�����һ��metatable������������һ��__call�Ķ�Ӧ����
        //������Ŀ���������ģ�__call�Ƕ�Ӧһ��()���ã���ʵ�岻�Ǻ���ʽ���ĵ��ú���
        //LUA�г��������ĵ��ã�var1,var2�ǹ���������͹��캯��һ��������ط�Ҫ��лfergzhang GG�İ���
        //object =class_name(var1,var2)
        lua_newtable(lua_state_);

        lua_pushstring(lua_state_, "__call");
        lua_pushcclosure(lua_state_, func, 0);
        lua_rawset(lua_state_, -3);
        //�������table��Ϊclass ԭ�͵�metatable.
        //����˵�������table��Ϊclass metatable��metatable.
        lua_setmetatable(lua_state_, -2);

        lua_pop(lua_state_, 1);
        return 0;
    }

    // 
    // class_type �󶨵���
    // var_type �ǰ󶨵ı��������ͣ�
    // base_type ��Ա�������࣬һ������ΪT��BASE��һ����,����Ҳ���԰�һ������ĳ�Ա���������Lua��meta table���棬
    template<typename class_type, typename base_type, typename var_type>
    int class_member_var(const char *name, var_type base_type::*val)
    {
        //����������ƣ�ȡ�����metatable�ı�����˵ԭ�͡�
        lua_pushstring(lua_state_, ZCE_LUA::class_name<class_type>::name());
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        //
        if (!lua_istable(lua_state_, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] class name[%s] is not tie to lua.",
                ZCE_LUA::class_name<class_type>::name());
            ZCE_ASSERT(false);
            lua_pop(lua_state_, 1);
            return -1;
        }

        lua_pushstring(lua_state_, name);
        //mem_var �̳���var_base,ʵ�ʵ��õ�ʱ������var_base���麯����ɻص���
        new(lua_newuserdata(lua_state_, sizeof(ZCE_LUA::member_var<base_type, var_type>)))  \
            ZCE_LUA::member_var<base_type, var_type>(val);
        lua_rawset(lua_state_, -3);

        lua_pop(lua_state_, 1);
        return 0;
    }


    template<typename class_type, typename F>
    void class_def(lua_State *L, const char *name, F func)
    {
        //����������ƣ�ȡ�����metatable�ı�����˵ԭ�͡�
        lua_pushstring(lua_state_, ZCE_LUA::class_name<class_type>::name());
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        if (lua_istable(L, -1))
        {
            lua_pushstring(L, name);
            //�����ĺ���ָ����Ϊupvalue_�ġ�
            //ע���������ĳ�Աָ�루���ӽӽ�size_t����������ʵ�ʵ�ָ�룬�������������light userdata
            new(lua_newuserdata(L, sizeof(F))) F(func);
            //
            push_functor(L, func);
            lua_rawset(L, -3);
        }
        lua_pop(L, 1);
    }


protected:

    //
    lua_State   *lua_state_;
};

#if defined (ZCE_OS_WINDOWS)
#pragma warning ( pop )
#endif

#endif //#if defined  ZCE_USE_LUA && defined ZCE_SUPPORT_CPP11

#endif // ZCE_LIB_SCRIPT_LUA_H_

