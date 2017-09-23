#ifndef __LUA_HANDLE_H_
#define __LUA_HANDLE_H_
#include "LuaDefine.h"
#ifdef __LUA_USE_ATOMIC_REF_
#include <atomic>
#endif

namespace lua {
  class Context;
  class Handle {
  public:
    friend class FixedObjectHandle;
    class FixedObjectHandle {
    private:
      /* data */
      bool PopStack;
      ObjectHandle fixedHandle;
      const Handle &handle;
    public:
      FixedObjectHandle (const Handle &rhandle);
      FixedObjectHandle (const Handle &rhandle , bool PopStack);
      operator ObjectHandle() const;
      ~FixedObjectHandle ();
    };
  public:
    #ifdef __LUA_USE_ATOMIC_REF_
      typedef std::atomic_int Counter;
    #else
      typedef int Counter;
    #endif

    enum ObjectMode {
      Local ,
      Persistent
    };

  public:
    Handle ();

    Handle (
            const Handle &other
    );

    Handle (
            const Context &context
    );
    Handle (
            const Context &context  ,
            const ObjectMode &mode
    );
    Handle (
            const Context &context  ,
            const ObjectHandle &handle
    );
    Handle (
            const Context &context  ,
            const ObjectHandle &handle,
            const ObjectMode &mode
    );

    Handle &operator=(Handle other);

          StateHandle   getStateHandle()        const;
    const ObjectHandle& getObjectHandle()       const;
    const Context*      getContext()            const;
          ObjectHandle  getRefTableHandle()     const;
          ObjectHandle  getRefNilHandle()       const;
                  bool  valid()                 const;
                  bool  nil()                   const;
                  bool  isPersistent()          const;
                  bool  pushToStack()           const;
                  void  popFromStack(int n = 1) const;
                  void  swap(Handle &other);
    virtual ~Handle ();
  public:
    static        bool  modeIsLocal(const ObjectMode &mode);
    static  ObjectHandle  mappedToAbsoluteObject(const StateHandle &state ,
                                                const ObjectHandle &object);
  protected:
    void    ref() const;
    bool    deref() const;
    void    setContext(const Context &);
  private:
    const Context*              context;
    ObjectHandle          object;
    mutable Counter*      refcount;
  };

  inline void Handle::setContext(const Context &ctx) {
    context = &ctx;
  }

  inline Handle &Handle::operator=(Handle other) {
    swap(other);
    return *this;
  }

  inline const ObjectHandle&Handle::getObjectHandle()   const {
    return  object;
  }

  inline const Context* Handle::getContext() const {
    return context;
  }

  inline      ObjectHandle  Handle::getRefTableHandle() const {
    return  LUA_REGISTRYINDEX;
  }

  inline      ObjectHandle  Handle::getRefNilHandle()   const {
    return  LUA_REFNIL;
  }

  inline bool Handle::valid() const {
    return  (context != nullptr);
  }

  inline bool Handle::nil() const {
    return (refcount == nullptr && object == 0)
        || (refcount != nullptr && object == LUA_REFNIL);
  }

  inline bool Handle::isPersistent() const {
    return  refcount != nullptr;
  }

  inline  void Handle::popFromStack(int n) const {
    lua_pop(getStateHandle(), n);
  }

  inline bool Handle::modeIsLocal(const Handle::ObjectMode &mode) {
    return mode == ObjectMode::Local;
  }

  inline void Handle::ref() const {
    if (refcount != nullptr) {
      ++(*refcount);
    }
  }

  inline ObjectHandle  Handle::mappedToAbsoluteObject(
                      const StateHandle &state ,
                      const ObjectHandle &handle) {
      return lua_absindex(state, handle);
  }
} /* lua */

#endif
