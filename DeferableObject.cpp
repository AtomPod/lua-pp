#include "DeferableObject.h"
#include <cstring>
#include <malloc.h>
#include <cfloat>
#include <algorithm>
#include <cassert>

namespace lua {

  DeferableObject::DeferableObject() : mode(ValueMode::Neither) ,
                                      otype(ObjectType::TNil) {

  }

  DeferableObject::DeferableObject(const DeferableObject &other) :
                                        mode(other.mode),
                                        otype(other.otype),
                                        object(other.object),
                                        handle(other.handle) {
      maybeRefString(object, otype);
  }


  DeferableObject::DeferableObject(const Context &context):
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TNil) ,
                        handle(context)
                         {

  }

  DeferableObject::DeferableObject(const Context &context , bool value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TBoolean)  ,
                        handle(context){
        object.bObject = value;
  }

  DeferableObject::DeferableObject(const Context &context , float value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TNumber)  ,
                        handle(context){
        object.nObject = value;
  }

  DeferableObject::DeferableObject(const Context &context , double value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TNumber)  ,
                        handle(context){
        object.nObject = value;
  }
  DeferableObject::DeferableObject(const Context &context , int8_t value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TInteger)  ,
                        handle(context){
        object.iObject = value;
  }
  DeferableObject::DeferableObject(const Context &context , uint8_t value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TInteger)  ,
                        handle(context){
        object.iObject = value;
  }
  DeferableObject::DeferableObject(const Context &context , int16_t value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TInteger)  ,
                        handle(context){
        object.iObject = value;
  }
  DeferableObject::DeferableObject(const Context &context , uint16_t value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TInteger)  ,
                        handle(context){
        object.iObject = value;
  }
  DeferableObject::DeferableObject(const Context &context , int32_t value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TInteger)  ,
                        handle(context){
        object.iObject = value;
  }
  DeferableObject::DeferableObject(const Context &context , uint32_t value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TInteger)  ,
                        handle(context){
        object.iObject = value;
  }
  DeferableObject::DeferableObject(const Context &context , int64_t value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TInteger)  ,
                        handle(context){
        object.iObject = value;
  }
  DeferableObject::DeferableObject(const Context &context , uint64_t value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TInteger)  ,
                        handle(context){
        object.iObject = value;
  }

  DeferableObject::DeferableObject (const Context &context , const char *value) :
                        mode(ValueMode::ValueOnly) ,
                        otype(ObjectType::TCString)  ,
                        handle(context) {
        String *&sString = object.sObject;
        sString = new String();

        sString->pString = strdup(value);
        sString->refcount = 1;
  }

  DeferableObject::DeferableObject(const Context &context ,
                                    void *p) : mode(ValueMode::ValueOnly),
                                              otype(ObjectType::TUserData) ,
                                              handle(context) {
    object.pObject = p;
  }

  DeferableObject::DeferableObject(const Handle &_handle
                                , const HandleMode &mode) :
                                mode(ValueMode::Neither)
                              , otype(ObjectType::TNil)
                              , handle(_handle) {
      PrejudgeObject(handle);
      PretreatmentHandle(handle, mode);
  }

  DeferableObject::~DeferableObject() {
    maybeDerefString(object, otype);
  }

  DeferableObject &DeferableObject::operator=(DeferableObject other) {
    swap(other);
    return *this;
  }

  void DeferableObject::PrejudgeObject(const Handle &handle) {
    StateHandle lstate = handle.getStateHandle();
    assert(lstate != nullptr);

    int idx = 0;
    bool persistent = handle.isPersistent();
    if (persistent) {
      handle.pushToStack();
      idx = lua_absindex(lstate, -1);
    } else {
      idx = handle.getObjectHandle();
    }

    int _type = lua_type(lstate, idx);
    switch (_type) { //使用预读
      case LUA_TBOOLEAN:
        object.bObject = lua_toboolean(lstate, idx);
        mode = ValueMode::ValueOnly;
        otype = ObjectType::TBoolean;
        break;
      case LUA_TLIGHTUSERDATA:
        object.pObject = lua_touserdata(lstate, idx);
        mode = ValueMode::ValueOnly;
        otype = ObjectType::TUserData;
        break;
      case LUA_TNUMBER:
        if (lua_isinteger(lstate, idx)) {
          object.iObject = lua_tointeger(lstate, idx);
          otype = ObjectType::TInteger;
        } else {
          object.nObject = lua_tonumber(lstate, idx);
          otype = ObjectType::TNumber;
        }
        mode = ValueMode::ValueOnly;
        break;
      case LUA_TTABLE:
        otype = ObjectType::TTable;
        break;
      case LUA_TUSERDATA:
        otype = ObjectType::TGCUserData;
        break;
      case LUA_TSTRING: {
        const char *lString = lua_tostring(lstate, idx);
        String *&sString = object.sObject;
        sString = new String();
        sString->pString = persistent ? lString : (strdup(lString));
        sString->refcount = 1;
        otype = ObjectType::TCString;
        mode = ValueMode::ValueOnly;
      }
      break;
      case LUA_TFUNCTION:
        otype = ObjectType::TFunction;
        break;
      default:
        break;
    }
    if (persistent) {
      handle.popFromStack(1);
    }
  }

  void DeferableObject::PretreatmentHandle(Handle &handle
                                          , const HandleMode &hmode) {
      if (handle.isPersistent() || hmode == HandleMode::Argument) {
        mode = (mode == ValueMode::ValueOnly) ?
                ValueMode::Both : ValueMode::HandleOnly;
      }
  }

  bool DeferableObject::pushObjectToStack() const {
    StateHandle lstate = handle.getStateHandle();
    assert(lstate != nullptr);

    switch (otype) {
      case ObjectType::TNil:
        lua_pushnil(lstate);
        break;
      case ObjectType::TBoolean:
        lua_pushboolean(lstate, object.bObject);
        break;
      case ObjectType::TInteger:
        lua_pushinteger(lstate, object.iObject);
        break;
      case ObjectType::TNumber:
        lua_pushnumber(lstate, object.nObject);
        break;
      case ObjectType::TUserData:
        lua_pushlightuserdata(lstate, object.pObject);
        break;
      case ObjectType::TCString:
        lua_pushstring(lstate, object.sObject->pString);
        break;
      default:
        return false;
    }
    return true;
  }

  bool DeferableObject::pushHandleToStack() const {
    if (!handle.valid()) {
      return false;
    }
    return handle.pushToStack();
  }

  bool DeferableObject::pushToStack() const {
    switch (mode) {
      case ValueMode::HandleOnly:
      case ValueMode::Both:
        return pushHandleToStack();
      case ValueMode::ValueOnly:
        return pushObjectToStack();
      default:
        break;
    }
    return false;
  }

  bool DeferableObject::toBoolean() const {
    switch (otype) {
      case ObjectType::TBoolean:
        return object.bObject;
      case ObjectType::TInteger:
        return object.iObject != 0;
      case ObjectType::TNumber:
        return !((object.nObject >= -DBL_EPSILON) && (object.nObject <= DBL_EPSILON));
      default:
      //错误处理
        assert(false);
        break;
    }
    return false;
  }

  lua_Integer DeferableObject::toInteger() const {

    switch (otype) {
      case ObjectType::TBoolean:
        return object.bObject ? 1 : 0;
      case ObjectType::TInteger:
        return object.iObject;
      case ObjectType::TNumber:
        return object.nObject;
      default:
      //错误处理
        assert(false);
        break;
    }
    return 0;
  }

  lua_Number DeferableObject::toNumber() const {
    switch (otype) {
      case ObjectType::TBoolean:
        return object.bObject ? 1.0 : 0.0;
      case ObjectType::TInteger:
        return object.iObject;
      case ObjectType::TNumber:
        return object.nObject;
      default:
      //错误处理
        assert(false);
        break;
    }
    return 0;
  }

  void* DeferableObject::toUserData() const {
    switch (otype) {
      case ObjectType::TUserData:
        return object.pObject;
      default:
        //错误处理
        assert(false);
        break;
    }
    return nullptr;
  }

  const Handle &DeferableObject::toClosure() const {
    switch (otype) {
      case ObjectType::TFunction:
        return handle;
      default:
        assert(false);
        break;
    }
    static Handle empty;
    return empty;
  }

  const Handle &DeferableObject::toTable() const {
    switch (otype) {
      case ObjectType::TTable:
        return handle;
      default:
        //错误处理
        assert(false);
        break;
    }
    static Handle empty;
    return empty;
  }

  const char *DeferableObject::toString() const {

    switch (otype) {
      case ObjectType::TCString:
        return object.sObject->pString;
      default:
        //错误处理
        assert(false);
        break;
    }
    return nullptr;
  }

  bool DeferableObject::canConvertToBoolean() const {
    switch (otype) {
      case ObjectType::TBoolean:
      case ObjectType::TInteger:
      case ObjectType::TNumber:
        return true;
      default:
        break;
    }
    return false;
  }

  bool DeferableObject::canConvertToInteger() const {
    switch (otype) {
      case ObjectType::TBoolean:
      case ObjectType::TInteger:
      case ObjectType::TNumber:
        return true;
      default:
        break;
    }
    return false;
  }

  bool DeferableObject::canConvertToNumber() const {
    switch (otype) {
      case ObjectType::TBoolean:
      case ObjectType::TInteger:
      case ObjectType::TNumber:
        return true;
      default:
      //错误处理
        break;
    }
    return false;
  }

  bool DeferableObject::canConvertToUserData() const {
    switch (otype) {
      case ObjectType::TUserData:
        return true;
      default:
        break;
    }
    return false;
  }

  bool DeferableObject::canConvertToTable() const {
    switch (otype) {
      case ObjectType::TTable:
        return true;
      default:
        break;
    }
    return false;
  }

  bool DeferableObject::canConvertToClosure() const {
    switch (otype) {
      case ObjectType::TFunction:
        return true;
      default:
        break;
    }
    return false;
  }

  bool DeferableObject::canConvertToString() const {
    switch (otype) {
      case ObjectType::TCString:
        return true;
      default:
        break;
    }
    return false;
  }

  void DeferableObject::maybeRefString(CObject &_object , const ObjectType &type) {
    String *&sString = _object.sObject;
    if (type == ObjectType::TCString && sString->refcount != 0) {
      ++(sString->refcount);
    }
  }

  void DeferableObject::maybeDerefString(CObject &_object , const ObjectType &type) {
    String *&sString = _object.sObject;
    if (type == ObjectType::TCString) {
      if (sString->refcount == 1) {
        free(const_cast<char*>(sString->pString));
        delete _object.sObject;
      } else {
        --(sString->refcount);
      }
    }
  }

  void DeferableObject::swap(DeferableObject &other) {
    std::swap(object, other.object);
    std::swap(otype, other.otype);
    handle.swap(other.handle);
    std::swap(mode, other.mode);
  }

  const Context &DeferableObject::getContext() const {
    const Context *ctx = handle.getContext();
    assert(ctx);
    return *ctx;
  }
} /* lua */
