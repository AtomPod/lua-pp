#ifndef __LUA_CONTEXT_H_
#define __LUA_CONTEXT_H_
#include "LuaDefine.h"
#include "Table.h"

namespace lua {
  class Context {
  public:
    typedef lua_Alloc Allocator;

    enum GCMode {
      Stop = LUA_GCSTOP,
      Restart = LUA_GCRESTART,
      Collect = LUA_GCCOLLECT,
      Count = LUA_GCCOUNT,
      CountByte = LUA_GCCOUNTB,
      Step = LUA_GCSTEP,
      IsRunning = LUA_GCISRUNNING,
      SetPause = LUA_GCSETPAUSE,
      SetStepmul = LUA_GCSETSTEPMUL
    };

    enum ThreadState {
      Running = LUA_OK,
      Yield = LUA_YIELD
    };
  private:
    /* data */
    StateHandle lstate;
  public:
    static Context &getInstance();
  public:
    Context ();
    Context (const Allocator &alloc , void *userdata);
    Context (const Context &) = delete;
    Context &operator=(const Context &) = delete;

    void setAllocator(const Allocator &alloc , void *userdata) const;
    Allocator getAllocator(void *&userdata) const;

    void OpenStandardLibrary() const;

    int RunScript(const char *script) const;
    int RunScriptFile(const char *file) const;
    void   moveStackTo(const Context &dest , int nStack) const;

    DeferableObject get(const char *name , const Handle::ObjectMode &mode) const;
    void   set(const char *name , const DeferableObject &handle) const;

    int   getUsedMemory() const;
    int   GC(const GCMode &mode , int data) const;
    bool  Valid() const;

    int   getLocalStackSize() const;

    ThreadState getThreadState() const;
    const StateHandle &getStateHandle() const;
    virtual ~Context ();
  };

  inline bool Context::Valid() const {
    return lstate != nullptr;
  }

  inline const StateHandle &Context::getStateHandle() const {
    return lstate;
  }
} /* lua */

#endif
