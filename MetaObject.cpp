#include "MetaObject.h"
#include "Context.h"
namespace lua {
  const char *MetaObject::ObjectClass = "Object = {}\n"
          "Object.meta = { __index = Object }\n"
          "function Object:create()\n"
              "local meta = rawget(self , \"meta\")\n"
              "if not meta then error(\"Cannot inherit from instance object\") end\n"
              "return setmetatable({} , meta)\n"
          "end\n"
          "function Object:new(...)\n"
              "local obj = self:create()\n"
              "if type(obj.initialize) == 'function' then\n"
                  "obj:initialize(...)\n"
              "end\n"
              "return obj\n"
          "end\n"
          "function Object:extend()\n"
              "local obj = self:create()\n"
              "local meta = {}\n"
              "for k , v in pairs(self.meta) do\n"
                  "meta[k] = v\n"
              "end\n"
              "meta.__index = obj\n"
              "meta.super = self\n"
              "obj.meta = meta\n"
              "return obj\n"
          "end\n";

    void MetaObject::registerObject(const Context &ctx) {
        luaL_dostring(ctx.getStateHandle(), MetaObject::ObjectClass);
    }

    ExtendClass MetaObject::extend(const Context &context ,
                                  const char *name,
                                  const char *base) {
      StateHandle state = context.getStateHandle();

      if (base == nullptr) {
        base = "Object";
      }

      lua_getglobal(state, base);

      if (!lua_istable(state, -1)) {
        lua_pop(state, 1); //弹出非table值
        return ExtendClass();
      }

      lua_getfield(state, -1, "extend");

      if (!lua_isfunction(state, -1)) {
        lua_pop(state, 2); //弹出table与非函数值
        return ExtendClass();
      }

      lua_pushvalue(state, -2); //将当前父类压入作为参数
      lua_pcall(state, 1, 1, 0); //运行函数，获取新类

      if (lua_istable(state, -1)) { //如果不是table，可能出现被修改等错误
        lua_setglobal(state, name); //将扩展后的类作为全局对象
        lua_pop(state, 1); //弹出父类
        return ExtendClass(context , name);
      } else {
        lua_pop(state, 2); //出错，弹出返回值和父类
        return ExtendClass();
      }

      return ExtendClass();
    }
} /* lua */
