#include <iostream>
#include "Context.h"
#include "MetaObject.h"
using namespace std;
using namespace lua;

class Rectangle {
private:
  double w;
  double h;
public:
  Rectangle (double w , double h) : w(w) ,h(h) {

  }

  double getArea() const {
    return w * h;
  }
  virtual ~Rectangle () {
    printf("~Rectangle\n");
  }
};

class Square : public Rectangle {
private:
  /* data */

public:
  Square (double w , double h ) : Rectangle(w,h) {

  };
  virtual ~Square () {
    printf("~Square\n");
  }
};

void initRectangle(Context &ctx) {

  ExtendClass rectangle = MetaObject::extend(ctx, "Rectangle");

  rectangle.setInitializeMethod(Closure::New(ctx,
            [](const Context &context , const Arguments &args) -> DeferableObject {
              const Table *self = args.self();
              Rectangle *r = new Rectangle(args[0].toNumber() , args[1].toNumber());
              self->setField("this", DeferableObject(context , r));
              return DeferableObject();
            } , nullptr))
         .setMethod("getArea",Closure::New(ctx , [](const Context &context , const Arguments &args) -> DeferableObject {
            const Table *self = args.self();
            DeferableObject thisPointer =  self->getField("this");
            Rectangle *r = thisPointer.toPointer<Rectangle>();
            return DeferableObject(context , r->getArea());
          } , nullptr))
          .setFinalizers(Closure::New(ctx, [](const Context &context , const Arguments &args) -> DeferableObject {
            const Table *self = args.self();
            DeferableObject object = self->getField("this");
            int exist = object.type();
            if (exist == DeferableObject::ObjectType::TUserData) {
              Rectangle *r = object.toPointer<Rectangle>();
              delete r;
            }
            return DeferableObject();
          }, nullptr));
    ExtendClass SquareClass = MetaObject::extend(ctx, "Square", "Rectangle");
    SquareClass.setInitializeMethod(Closure::New(ctx,
              [](const Context &context , const Arguments &args) -> DeferableObject {
                const Table *self = args.self();
                Square *r = new Square(args[0].toNumber() , args[1].toNumber());
                self->setField("this", DeferableObject(context , r));
                return DeferableObject();
              } , nullptr));
}

int main()
{
    Context &ctx = Context::getInstance();
    ctx.OpenStandardLibrary();

    MetaObject::registerObject(ctx);
    initRectangle(ctx);

    ctx.RunScriptFile("./hello.lua");

    return 0;
}
