#ifndef __LUA_EXTNDS_CLASS_H_
#define __LUA_EXTNDS_CLASS_H_
#include <string>
#include "Closure.h"

namespace lua {
  class Context;
  class ExtendClass {
  public:
    typedef std::string String;
  private:
    /* data */
    String _className;
    const Context *context;
  public:
    ExtendClass ();
    ExtendClass (const Context &ctx , const String &_className);

    const ExtendClass &setMethod(const char *name , const Closure &closure) const;
    const ExtendClass &setInitializeMethod(const Closure &closure) const;
    const ExtendClass &setField(const char *name , const DeferableObject &object) const;
    const ExtendClass &setField(lua_Integer idx , const DeferableObject &object) const;
    const ExtendClass &setField(lua_Number idx , const DeferableObject &object) const;
    const ExtendClass &setFinalizers(const Closure &closure) const;
//尽量淘汰
    const ExtendClass *setMethod(const char *name , const Closure::ExtendMethod &method) const;
    const ExtendClass *setInitializeMethod(const Closure::ExtendMethod &method) const;

    const String &getClassName() const;

    explicit operator bool() const;
    virtual ~ExtendClass ();
  };

  inline ExtendClass::operator bool() const {
    return context != nullptr;
  }

  inline const ExtendClass::String &ExtendClass::getClassName() const {
    return _className;
  }
} /* lua */

#endif
