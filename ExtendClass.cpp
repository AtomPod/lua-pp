#include "ExtendClass.h"
#include "Context.h"
#include <cassert>

namespace lua {

  ExtendClass::ExtendClass() : context(nullptr) {

  }

  ExtendClass::ExtendClass(const Context &ctx ,
                          const String &className) : _className(className) ,
                          context(&ctx) {

  }

  ExtendClass::~ExtendClass() {

  }

  const ExtendClass &ExtendClass::setField(const char *name , const DeferableObject &object) const {
    assert(context != nullptr && name != nullptr && object.valid());

    StateHandle lstate = context->getStateHandle();
    assert(lstate != nullptr);

    int type = lua_getglobal(lstate, _className.c_str());

    if (type != LUA_TTABLE) {
      lua_pop(lstate, 1); //nil
      assert(false);
      return *this;
    }

    lua_pushstring(lstate, name);
    if (!object.pushToStack()) {
      lua_pop(lstate, 2); //table , name
      assert(false);
      return *this;
    }
    lua_rawset(lstate, -3);
    lua_pop(lstate, 1);

    return *this;
  }

  const ExtendClass &ExtendClass::setField(lua_Integer idx , const DeferableObject &object) const {
    assert(context != nullptr && object.valid());

    StateHandle lstate = context->getStateHandle();
    assert(lstate != nullptr);

    int type = lua_getglobal(lstate, _className.c_str());

    if (type != LUA_TTABLE) {
      lua_pop(lstate, 1); //nil
      assert(false);
      return *this;
    }

    if (!object.pushToStack()) {
      lua_pop(lstate , 1); //table;
      assert(false);
      return *this;
    }
    lua_rawseti(lstate, -2, idx);
    lua_pop(lstate, 1);
    return *this;
  }

  const ExtendClass &ExtendClass::setField(lua_Number idx , const DeferableObject &object) const {
    assert(context != nullptr && object.valid());

    StateHandle lstate = context->getStateHandle();
    assert(lstate != nullptr);

    int type = lua_getglobal(lstate, _className.c_str());

    if (type != LUA_TTABLE) {
      lua_pop(lstate, 1); //nil
      assert(false);
      return *this;
    }

    lua_pushnumber(lstate, idx);
    if (!object.pushToStack()) {
      lua_pop(lstate, 2); //table，idx
      assert(false);
      return *this;
    }
    lua_rawset(lstate, -3);
    lua_pop(lstate, 1);

    return *this;
  }


  const ExtendClass &ExtendClass::setMethod(const char *name ,
                                            const Closure &closure) const {
    assert(context != nullptr && name != nullptr && closure.valid() && !closure.nil());

    StateHandle _state = context->getStateHandle();
    int type = lua_getglobal(_state, _className.c_str());

    if (type != LUA_TTABLE) {
      lua_pop(_state, 1); //nil
      assert(false);
      return *this;
    }

    lua_pushstring(_state , name);
    if (!closure.pushToStack()) {
      lua_pop(_state, 2); //table，与name
      assert(false);
      return *this;
    }
    lua_rawset(_state, -3);
    lua_pop(_state, 1);
    return *this;
  }
  const ExtendClass &ExtendClass::setInitializeMethod(
                                            const Closure &closure) const {
    return setMethod("initialize", closure);
  }

  const ExtendClass &ExtendClass::setFinalizers(const Closure &closure) const {
    assert(context != nullptr && closure.valid() && !closure.nil());
    StateHandle lstate = context->getStateHandle();
    assert(lstate != nullptr);
    int type = lua_getglobal(lstate, _className.c_str());

    if (type != LUA_TTABLE) {
      lua_pop(lstate, 1);
      assert(false);
      return *this;
    }

    lua_pushstring(lstate, "meta");
    type = lua_rawget(lstate, -2);

    if (type != LUA_TTABLE) {
      lua_pop(lstate, 2); //弹出nil与table
      assert(false);
      return *this;
    }

    lua_pushstring(lstate, "__gc");
    if (!closure.pushToStack()) {
      lua_pop(lstate, 3); //弹出类表，meta表，与__gc字段
      assert(false);
      return *this;
    }

    lua_rawset(lstate, -3);
    lua_pop(lstate, 2); //弹出类表，meta表
    return *this;
  }


  const ExtendClass *ExtendClass::setMethod(const char *name,
                          const Closure::ExtendMethod &method) const {
    assert(context != nullptr && name != nullptr && method != nullptr);

    StateHandle _state = context->getStateHandle();
    int type = lua_getglobal(_state, _className.c_str());

    if (type != LUA_TTABLE) {
      lua_pop(_state, 1);
      assert(false);
      return this;
    }

    lua_pushstring(_state , name);
    lua_pushlightuserdata(_state, const_cast<Context*>(context));
    lua_pushlightuserdata(_state, reinterpret_cast<void*>(method));
    lua_pushcclosure(_state, Closure::ExtendMethodAdapter , 2);
    lua_rawset(_state, -3);
    lua_pop(_state, 1);
    return this;
  }

  const ExtendClass *ExtendClass::setInitializeMethod(const Closure::ExtendMethod &method)
  const {
    setMethod("initialize", method);
    return this;
  }
} /* lua */
