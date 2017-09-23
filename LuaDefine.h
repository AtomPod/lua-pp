#ifndef __LUA_DEFINE_H_
#define __LUA_DEFINE_H_
extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

/* 启动原子计数
#ifndef __LUA_USE_ATOMIC_REF_
#define __LUA_USE_ATOMIC_REF_
#endif
*/




//struct lua_State;
namespace lua {
  typedef int LuaType;
  typedef lua_Integer ObjectHandle;
  typedef lua_State* StateType;
  typedef lua_State* StateHandle;
} /* lua */



#endif
