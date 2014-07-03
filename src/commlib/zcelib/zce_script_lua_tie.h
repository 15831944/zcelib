/*!
* @copyright  2004-2014  Apache License, Version 2.0 FULLSAIL
* @filename   zce_script_lua_tie.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version
* @date       Sunday, June 22, 2014
* @brief      ��������ǲο�Tinkerʵ�ֵģ���Ȼ��лԭ����
*             ���쿴��һ�Σ��׷���ǧ�ɣ�����д���У���֪������δ���������
*             ��������Ŀ��һ�������Ҷ���������һ���ű��������Ǻ��棬����
*             �������˻�������һ��
*
* @details
*
*
*
* @note       �ҵ������꣬����Ҫ��һ�����ᣬ��Ҫ��һ������������֤�����˲�������Ҫ�����˼ң���ʧȥ�Ķ���һ��Ҫ�����û�����
*             -- ��Ӣ�۱�ɫ�� С���
*             2014��6��13�����ϣ������ɾ�����ı���������5��1��
*             
*             2014��6��25�� ���ˣ����룬���ˣ�Ƥ���壬���ˣ��������
*             �������۵���ǿ��������Ӧ�������׸��Ƥ�����ͱ�
*             ��ͤ�⣬�ŵ���
*             ���ݱ�����
*             ������������
*             Ϧ��ɽ��ɽ
*             ��֮�ģ���֮��
*             ֪��������              
*             һư(��)[a]�Ǿƾ��໶
*             �������κ�         
*/



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

///@namespace ZCE_LUA �����ʹ������ZCE_LUA�����ֿռ䣬��Ϊ��ZCE_LIB
///�о�������ȫ�����ͻ��
namespace ZCE_LUA
{

///ֻ����table��newdindex
int newindex_onlyread(lua_State *state);

///dump lua���еĵĶ�ջ�����ڼ��lua����ʱ�����⣬�������
int dump_luacall_stack(lua_State *state);

///dump C����lua�Ķ�ջ��
int dump_clua_stack(lua_State *state);

///�ں�����������Ĵ�����
int on_error(lua_State *state);



/*!
* @brief      ��ģ�庯��������æʵ��һ��������
*             ����ͨ��class �ҵ���Ӧ�������ƣ�ע�ᵽLUA�����ƣ���
* @tparam     class_type ����
*/
template<typename class_type>
class class_name
{
public:
    /*!
    * @brief      ��¼(����)���͵�ȫ�ֵ�����
    * @return     const char* ���ص�����
    * @param      name        ��¼�����ƣ��ǿյ�ʱ�������ƣ�Ϊ��ȡ��ԭ����һ��
    * @note
    */
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

/*!
* @brief      USER DATA�Ļ��࣬����һ��void *��ָ��
*             ���ڼ�¼���ݵ�ָ�룬ͨ���̳�ʵ�ֶ���val��ptr��ref�Ķ������
*/
class lua_udat_base
{
public:
    lua_udat_base(void *p)
        : obj_ptr_(p)
    {
    }
    ///������virtual
    virtual ~lua_udat_base()
    {
    }
    void *obj_ptr_;
};

/*!
* @brief      ��һ��valֵ����LUA USER DATA�ķ�װ
* @tparam     val_type ֵ������
*/
template<typename val_type>
class val_2_udat :public lua_udat_base
{
public:
    val_2_udat()
        : lua_udat_base(new val_type)
    {
    }

    //args_type�ǹ��캯���Ĳ���,args_type1 ���ڵ�Ŀ���Ǻ�Ĭ�Ϲ��캯���ֿ���
    template<typename args_type1, typename ...args_tlist>
    val_2_udat(args_type1 arg1, args_tlist ...arg) : lua_udat_base(new val_type(arg1, arg ...))
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

/*!
* @brief      ��һ��ptrָ�����LUA USER DATA�ķ�װ
* @tparam     val_type ָ�������
*/
template<typename val_type>
class ptr_2_udat :public lua_udat_base
{
public:
    ptr_2_udat(val_type t)
        : lua_udat_base((void *)t)
    {
    }
};

/*!
* @brief      ��һ��ref���÷���LUA USER DATA�ķ�װ
* @tparam     val_type ���õ�����
*/
template<typename val_type>
class ref_2_udat :public lua_udat_base
{
public:
    //ע���һ��&t��ʾ�����ò������ڶ����Ǳ�ʾ����ָ���lua_udat_base����
    ref_2_udat(val_type &t) :
        lua_udat_base(&t)
    {
    }
};

/*!
* @brief      ��һ�������������LUA USER DATA�ķ�װ
* @tparam     val_type ���õ�����
*/
template<typename ary_type>
class arrayref_2_udat :public lua_udat_base
{
public:
    ///���캯��
    arrayref_2_udat(ary_type *ary_ptr, size_t ary_size, bool read_only) :
        lua_udat_base(ary_ptr),
        ary_size_(ary_size),
        read_only_(read_only)
    {
    }

