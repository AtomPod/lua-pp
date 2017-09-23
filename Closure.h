#ifndef __LUA_CLOSURE_H_
#define __LUA_CLOSURE_H_
#include "Arguments.h"

namespace lua {
  class Closure : public Handle {
  public:
    typedef DeferableObject (*ExtendMethod)(const Context &context , const Arguments &);
    static int ExtendMethodAdapter(StateHandle state);
  public:
    
    Closure ();

    Closure (
            const Handle &other
    );

    Closure (
            const Context &context
    );
    Closure (
            const Context &context  ,
            const ObjectMode &mode
    );
    Closure (
            const Context &context  ,
            const ObjectHandle &handle
    );
    Closure (
            const Context &context  ,
            const ObjectHandle &handle,
            const ObjectMode &mode
    );

    DeferableObject invoke(const Arguments *args) const;
    virtual ~Closure ();
  public:
    static Closure New(const Context &ctx , const ExtendMethod &method , const Arguments *args);
  };
} /* lua */

#endif
