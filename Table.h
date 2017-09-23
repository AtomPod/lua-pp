#ifndef __LUA_TABLE_H_
#define __LUA_TABLE_H_

#include <cstdint>
#include <memory>
#include "DeferableObject.h"

namespace lua {
  class Table : public Handle {
  public:

    Table ();

    Table (
            const Handle &other
    );

    Table (
            const Context &context
    );
    Table (
            const Context &context  ,
            const ObjectMode &mode
    );
    Table (
            const Context &context  ,
            const ObjectHandle &handle
    );
    Table (
            const Context &context  ,
            const ObjectHandle &handle,
            const ObjectMode &mode
    );

    void setField(const char *name , const DeferableObject &object) const;
    void setField(const char *name , const Handle &field) const;
    void setField(lua_Integer idx , const DeferableObject &object) const;
    void setField(lua_Integer idx , const Handle &field) const;

    DeferableObject getField(const char *name) const;
    DeferableObject getField(lua_Integer idx) const;

    void rawset(const char *name , const DeferableObject &object) const;
    void rawset(lua_Integer idx , const DeferableObject &object) const;

    DeferableObject rawget(const char *name) const;
    DeferableObject rawget(lua_Integer idx) const;

    Table getMetatable() const;

    static Table New(const Context &ctx ,
                    const Handle::ObjectMode &mode);

    virtual ~Table ();
  };

} /* lua */
#endif