    ///�ռ䳤��
    size_t ary_size_;
    ///�Ƿ�ֻ��
    bool   read_only_;

};

//-------------------------------------------------------------------------------
///Push һ�����ݵ�Lua�Ķ�ջ��

///��ͨ�õĺ�������һ������������ͨ�����غ��ػ������ˣ��������ֻ��ӡ��һ��������Ϣ
template<typename val_type >
void push_stack(lua_State * state, typename val_type val)
{
    ZCE_LOGMSG(RS_ERROR, "[LUATIE]Type[%s] not support in this code?", typeid(val_type).name());
    new(lua_newuserdata(state,
        sizeof(val_2_udat<val_type>))) val_2_udat<val_type>(val);

    //����������ƣ�����metatable
    lua_pushstring(state, class_name<val_type >::name());
    lua_gettable(state, LUA_GLOBALSINDEX);
    if (lua_istable(state, -1))
    {
        ZCE_LOGMSG(RS_ERROR, "[LUATIE][%s][%s] is not tie to lua,[%d][%s]? May be you don't register or name conflict? ",
            typeid(val).name(),
            class_name<val_type >::name());
        lua_pop(state, 1);
        return;
    }
    lua_setmetatable(state, -2);
    return;
}

/*!
* @brief      PUSHһ��object���ݵ���ջ��
* @tparam     val_type ֵ�����ͣ�
* @param      state  lua state
* @param      val    ֵ��enable_if�� is_object���÷��Ͳ�ϸ��˵����
*/
//template<typename val_type  >
//void push_stack(lua_State *state,
//    typename std::enable_if<std::is_object<val_type>::value, val_type>::type val)
//{
//
//    return;
//
//}

///Ϊ���ģ�庯���ݹ�׼���ĺ���
template<typename val_type, typename... val_tlist>
void push_stack(lua_State *state, val_type val, val_tlist ... val_s)
{
    push_stack<val_type>(state, val);
    push_stack(state, val_s...);
    return;
}


/*!
* @brief      ����һ������
* @tparam     val_type  ���õ�����
* @param      state     lua state
* @param      ref       ���ã�
* @note
*/
template<typename val_type  >
void push_stack(lua_State *state,
     typename std::enable_if<std::is_reference<val_type>::value, val_type>::type ref)
{
    //
    new(lua_newuserdata(state,
                        sizeof(ref_2_udat<val_type>))) ref_2_udat<val_type>(ref);

    //����������ƣ�����metatable
    lua_pushstring(state, class_name<val_type >::name());
    lua_gettable(state, LUA_GLOBALSINDEX);
    if (lua_istable(state, -1))
    {
        ZCE_LOGMSG(RS_ERROR, "[LUATIE][%s][%s] is not tie to lua,[%d][%s]? May be you don't register or name conflict? ",
                   typeid(ref).name(),
                   class_name<val_type >::name());
        lua_pop(state, 1);
        return;
    }
    lua_setmetatable(state, -2);
    return;
}


/*!
* @brief      ����һ��ָ��
* @tparam     val_type ָ�������
* @param      state    Lua state
* @param      ptr      �����ָ��
*/
template<typename val_type  >
void push_stack(lua_State *state, 
    typename std::enable_if<std::is_pointer<val_type>::value, val_type>::type ptr)
{
    if (ptr)
    {
        new(lua_newuserdata(state, sizeof(ptr_2_udat<val_type>))) ptr_2_udat<val_type>(ptr);

        lua_pushstring(state, class_name<val_type >::name());
        lua_gettable(state, LUA_GLOBALSINDEX);
        if (!lua_istable(state, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE][%s][%s] is not tie to lua,[%d][%s]? May be you don't register or name conflict? ",
                       typeid(ptr).name(),
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

//ö��
template<typename val_type  >
int push_stack(lua_State *state,
               typename  std::enable_if<std::is_enum<val_type>::value>::type val)
{
    lua_pushnumber(state, val);
    return 0;
}

///����һ�����������
template<typename array_type  >
void push_stack(lua_State *state, typename arrayref_2_udat<array_type> & ary_dat)
{
    new (lua_newuserdata(state, sizeof(arrayref_2_udat<array_type>)))
    arrayref_2_udat<array_type>(ary_dat);
    lua_newtable(state);

    lua_pushstring(state, "__array_size");
    lua_pushnumber(state, static_cast<int>(ary_dat.ary_size_));
    lua_rawset(state, -3);

    lua_pushstring(state, "__index");
    lua_pushcclosure(state, ZCE_LUA::array_meta_get<array_type>, 0);
    lua_rawset(state, -3);

    //��ֻ��
    if (!ary_dat.read_only_)
    {
        lua_pushstring(state, "__newindex");
        lua_pushcclosure(state, ZCE_LUA::array_meta_set<array_type>, 0);
        lua_rawset(state, -3);
    }
    //���ֻ����__newindex
    else
    {
        lua_pushstring(state, "__newindex");
        lua_pushcclosure(state, ZCE_LUA::newindex_onlyread, 0);
        lua_rawset(state, -3);
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


//-------------------------------------------------------------------------------

///���ڷ�object���͵����ݣ����Ҫ�������ú�ָ�룬������������Щ����
///�����ע�⣡����

template<typename ret_type>
typename ret_type read_stack(lua_State *state, int index)
{
    if (std::is_pointer<ret_type>::value)
    {
        return (std::remove_pointer<ret_type>::type *)(((lua_udat_base *)lua_touserdata(state, index))->obj_ptr_);
    }
    else if (std::is_reference<ret_type>::value)
    {
        return *( std::remove_reference<ret_type>::type *)(((lua_udat_base *)lua_touserdata(state, index))->obj_ptr_);
    }
    else
    {
        return *(ret_type *)(((lua_udat_base *)lua_touserdata(state, index))->obj_ptr_);
    }
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


//-------------------------------------------------------------------------------


///��LUA��һ���󶨵����飨ָ�룩����õ�����ĵ�ֵ
template<typename array_type>
int array_meta_get(lua_State *state)
{
    //�������
    int index = static_cast<int>( lua_tonumber(state, -1));
    arrayref_2_udat<array_type> *ptr = ((arrayref_2_udat<array_type> *)lua_touserdata(state, -2));
    if (index < 1 && index > static_cast<int>( ptr->ary_size_ ))
    {
        ZCE_LOGMSG(RS_ERROR, "Lua script use error index [%d] to visit array %s[] size[%u].",
                   index,
                   typeid(array_type).name(),
                   static_cast<uint32_t>(ptr->ary_size_));
        ZCE_ASSERT(false);
        lua_pushnil(state);

    }
    else
    {
        array_type *ary_ptr = (array_type *)(ptr->obj_ptr_);
        //ע�����Ϊ�˷���Lua��ϰ�ߣ�LUA�������Ǵ�1��ʼ
        push_stack<array_type>(state, ary_ptr[index - 1]);
    }

    //index Ӧ���������

    return 1;
}

///��LUA��������һ���󶨵����飨ָ�룩��ĳ��ֵ
template<typename array_type>
int array_meta_set(lua_State *state)
{
    arrayref_2_udat<array_type> *ptr = ((arrayref_2_udat<array_type> *)lua_touserdata(state, -3));
    int index = static_cast<int>(lua_tonumber(state, -2));

    //��index���߽���
    if (index < 1 && index > static_cast<int>(ptr->ary_size_) )
    {
        ZCE_LOGMSG(RS_ERROR, "Lua script use error index [%d] to visit array %s[] size[%u].",
                   index,
                   typeid(array_type).name(),
                   static_cast<uint32_t>(ptr->ary_size_));
        ZCE_ASSERT(false);

    }
    else
    {
        array_type *ary_ptr = (array_type *)(ptr->obj_ptr_);
        //ע�����Ϊ�˷���Lua��ϰ�ߣ�LUA�������Ǵ�1��ʼ
        ary_ptr[index - 1] = read_stack<array_type>(state, -1);
    }

    return 0;
}

//-------------------------------------------------------------------------------

//����lua�ĺ������������ʵ��û��get����ֵ��ʵ��Ӧ��ʹ��ZCE_Lua_Tie�ķ�װ��

/*!
* @brief      ����LUA�ĺ�����û�з���ֵ���������ݲ�ȡ��һ������ֵ��
* @tparam     args_type ���������б�
* @return     int == 0 ��ʾ�ɹ�
* @param      fun_name ��������
* @param      ret_num  ����ֵ������
* @param      args     �����б�
*/
template<typename... args_type>
int call_luafun(lua_State *state, const char *fun_name, size_t ret_num, args_type... args)
{
    int ret = 0;
    //���������ĺ��������Ҽ�¼��ջ�ĵ�ַ
    lua_pushcclosure(state, ZCE_LUA::on_error, 0);
    int errfunc = lua_gettop(state);

    lua_pushstring(state, fun_name);
    lua_gettable(state, LUA_GLOBALSINDEX);
    //������Ƿ��Ǻ���
    if (!lua_isfunction(state, -1))
    {
        ZCE_LOGMSG(RS_ERROR, "[LUATIE] call_luafun() attempt to call global `%s' (not a function)", fun_name);
        lua_pop(state, 1);
        return -1;
    }

    //�����ջ������args
    ZCE_LUA::push_stack(state, args...);

    size_t arg_num = sizeof...(args);
    //����lua�ĺ�����
    ret = lua_pcall(state,
                    static_cast<int>(arg_num),
                    static_cast<int>(ret_num),
                    errfunc);
    if (ret != 0)
    {
        ZCE_LOGMSG(RS_ERROR, "[LUATIE] lua_pcall ret = %d", ret);
    }

    //�ڶ�ջɾ����������ĺ���
    lua_remove(state, errfunc);

    //ע���������û��ȡ������ֵ���������������û�з���ֵ

    return 0;
}




///��C++11������Ч�����ʵ�ֵ�ȫ��(����static)�����Ž�
template<typename ret_type, typename... args_type>
class g_functor
{
public:
    static int invoke(lua_State *state)
    {
        //push�ǽ���������ջ
        void *upvalue_1 = lua_touserdata(state, lua_upvalueindex(1));
        ret_type(*fun_ptr)(args_type...) = (ret_type( *)(args_type...)) (upvalue_1);

        size_t sz_par = sizeof...(args_type);
        int para_idx = static_cast<int>(sz_par);
        //���Ҫע�⣬��һ�����ص����⡣����ȷʵ�Ҳ���������ܣ��������ñ���ǿ��Եģ�
        //�����ģ�C++��ģ���εĺ�������չ����VS2013�ı���������GCC 4.8�ı������϶��в��㣬
        //����ֲ���˳��ߵ����������������Ǳ���������չ�����ǣ��ǲ��õı�׼��������˳�򣬴���
        //���ң������䴦��һ�������ͽ�������ջ�ˣ���C++�ı���������׼��ջ����˳���Ǵ��ҵ���
        //�������õ��Ĳ���˳����Ƿ��ġ�������ֻ�з�����ȡ������
        //������Ӧ����һ��bug���Ҳ�֪��������������޸�������⣬���㣬��ʱ����ֻ�У�����


        //�����Ƿ��з���ֵ��������δ����Ƿ�push_stack
        if (std::is_void<ret_type>::value)
        {
            fun_ptr(read_stack<args_type>(state, para_idx--)...);
            return 0;
        }
        else
        {
            push_stack<ret_type>(state, fun_ptr(read_stack<args_type>(state, para_idx--)...));
            return 1;
        }
    }
};


///lua��ȡ����������ݵ�meta table��__index��Ӧ����
int class_meta_get(lua_State *state);

///luaд������������ݵ�meta table��__newindex��Ӧ����
int class_meta_set(lua_State *state);


///���ø�ĸ�ĺ���
int class_parent(lua_State *state);


//��װ��Ĺ��캯����LUAʹ��
template<typename class_type, typename... args_type>
int constructor(lua_State *state)
{
    //new һ��user data����<T>�Ĵ�С,ͬʱ��ͬʱ��placement new �ķ�ʽ��
    //��ָ��ʽlua_newuserdata����ģ���ɹ��캯��
    int para_idx = 1;
    new(lua_newuserdata(state,
                        sizeof(val_2_udat<class_type>)))
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




/*!
* @brief      ��C++11������Ч�����ʵ�ֵ��ຯ���Ž�
* @tparam     class_type �������
* @tparam     ret_type   ����ֵ������
* @tparam     ...args_type ��εĲ��������б�
*/
template<typename class_type, typename ret_type, typename ...args_type>
class member_functor
{
public:
    static int invoke(lua_State *state)
    {
        //push�ǽ���������ջ
        void *upvalue_1 = lua_touserdata(state, lua_upvalueindex(1));
        
        typedef int (class_type::*mem_fun)(int, int);
        mem_fun fun_ptr = *(mem_fun *)(upvalue_1);

        class_type *obj_ptr = read_stack<class_type *>(state, -1);

        size_t sz_par = sizeof...(args_type);
        int para_idx = static_cast<int>(sz_par);

        //�����Ƿ��з���ֵ��������δ����Ƿ�push_stack
        if (std::is_void<ret_type>::value)
        {

            //�Һ޺���ָ�룬�Ҹ������Ա��ָ��,ע��������Ǹ����š�һ��Ҫ�������ҿ���1��Сʱ
            //Ϊʲô����--����ο�ǰ��Ľ��� g_functor
            (obj_ptr->*fun_ptr)(read_stack<args_type>(state, para_idx--)... );
            return 0;
        }
        else
        {
            push_stack<ret_type>(state,
                                 (obj_ptr->*fun_ptr)(read_stack<args_type>(state, \
                                 para_idx--)...));
            return 1;
        }
    }
};


/*!
* @brief      ��Ա�����Ĵ���Ļ��࣬����class_meta_get,class_meta_set�ڲ�����
* @note       memvar_base ע���䲻��һ����ģ��ĺ�������һ������
*             �����Ϳ��Ա�֤ͨ��LUA user data�����void * ת��Ϊ memvar_base *
*             ��ͨ��memvar_base *�ĵ��� get, set ��æǡǡ��ֱ�ӵ��õ�������
*             member_var <class_type,var_type>
*/
class memvar_base
{
public:
    virtual void get(lua_State *state) = 0;
    virtual void set(lua_State *state) = 0;
};


/*!
* @brief      ��Ա�����󶨵ĸ����࣬Ҳ����user data�������
*
* @tparam     class_type class_type Ϊclass ���ͣ�
* @tparam     var_type   Ϊ��������
*/
template<typename class_type, typename var_type>
class member_var : memvar_base
{
public:
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

    //�����ĳ�Աָ��
    var_type class_type::*var_ptr_;
};

//
template<typename class_type, typename ary_type, size_t ary_size>
class member_array : memvar_base
{
public:
    //���캯��
    member_array(typename ary_type (class_type::*mem_ary_ptr)[ary_size],
                 bool read_only) :
        mem_ary_ptr_(mem_ary_ptr),
        read_only_(read_only)
    {
    }


    ///���ڳ�Ա���飬����û���κ����壬
    void set(lua_State *state)
    {
        luaL_error(state, "Member array ptr is read only ,can't modify,please check your code.");
    }

    //get��LUA��ȡ�Ĳ�����Ҳ���ǰ�C++�����ݶ�ȡ��LUA���棬������PUSH
    void get(lua_State *state)
    {
        //read_stack��ʵ���ǰ���Ķ����ָ���ȡ������
        //Ȼ��ͨ�����Աָ�룬�ѳ�Ա��ȡ����
        ary_type *ary_ptr = read_stack<class_type *>(state, 1)->*(mem_ary_ptr_);
        arrayref_2_udat<ary_type> ary_dat(ary_ptr, ary_size, read_only_);
        push_stack(state, ary_dat);
    }

    ///��Ա�����ָ��
    ary_type(class_type:: *mem_ary_ptr_)[ary_size];

    ///
    bool                   read_only_;
};


};  //namespace ZCE_LUA


//=======================================================================================================
//Tie class to lua���﷨�ǣ����庯���Ľ�����ο�ZCE_Lua_Tie
class ZCE_Lua_Tie;

template <typename class_type>
class Candy_Tie_Class
{
public:
    //
    Candy_Tie_Class(ZCE_Lua_Tie *lua_tie,
        bool read_only):
        lua_tie_(lua_tie),
        read_only_(read_only)
    {
    }

    template <typename construct_fun >
    Candy_Tie_Class& construct(construct_fun func)
    {
        lua_tie_->class_constructor<class_type, construct_fun >(func);
        return *this;
    }

    template <typename var_type >
    Candy_Tie_Class& mem_var(const char *name, var_type class_type::*val)
    {
        lua_tie_->class_mem_var<class_type, var_type >(name, val);
        return *this;
    }

    template <typename array_type ,size_t array_size>
    Candy_Tie_Class& mem_ary(const char *name, array_type (class_type::*ary)[array_size])
    {
        lua_tie_->class_mem_ary<class_type, array_type, array_size >(name, ary, read_only_);
        return *this;
    }

    //�󶨺���
    template<typename ret_type, typename... args_type>
    Candy_Tie_Class& mem_fun(const char *name, typename ret_type(class_type::*func)(args_type...))
    {
        lua_tie_->class_mem_fun<class_type>(name, func);
        return *this;
    }

protected:
    ///Lua�Ľ�������״̬
    ZCE_Lua_Tie   *lua_tie_ = nullptr;
    
    ///������Ƿ���ֻ���ķ�ʽ
    bool           read_only_ = false;
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
    void tie_int64();
    ///��LUAע��uint64_t������
    void tie_uint64();
    ///��LUAע��std::string������
    void tie_stdstring();

    ///��LUAע��ö��ֵ
    void tie_enum(const char *name, size_t item_num, ...);


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
        ZCE_LUA::push_stack<var_type>(lua_state_, var);
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



    ///��LUA����һ�����������,��LUA�ڲ�����һ����ص�userdata��
    template<typename array_type>
    void set_garray(const char *name,
                    typename array_type ary_data[],
                    size_t ary_size,
                    bool read_only = false)
    {
        ZCE_LUA::arrayref_2_udat<array_type> aux_ary(ary_data, ary_size, read_only);
        //���ƶ���
        lua_pushstring(lua_state_, name);
        push_stack(lua_state_, aux_ary);
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

    ///��LUA�л�ȡһ��ȫ�ֵ�����
    template<typename ary_type>
    int get_garray(const char *name,
                   typename ary_type ary_data[],
                   size_t &ary_size)
    {
        //���ƶ���
        lua_pushstring(lua_state_, name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        //�������һ��table������Ŷ
        if (!lua_isuserdata(lua_state_, -1))
        {
            lua_remove(lua_state_, -1);
            return -1;
        }
        ZCE_LUA::arrayref_2_udat<array_type> aux_ary = 
            *(aux_ary *)lua_touserdata(lua_state_, -1);
        ary_size = aux_ary.ary_size_;
        for (size_t i = 0; i < ary_size; ++i)
        {
            ary_data[i] = *((ary_type *)aux_ary.obj_ptr_ + i);
        }
        return 0;
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
        lua_pushcclosure(lua_state_, ZCE_LUA::g_functor<ret_type, args_type...>::invoke, 1);
        //�������ȫ�ֻ�������
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

    /*!
    * @brief      ʹ�õ������������飬vector������,��LUA�е�һ��table�У�
    * @tparam     input_iter ����ĵ�������
    * @param      table_name table������
    * @param      first      ��һ��begin������
    * @param      last       ���һ��end�ĵ�����
    * @note       ��Ȼlist������Ҳ�ܷŽ�ȥ����ûɶ���壬��luatable���棬
    *             tableʹ��number���в�ѯ�ģ������������Υ��list��Ч��
    */
    template<class input_iter >
    void to_luatable(const char *table_name,
                     typename const input_iter first,
                     typename const input_iter last)
    {
        lua_pushstring(lua_state_, table_name);
        lua_createtable(lua_state_, 
            static_cast<int>(std::distance(first, last)), 0);
        typename input_iter iter_temp = first;
        for (int i = 0; iter_temp != last; iter_temp++, i++)
        {
            //Lua��ʹ��ϰ�������Ǵ�1��ʼ
            lua_pushnumber(lua_state_, i + 1);
            ZCE_LUA::push_stack<std::iterator_traits<input_iter>::value_type >(lua_state_, *iter_temp);
            lua_settable(lua_state_, -3);
        }
        lua_settable(lua_state_, LUA_GLOBALSINDEX);
    }

    /*!
    * @brief      ͨ����������һ��map,����unorder_map����lua table��
    * @tparam     map_iter   ����������
    * @param      table_name �������
    * @param      first      ��һ����������
    * @param      last       ���һ��������
    * @param      map_iter::second_type* �������û��ʹ�ã�����ʹ��SFINAE��ȷ��������map��
    * @note       �����ʵҲ֧��multi�ļ���map������������
    */
    template<class map_iter >
    void to_luatable(const char *table_name,
                     typename const map_iter first,
                     typename const map_iter last,
                     typename map_iter::second_type * = nullptr)
    {
        lua_pushstring(lua_state_, table_name);
        lua_createtable(lua_state_, 
            static_cast<int>(std::distance(first, last)), 0);

        typename input_iter iter_temp = first;
        for (; iter_temp != last; iter_temp++)
        {
            //��map��key��Ϊtable��key
            ZCE_LUA::push_stack<input_iter::first_type >(lua_state_, iter_temp.first);
            ZCE_LUA::push_stack<input_iter::second_type >(lua_state_, iter_temp.second);
            lua_settable(lua_state_, -3);
        }

        lua_settable(lua_state_, LUA_GLOBALSINDEX);
        return 0;
    }


    //��Lua�п������ݵ�C++�������У��������飬vector��vector��Ҫ��resize
    template<class container_type >
    int from_luatable(const char *table_name, container_type &container_dat)
    {
        //����������ƣ�ȡ�����metatable�ı�����˵ԭ�͡�
        lua_pushstring(lua_state_, table_name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        if (!lua_istable(lua_state_, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] table name[%s] is not tie to lua.",
                       table_name);
            ZCE_ASSERT(false);
            lua_pop(lua_state_, 1);
            return -1;
        }

        //first key ,ferg���Ұ��ⶫ�������ɵ�����
        lua_pushnil(lua_state_);
        while (lua_next(lua_state_, -2) != 0)
        {
            // uses 'key' (at index -2) and 'value' (at index -1)
            //
            int index = read_stack<int>(lua_state_, -2) - 1;
            container_dat[index] = read_stack<container_type::value_type>(lua_state_, -1);
            // removes 'value'; keeps 'key' for next iteration
            lua_pop(lua_state_, 1);
        }
        return 0;
    }

    //��Lua�п������ݵ�C++�������У��������飬vector��vector��Ҫ��resize
    template<class container_type >
    int from_luatable(const char *table_name,
                      container_type &container_dat,
                      typename container_type::key_type * = nullptr)
    {
        //����������ƣ�ȡ�����metatable�ı�����˵ԭ�͡�
        lua_pushstring(lua_state_, table_name);
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        if (!lua_istable(lua_state_, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] table name[%s] is not tie to lua.",
                       table_name);
            ZCE_ASSERT(false);
            lua_pop(lua_state_, 1);
            return -1;
        }

        //first key ,ferg���Ұ��ⶫ�������ɵ�����
        lua_pushnil(lua_state_);
        while (lua_next(lua_state_, -2) != 0)
        {
            // uses 'key' (at index -2) and 'value' (at index -1)
            container_dat[read_stack<container_type::key_type>(lua_state_, -2)] =
                read_stack<container_type::value_type>(lua_state_, -1);
            // removes 'value'; keeps 'key' for next iteration
            lua_pop(lua_state_, 1);
        }
        return 0;
    }

    ///����LUA�ĺ�����ֻ��һ������ֵ
    template<typename ret_type1, typename... args_type>
    int call_luafun_0(const char *fun_name, args_type... args)
    {
        int ret = 0;
        ret = ZCE_LUA::call_luafun(lua_state_, fun_name, 1, args...);
        if (ret != 0)
        {
            return ret;
        }
        return 0;
    }

    ///����LUA�ĺ�����ֻ��һ������ֵ
    template<typename ret_type1, typename... args_type>
    int call_luafun_1(const char *fun_name, ret_type1 &ret_val1, args_type... args)
    {
        int ret = 0;
        ret = ZCE_LUA::call_luafun(lua_state_, fun_name, 1, args...);
        if (ret != 0)
        {
            return ret;
        }
        //�ڶ�ջ��������ֵ
        ret_val1 = ZCE_LUA::read_stack<ret_type1>(lua_state_, -1);
        lua_pop(lua_state_, 2);
        return 0;
    }

    ///����LUA�ĺ���������������ֵ
    template<typename ret_type1, typename ret_type2, typename... args_type>
    int call_luafun_2(const char *fun_name, ret_type1 &ret_val1, ret_type2 &ret_val2, args_type... args)
    {
        int ret = 0;
        ret = ZCE_LUA::call_luafun(lua_state_, fun_name, 2, args...);
        if (ret != 0)
        {
            return ret;
        }
        //�ڶ�ջ��������ֵ
        ret_val1 = ZCE_LUA::read_stack<ret_type1>(lua_state_, -2);
        ret_val2 = ZCE_LUA::read_stack<ret_type2>(lua_state_, -1);
        lua_pop(lua_state_, 2);
        return 0;
    }

    ///����LUA�ĺ���������������ֵ,�ðɾ�֧�ֵ�3������ֵ�ѣ�ʵ��û��Ȥ��
    template<typename ret_type1, typename ret_type2, typename ret_type3, typename... args_type>
    int call_luafun_3(const char *fun_name, ret_type1 &ret_val1, ret_type2 &ret_val2, ret_type3 &ret_val3, args_type... args)
    {
        int ret = 0;
        ret = ZCE_LUA::call_luafun(lua_state_, fun_name, 3, args...);
        if (ret != 0)
        {
            return ret; 
        }
        //�ڶ�ջ��������ֵ,ע�����˳��
        ret_val1 = ZCE_LUA::read_stack<ret_type1>(lua_state_,-3);
        ret_val2 = ZCE_LUA::read_stack<ret_type2>(lua_state_,-2);
        ret_val3 = ZCE_LUA::read_stack<ret_type3>(lua_state_,-1);
        lua_pop(lua_state_, 3);
        return 0;
    }

    /*!
    * @brief      ����ĸ�Luaʹ�ã��������metatable�ı�����˵ԭ�͵ı�
    * @tparam     class_type
    * @return     Candy_Tie_Class ���ڷ������ĳ�Ա����������д������.�Ĳ���
    * @param      class_name
    * @param      read_only  �����������Ƿ�ֻ����������д
    * @note
    */
    template<typename class_type>
    Candy_Tie_Class<class_type> tie_class(const char *class_name,
                                          bool read_only = false)
    {
        //��T������,�������
        lua_pushstring(lua_state_, ZCE_LUA::class_name<class_type>::name(class_name));
        //new һ��table�����table����Ϊ���������metatable�ģ�ĳ�̶ֳ���Ҳ����˵��ԭ�ͣ���
        lua_newtable(lua_state_);

        //__name���Ǳ�׼��Ԫ������������������ʹ��
        lua_pushstring(lua_state_, "__name");
        lua_pushstring(lua_state_, ZCE_LUA::class_name<class_type>::name());
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

        Candy_Tie_Class<class_type> candy_tie(this, read_only);
        return candy_tie;
    }



    /*!
    * @brief
    * @tparam     class_type class_type ����
    * @tparam     construct_fun �ǹ��캯���ķ�װ��ZCE_LUA::constructor
    * @return     int
    * @param      func
    */
    template<typename class_type, typename construct_fun>
    int class_constructor(construct_fun func)
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


    /*!
    * @brief      ������Ѷ��ؼ̳�ʵ���ˣ�������������ò���
    * @tparam     class_type  ����
    * @tparam     parent_type ������
    * @return     int == 0 ����ֵ��ʶ�ɹ�
    */
    template<typename class_type, typename parent_type>
    int class_inherit()
    {
        //����������ƣ�ȡ�����metatable�ı�����˵ԭ�͡�
        lua_pushstring(lua_state_, ZCE_LUA::class_name<class_type>::name());
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);

        //���ջ����һ����
        if (!lua_istable(lua_state_, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] class name[%s] is not tie to lua.",
                ZCE_LUA::class_name<class_type>::name());
            ZCE_ASSERT(false);
            lua_pop(lua_state_, 1);
            return -1;
        }

        //����__parent Ϊ �������ƣ�Ŀǰ���ܶ��ؼ̳�,�Һ�freg���۹�����ʵʵ�ֶ��ؼ���Ҳ���ԣ�
        //�����ڴ����ѯ��ʱ������鷳

#if defined DEBUG || defined _DEBUG
        lua_pushstring(lua_state_, "__parent");
        lua_gettable(lua_state_, -2);
        if (lua_isnil(lua_state_, -1));
        {
            lua_remove(lua_state_, -1);
        }
        else
        {
            ZCE_ASSERT(false);
        }
#endif

        lua_pushstring(lua_state_, "__parent");
        lua_pushstring(lua_state_, ZCE_LUA::class_name<parent_type>::name());
        lua_gettable(lua_state_, LUA_GLOBALSINDEX);
        if (!lua_istable(lua_state_, -1))
        {
            ZCE_LOGMSG(RS_ERROR, "[LUATIE] class name[%s] is not tie to lua.",
                ZCE_LUA::class_name<parent_type>::name());
            ZCE_ASSERT(false);
            lua_pop(lua_state_, 1);
            return -1;
        }
        lua_rawset(lua_state_, -3);

        //�Ӷ�ջ����push_metaȡ�õ�vlue
        lua_pop(lua_state_, 1);
        return 0;
    }


    /*!
    * @brief      ��һ�����meta table �󶨳�Ա����
    * @tparam     class_type �󶨵��������
    * @tparam     var_type   ��Ա����
    * @return     int        �Ƿ�󶨳ɹ�
    * @param      name       �󶨵�����
    * @param      class_type::*val �󶨵ĳ�Ա������ָ��
    * @note       Luatinker�л���һ��ģ�����base_type����Ա�������࣬һ������Ϊclass_type
    *             ��base_type��һ����,����Ҳ���԰�һ������ĳ�Ա���������Lua��meta table���棬
    *             ���Լ��о��ⶫ���Ĵ��ڱ�Ҫ�Բ���ȥ����
    */
    template<typename class_type, typename var_type>
    int class_mem_var(const char *name, var_type class_type::*val)
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
        new(lua_newuserdata(lua_state_, sizeof(ZCE_LUA::member_var<class_type, var_type>)))  \
            ZCE_LUA::member_var<class_type, var_type>(val);
        lua_rawset(lua_state_, -3);

        lua_pop(lua_state_, 1);
        return 0;
    }


    ///��һ�����meta table �󶨳�Ա����
    template<typename class_type, typename ary_type, size_t ary_size>
    int class_mem_ary(const char *name,
        ary_type(class_type:: *mem_ary)[ary_size],
        bool read_only = false)
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
        new(lua_newuserdata(lua_state_, sizeof(ZCE_LUA::member_array<class_type, ary_type, ary_size>)))  \
            ZCE_LUA::member_array<class_type, ary_type, ary_size>(mem_ary, read_only);
        lua_rawset(lua_state_, -3);

        lua_pop(lua_state_, 1);
        return 0;
    }



    template<typename class_type, typename ret_type, typename... args_type>
    int class_mem_fun(const char *name, typename ret_type(class_type::*func)(args_type...))
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
        //�����ĺ���ָ����Ϊupvalue_�ġ�
        //ע���������ĳ�Աָ�루���ӽӽ�size_t����������ʵ�ʵ�ָ�룬�������������light userdata
        //�������д������Ҫ�����񣬷�Ҫ��typedef��תһ��
        typedef ret_type(class_type:: *mem_fun)(args_type...);
        new(lua_newuserdata(lua_state_, sizeof(mem_fun))) mem_fun(func);
        //
        lua_pushcclosure(lua_state_,
            ZCE_LUA::member_functor<class_type, ret_type, args_type...>::invoke, 1);
        lua_rawset(lua_state_, -3);

        lua_pop(lua_state_, 1);
        return 0;
    }


protected:

    //Lua�Ľ�������״̬
    lua_State   *lua_state_;
};

#if defined (ZCE_OS_WINDOWS)
#pragma warning ( pop )
#endif

#endif //#if defined  ZCE_USE_LUA && defined ZCE_SUPPORT_CPP11

#endif // ZCE_LIB_SCRIPT_LUA_H_

