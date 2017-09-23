#include "Arguments.h"
#include "DeferableObject.h"
#include "Context.h"
namespace lua {

  Arguments::Arguments(size_t nobj) : objects(new DeferableObject[nobj]()),
                                      upvalues(nullptr) ,
                                      nupvalue(0) ,
                                      nobjects(nobj) {

  }

  Arguments::Arguments(const Context &context , size_t nobj) :
                            objects(nullptr) ,
                            upvalues(nullptr) ,
                            nupvalue(0) ,
                            nobjects(nobj) {
      int offset = 0;
      StateHandle lstate = context.getStateHandle();
      if (lua_istable(lstate, 1)) {
        --nobjects;
        offset = 1;
        table = Table(context , 1);
      }

      objects = new DeferableObject[nobjects]();
      for (size_t idx = 0; idx != nobjects; ++idx) {
        objects[idx] = DeferableObject(Handle(context ,
                                  idx + 1 + offset
                                ) ,
                                DeferableObject::HandleMode::Argument);
      }
  }

  Arguments::Arguments (
      const Context &context , size_t nobj , size_t nupvalue) :
      Arguments(context , nobj) {
        this->nupvalue = nupvalue;
        upvalues = nupvalue > 0 ? new DeferableObject[nupvalue]() : nullptr;

        if (upvalues != nullptr) {
          int freeIndex = 4; //由于前三个被占领，只能从第四个开始
          for (size_t idx = 0; idx != nupvalue; ++idx) {
            Handle hd(context ,lua_upvalueindex(freeIndex + idx));
            upvalues[idx] = DeferableObject(hd ,
                                    DeferableObject::HandleMode::Argument);
          }
        }
  }

  Arguments::~Arguments() {
    delete[] objects;
    delete[] upvalues;
  }
} /* lua */
