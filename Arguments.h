#ifndef __LUA_ARGUMENTS_H_
#define __LUA_ARGUMENTS_H_
#include "Table.h"

namespace lua {
  class Arguments {
  private:
    /* data */
    DeferableObject *objects;
    DeferableObject *upvalues;
    size_t nupvalue;
    size_t nobjects;
    Table table;
  public:
    Arguments (size_t nobj);
    Arguments (const Context &context , size_t nobj);
    Arguments (const Context &context , size_t nobj , size_t nupvalue);
    virtual ~Arguments ();

    /*
    未考虑Arguments拷贝问题，所以暂时为不可拷贝
     */
    Arguments (const Arguments &arguments) = delete;
    Arguments &operator=(const Arguments &) = delete;

  public:
    bool  hasSelf() const;
    const Table *self() const;

    DeferableObject &get(const size_t &n);
    const DeferableObject &get(const size_t &n) const;

    DeferableObject &getUpvalue(const size_t &n);
    const DeferableObject &getUpvalue(const size_t &n) const;


    DeferableObject &operator[](const size_t &n);
    const DeferableObject &operator[](const size_t &n) const;
    const size_t &size() const;
    const size_t &upvalueSize() const;
  };

  inline bool Arguments::hasSelf() const {
    return table.valid() && !table.nil();
  }

  inline const Table *Arguments::self() const {
    return table.valid() ? &table : nullptr;
  }

  inline DeferableObject &Arguments::getUpvalue(const size_t &n) {
    return upvalues[n];
  }

  inline const DeferableObject &Arguments::getUpvalue(const size_t &n) const {
    return upvalues[n];
  }

  inline DeferableObject &Arguments::get(const size_t &n) {
    return objects[n];
  }

  inline DeferableObject &Arguments::operator[](const size_t &n) {
    return get(n);
  }

  inline const DeferableObject &Arguments::get(const size_t &n) const {
    return objects[n];
  }

  inline const DeferableObject &Arguments::operator[](const size_t &n) const {
    return get(n);
  }

  inline const size_t &Arguments::size() const {
    return nobjects;
  }

  inline const size_t &Arguments::upvalueSize() const {
    return nupvalue;
  }
} /* lua */

#endif
