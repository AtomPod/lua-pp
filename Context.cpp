#include "Context.h"
#include <cassert>

namespace lua {
  Context::Context() :
                      lstate(luaL_newstate()) {

  }

  Context::Context(const Allocator &alloc , void *userdata) :
                      lstate(lua_newstate(alloc, userdata)) {

  }

  Context &Context::getInstance() {
    static Context instance;
    return instance;
  }

  void Context::setAllocator(const Allocator &alloc, void *userdata) const {
    assert (Valid());
    lua_setallocf(getStateHandle(), alloc, userdata);
  }

  Context::Allocator Context::getAllocator(void *&userdata) const {
    assert (Valid());
    return lua_getallocf(getStateHandle(), &userdata);
  }

  void Context::OpenStandardLibrary() const {
    assert (Valid());

    luaL_openlibs(getStateHandle());
  }

  int Context::RunScript(const char *script) const {
    assert (Valid() && script != nullptr);
    StateHandle shandle = getStateHandle();
    int ret = luaL_loadstring(shandle, script);

    if (ret == 0) {
      ret = lua_pcall(shandle , 0 , 0 , 0);
    }
    return ret;
  }

  int Context::RunScriptFile(const char *file) const {
    assert (Valid() && file != nullptr);
    StateHandle shandle = getStateHandle();
    int ret = luaL_loadfile(shandle, file);

    if (ret == 0) {
      ret = lua_pcall(shandle, 0, 0, 0);
    }
    return ret;
  }

  DeferableObject Context::get(const char *name , const Handle::ObjectMode &mode) const {
    assert (Valid() && name != nullptr);

    lua_getglobal(getStateHandle(), name);
    return DeferableObject(Handle(*this, mode) ,
                          DeferableObject::HandleMode::None);
  }

  void Context::set(const char *name, const DeferableObject &handle) const {
    assert(Valid() && name != nullptr);

    handle.pushToStack();
    lua_setglobal(getStateHandle(), name);
  }

  void Context::moveStackTo(const Context &dest, int nStack) const {
    assert(Valid() && dest.Valid());
    lua_xmove(getStateHandle() , dest.getStateHandle() , nStack);
  }

  int Context::getUsedMemory() const {
    return GC(GCMode::Count , 0) * 1024 + GC(GCMode::CountByte , 0);
  }

  int Context::GC(const GCMode &mode, int data) const {
    assert(Valid());
    return lua_gc(getStateHandle(), mode, data);
  }

  Context::ThreadState Context::getThreadState() const {
    assert(Valid());
    return static_cast<Context::ThreadState>(lua_status(getStateHandle()));
  }

  int Context::getLocalStackSize() const {
    assert(Valid());
    return lua_gettop(getStateHandle());
  }

  Context::~Context() {
    if (lstate != nullptr) {
      lua_close(lstate);
      lstate = nullptr;
    }
  }
} /* lua */
