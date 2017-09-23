#ifndef __LUA_DEFERABLE_OBJECT_H_
#define __LUA_DEFERABLE_OBJECT_H_
#include "Handle.h"
#include <cstdint>

namespace lua {
  class Table;
  class DeferableObject {
  public:
    enum HandleMode {
      None,
      Argument
    };

    enum ObjectType {
      TBoolean,
      TInteger,
      TNumber,
      TCString,
      TUserData,
      TGCUserData,
      TTable,
      TFunction,
      TNil
    };
  private:
    /* data */
    enum ValueMode {
      Neither = 0x00,
      ValueOnly = 0x01,
      HandleOnly = 0x02,
      Both = 0x03
    } mode;

    ObjectType otype;

    struct String{
      const char *pString;
      Handle::Counter refcount;
    };

    union CObject {
      bool bObject;
      lua_Integer iObject;
      lua_Number  nObject;
      void *pObject;
      String *sObject;
    } object;

    Handle handle;
  protected:
    void PretreatmentHandle(Handle &handle , const HandleMode &mode);
    void PrejudgeObject(const Handle &handle);

    bool pushObjectToStack() const; //未实现
    bool pushHandleToStack() const; //未实现
    void maybeRefString(CObject &object , const ObjectType &type);
    void maybeDerefString(CObject &object , const ObjectType &type);
  public:

    /*
      实现各种普通类型转换的构造函数
     */
    DeferableObject ();
    DeferableObject (const DeferableObject &);
    DeferableObject (const Context &context);
    DeferableObject (const Context &context , bool value);
    DeferableObject (const Context &context , float value);
    DeferableObject (const Context &context , double value);
    DeferableObject (const Context &context , int8_t value);
    DeferableObject (const Context &context , uint8_t value);
    DeferableObject (const Context &context , int16_t value);
    DeferableObject (const Context &context , uint16_t value);
    DeferableObject (const Context &context , int32_t value);
    DeferableObject (const Context &context , uint32_t value);
    DeferableObject (const Context &context , int64_t value);
    DeferableObject (const Context &context , uint64_t value);
    DeferableObject (const Context &context , const char *value);
    DeferableObject (const Context &context , void *p);
    DeferableObject (const Handle &handle , const HandleMode &mode);
    ~DeferableObject ();

    DeferableObject &operator=(DeferableObject other);

    bool pushToStack() const;
    const Context &getContext() const;

    bool canConvertToBoolean() const;
    bool canConvertToInteger() const;
    bool canConvertToNumber() const;
    bool canConvertToUserData() const;


    bool canConvertToTable() const;
    bool canConvertToClosure() const;
    bool canConvertToString() const;

    bool toBoolean() const;
    lua_Integer toInteger() const;
    lua_Number  toNumber() const;
    void* toUserData() const;

    const Handle &toTable() const;
    const Handle &toClosure() const;
    const char* toString() const;

    template <typename T>
    T *toPointer() const;

    ObjectType type() const;
    void swap(DeferableObject &other);
    bool  valid() const;
    bool  nil() const;
  };

  template <typename T>
  inline T *DeferableObject::toPointer() const {
    return reinterpret_cast<T*>(toUserData());
  }

  inline bool DeferableObject::valid() const {
    return mode != Neither;
  }

  inline bool DeferableObject::nil() const {
    return otype == ObjectType::TNil;
  }

  inline DeferableObject::ObjectType DeferableObject::type() const {
    return otype;
  }
} /* lua */

#endif
