#include "Closure.h"
#include "Context.h"
#include <cassert>

namespace lua {

  Closure::Closure () {

  }

  Closure::Closure (
          const Handle &other
  ) : Handle(other) {

  }

  Closure::Closure (
          const Context &context
  ) : Handle(context) {

  }
  Closure::Closure (
          const Context &context  ,
          const ObjectMode &mode
  ) : Handle(context , mode) {

  }
  Closure::Closure (
          const Context &context  ,
          const ObjectHandle &handle
  ) : Handle(context , handle) {

  }
  Closure::Closure (
          const Context &context  ,
          const ObjectHandle &handle,
          const ObjectMode &mode
  ) : Handle(context , handle , mode) {

  }

  DeferableObject Closure::invoke(const Arguments *args) const {
    const Context *context = getContext();
    StateHandle lstate = getStateHandle();

    assert(context != nullptr && lstate != nullptr && valid() && !nil());

    FixedObjectHandle ohandle(*this , false);

    int nargs = 0;
    if (args != nullptr) {
      nargs = args->size();
      const Arguments &rargs = *args;
      for (size_t idx = 0; idx != nargs; ++idx) {
        bool push = rargs[idx].pushToStack();
        assert(push);
      }
    }

    lua_pcall(lstate, nargs, 1, 0);
    return DeferableObject(Handle(*context , Handle::ObjectMode::Persistent)
                                , DeferableObject::HandleMode::None);
  }

  Closure::~Closure() {

  }

  int Closure::ExtendMethodAdapter(StateHandle state) {
    int contextIndex = lua_upvalueindex(1);
    int methodIndex = lua_upvalueindex(2);
    int nargsIndex = lua_upvalueindex(3);

    ExtendMethod method = reinterpret_cast<ExtendMethod>(lua_touserdata(state, methodIndex));
    const Context *pContext = reinterpret_cast<const Context*>(lua_touserdata(state, contextIndex));
    lua_Integer nargs = lua_tointeger(state, nargsIndex);

    assert(method != nullptr && pContext != nullptr);

    Arguments args(*pContext , lua_gettop(state) , nargs);
    DeferableObject result = method(*pContext , args);
    return result.pushToStack() ? 1 : 0;
  }

  Closure Closure::New(const Context &ctx , const ExtendMethod &method, const Arguments *pargs) {

    StateHandle lstate = ctx.getStateHandle();
    assert(lstate != nullptr);

    lua_pushlightuserdata(lstate, const_cast<Context*>(&ctx));
    lua_pushlightuserdata(lstate, reinterpret_cast<void*>(method));

    int nargs = pargs != nullptr ? pargs->size() : 0;

    lua_pushinteger(lstate, nargs);

    for (size_t idx = 0; idx != nargs; idx++) {
      bool push = pargs->get(idx).pushToStack();
      assert(push);
    }

    lua_pushcclosure(lstate , ExtendMethodAdapter , nargs + 3);
    return Closure(ctx, Handle::Persistent);
  }
} /* lua */
