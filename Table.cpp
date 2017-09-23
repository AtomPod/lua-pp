#include "Table.h"
#include "handle.h"
#include "Context.h"
#include "DeferableObject.h"
#include <cassert>

namespace lua {

  Table::Table () {

  }

  Table::Table (
          const Handle &other
  ) : Handle(other) {

  }

  Table::Table (
          const Context &context
  ) : Handle(context) {

  }
  Table::Table (
          const Context &context  ,
          const ObjectMode &mode
  ) : Handle(context , mode) {

  }
  Table::Table (
          const Context &context  ,
          const ObjectHandle &handle
  ) : Handle(context , handle) {

  }
  Table::Table (
          const Context &context  ,
          const ObjectHandle &handle,
          const ObjectMode &mode
  ) : Handle(context , handle , mode) {

  }

  void Table::setField(const char *name , const DeferableObject &object) const {
    StateHandle lstate = getStateHandle();
    assert(lstate != nullptr);
    Handle::FixedObjectHandle ohandle(*this);
    bool push = object.pushToStack();
    assert(push);
    lua_setfield(lstate, ohandle, name);
  }

  void Table::setField(lua_Integer idx , const DeferableObject &object) const {

    StateHandle lstate = getStateHandle();
    assert(lstate != nullptr);

    Handle::FixedObjectHandle ohandle(*this);
    bool push = object.pushToStack();
    assert(push);
    lua_seti(lstate, ohandle , idx);
  }

  void Table::setField(const char *name , const Handle &field) const {
    StateHandle lstate = getStateHandle();
    assert(lstate != nullptr);

    Handle::FixedObjectHandle ohandle(*this);
    bool push = field.pushToStack();
    assert(push);
    lua_setfield(lstate, ohandle, name);
  }

  void Table::setField(lua_Integer idx , const Handle &field) const {
    StateHandle lstate = getStateHandle();
    assert(lstate != nullptr);

    Handle::FixedObjectHandle ohandle(*this);

    bool push = field.pushToStack();
    assert(push);
    lua_seti(lstate, ohandle, idx);
  }

  DeferableObject Table::getField(const char *name) const {
    const Context *ctx = getContext();
    StateHandle lstate = getStateHandle();

    assert(ctx != nullptr && lstate != nullptr);

    Handle::FixedObjectHandle ohandle(*this);
  
    int type = lua_getfield(lstate, ohandle, name);

    if (type == LUA_TNIL) {
      lua_pop(lstate, 1); //弹出nil
      return DeferableObject(*ctx); //返回nil值
    }

    return DeferableObject(Handle(*ctx , Handle::Persistent) ,
                    DeferableObject::HandleMode::None);
  }

  DeferableObject Table::getField(lua_Integer idx) const {
    const Context *ctx = getContext();
    StateHandle lstate = getStateHandle();

    assert(ctx != nullptr && lstate != nullptr);

    Handle::FixedObjectHandle ohandle(*this);
    int type = lua_geti(lstate, ohandle, idx);

    if (type == LUA_TNIL) {
      lua_pop(lstate, 1); //弹出nil
      return DeferableObject(*ctx); //返回nil值
    }

    return DeferableObject(Handle(*ctx , Handle::Persistent) ,
                    DeferableObject::HandleMode::None);
  }

  void Table::rawset(const char *name , const DeferableObject &object) const {
    const Context *ctx = getContext();
    StateHandle lstate = getStateHandle();
    assert(ctx != nullptr && lstate != nullptr && name != nullptr && object.valid());

    Handle::FixedObjectHandle ohandle(*this);

    lua_pushstring(lstate, name);
    bool push = object.pushToStack();
    assert(push);
    lua_rawset(lstate, ohandle);
  }

  void Table::rawset(lua_Integer idx , const DeferableObject &object) const {
    const Context *ctx = getContext();
    StateHandle lstate = getStateHandle();
    assert(ctx != nullptr && lstate != nullptr && object.valid());

    Handle::FixedObjectHandle ohandle(*this);

    bool push = object.pushToStack();
    assert(push);
    lua_rawseti(lstate, ohandle  , idx);
  }

  DeferableObject Table::rawget(const char *name) const {
    const Context *ctx = getContext();
    StateHandle lstate = getStateHandle();
    assert(ctx != nullptr && lstate != nullptr && name != nullptr);
    Handle::FixedObjectHandle ohandle(*this);
    lua_pushstring(lstate , name);
    int type = lua_rawget(lstate, ohandle);

    if (type == LUA_TNIL) {
      lua_pop(lstate , 1);
      return DeferableObject(*ctx);
    }

    return DeferableObject(Handle(*ctx , Handle::Persistent) ,
                          DeferableObject::HandleMode::None);
  }
  DeferableObject Table::rawget(lua_Integer idx) const {
    const Context *ctx = getContext();
    StateHandle lstate = getStateHandle();
    assert(ctx != nullptr && lstate != nullptr);
    Handle::FixedObjectHandle ohandle(*this);
    int type = lua_rawgeti(lstate, ohandle , idx);

    if (type == LUA_TNIL) {
      lua_pop(lstate , 1);
      return DeferableObject(*ctx);
    }

    return DeferableObject(Handle(*ctx , Handle::Persistent) ,
                          DeferableObject::HandleMode::None);
  }

  Table Table::getMetatable() const {
    const Context *ctx = getContext();
    StateHandle lstate = getStateHandle();
    assert(ctx != nullptr && lstate != nullptr);

    Handle::FixedObjectHandle ohandle(*this);
    int success = lua_getmetatable(lstate, ohandle);

    if (success == 0) {
      lua_pop(lstate, 1);
      return Table();
    }

    return Table(*ctx , Handle::Persistent);
  }

  Table::~Table() {

  }

  Table Table::New(const Context &ctx , const Handle::ObjectMode &mode) {
    lua_newtable(ctx.getStateHandle());
    return Table(ctx , mode);
  }
} /* lua */
