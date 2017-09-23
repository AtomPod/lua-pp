#ifndef __LUA_META_OBJECT_H_
#define __LUA_META_OBJECT_H_
#include "ExtendClass.h"

namespace lua {
  class Context;
  class MetaObject {
  private:
    static const char *ObjectClass;
  public:
    static void registerObject(const Context &context);
    static ExtendClass extend(const Context &context , const char *name, const char *base = nullptr);
  };
} /* lua */

#endif
