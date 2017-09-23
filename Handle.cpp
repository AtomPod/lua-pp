#include "Handle.h"
#include "Context.h"
#include <algorithm>
#include <cassert>

namespace lua {

  Handle::FixedObjectHandle
  ::FixedObjectHandle(const Handle &rhandle) : PopStack(true) ,
                                                fixedHandle(0) ,
                                               handle(rhandle) {
    if (rhandle.refcount != nullptr) {
      rhandle.pushToStack();
      fixedHandle = mappedToAbsoluteObject(rhandle.getStateHandle(), -1);
    } else {
      fixedHandle = rhandle.getObjectHandle();
    }
  }

  Handle::FixedObjectHandle
  ::FixedObjectHandle(const Handle &rhandle , bool popStack) : PopStack(popStack) ,
                                                fixedHandle(0) ,
                                               handle(rhandle) {
    if (rhandle.refcount != nullptr) {
      rhandle.pushToStack();
      fixedHandle = mappedToAbsoluteObject(rhandle.getStateHandle(), -1);
    } else {
      fixedHandle = rhandle.getObjectHandle();
    }
  }

  Handle::FixedObjectHandle::operator ObjectHandle() const {
    return fixedHandle;
  }

  Handle::FixedObjectHandle::~FixedObjectHandle() {
    if (handle.refcount == nullptr || !PopStack)
      return;
    lua_pop(handle.getStateHandle(), 1);
  }

  Handle::Handle() :
                    context(nullptr) ,
                    object(0)       ,
                    refcount(nullptr) {

  }

  Handle::Handle (
          const Handle &other
  ) : context(other.context) ,
      object(other.object) ,
      refcount(other.refcount) {
        ref();
  }

  Handle::Handle (
          const Context &_context
  ) : context(&_context) ,
      object(0)    ,
      refcount(nullptr) {

  }

  Handle::Handle (
          const Context &_context  ,
          const ObjectMode &mode
  ) : context(&_context) ,
      object(modeIsLocal(mode) ?
              mappedToAbsoluteObject(_context.getStateHandle() , -1) : LUA_REFNIL) ,
      refcount(modeIsLocal(mode) ?
              nullptr : new Counter(1) ) {
        if (!modeIsLocal(mode)) {
          object = luaL_ref(_context.getStateHandle(), getRefTableHandle());
        }
  }

  Handle::Handle (
          const Context &_context  ,
          const ObjectHandle &handle
  ) : context(&_context) ,
      object(lua_absindex(_context.getStateHandle() , handle)) ,
      refcount(nullptr) {

  }

  Handle::Handle (
          const Context &_context  ,
          const ObjectHandle &handle,
          const ObjectMode &mode
  ) : context(&_context) ,
      object(modeIsLocal(mode) ? lua_absindex(_context.getStateHandle() , handle) : LUA_REFNIL) ,
      refcount(modeIsLocal(mode) ? nullptr : new Counter(1) ) {

        if (!modeIsLocal(mode)) {
          lua_pushvalue(_context.getStateHandle(), handle);
          object = luaL_ref(_context.getStateHandle(), getRefTableHandle());
        }
  }


  bool Handle::pushToStack() const {
    if (!valid())
      return false;

    if (nil()) {
      lua_pushnil(getStateHandle());
      return true;
    }

    if (refcount != nullptr) {
      lua_rawgeti(getStateHandle(), getRefTableHandle(), getObjectHandle());
      return true;
    }
    lua_pushvalue(getStateHandle(), getObjectHandle());
    return true;
  }

  bool Handle::deref() const {
    if (refcount == nullptr)
      return true;

    if (*refcount == 1) {
      luaL_unref(getStateHandle(), getRefTableHandle(), getObjectHandle());
      delete refcount;
      return true;
    }

    --(*refcount);
    assert(*refcount != 0);

    return false;
  }

  Handle::~Handle() {
    deref();
  }

  void Handle::swap(Handle &other) {
    std::swap(context, other.context);
    std::swap(object , other.object);
    std::swap(refcount, other.refcount);
  }

  StateHandle Handle::getStateHandle() const {
    return context == nullptr ? nullptr : context->getStateHandle();
  }
} /* lua */
