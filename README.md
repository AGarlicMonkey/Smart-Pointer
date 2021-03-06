# Smart Pointer
This is a C++ reference counted smart pointer solution.

Version 1.0.7

**_Disclaimer_**

Making an effecient reference counting system is complicated and difficult, so I do not recommend using this in any serious project. If you need a reference counted / smart pointer solution in your project then I would recommend using the [std smart pointer](https://msdn.microsoft.com/en-us/library/hh279674.aspx?f=255&MSPPError=-2147217396).

This was a project that was started for educational purposes and will be maintained as such.

**Use with caution!**

#

1. [Shared Pointer](#SP)
2. [Casting](#Ca)
3. [Weak Pointer](#WP)
4. [SharedFromThis](#SFT)
5. [Unique Pointer](#UP)
6. [Custom Deleters](#CD)

#

## <a name="SP"></a> Shared Pointer
A ```SharedPtr``` is a way to keep a strong reference to an object - while at least one ```SharedPtr``` is pointing to an object that object will not be deleted.

#### Usage
You can initialise a ```SharedPtr``` like so:
```C++
class MyObj{
  public:
  int x;
};
agm::SharedPtr<MyObj> myPtr = agm::makeShared(new MyObj());
```

From this point on you can use the ```SharedPtr``` like a normal C++ raw pointer.

```C++
//nullptr check
if(myPtr){
  //...
}
if(myPtr != nullptr){
  //...
}
if(myPtr == nullptr){
  //...
}

//Memeber access
myPtr->x += 1;

//Dereferencing
MyObj obj = *myPtr;
```

```SharedPtr``` also includes explicit functions for these operations.

```C++
if(myPtr.isValid()){
  //...
}

myPtr.get()->x += 1;
```

You can reset your ```SharedPtr``` at anytime which will make the ```SharedPtr``` release the object it is pointing to or delete it if it is the last strong reference holding onto it.

```C++
myPtr.reset();
```

A ```SharedPtr``` will also be reset once it leaves a scope.

```C++
{
  agm::SharedPtr<MyObj> myPtr = agm::makeShared(new MyObj());
  //myPtr is now valid
  //...
}

//MyObj pointed to by myPtr has now been cleaned up
```

You can also use a ```SharedPtr``` to initialise another one.

```C++
agm::SharedPtr<MyObj> myPtr1 = agm::makeShared(new MyObj());
agm::SharedPtr<MyObj> myPtr2 = myPtr1;
```


### <a name="Ca"></a> Casting
Currently **only SharedPtr** can be cast. This does however support the four casting types; static, dynamic, const and reiniterpret.

```C++
//Static
agm::SharedPtr<Base> b = agm::makeShared(new Derived());
agm::SharedPtr<Derived> d = agm::staticCast<Derived>(b);

//Dynamic
agm::SharedPtr<Base> b = agm::makeShared(new Derived());
agm::SharedPtr<Derived> d = agm::dynamicCast<Derived>(b);

//Const
agm::SharedPtr<int> i = agm::makeShared(new int(10));
agm::SharedPtr<const int> ci = agm::constCast<const int>(i);

//Reinterpret
struct S{ int a; };
agm::SharedPtr<S> structPtr = agm::makeShared(new S());
agm::SharedPtr<int> intPtr = agm::reinterpretCast<int>(structPtr);
```

### <a name="WP"></a> Weak Pointer
A ```WeakPtr``` is similar to a SharedPtr except for a few key differences.
1. A ```WeakPtr``` can only be initialised from a ```SharedPtr``` or another valid ```WeakPtr```.
2. A ```WeakPtr``` will not keep an object alive, once the last ```SharedPtr``` has been reset the object will be deleted.
3. A ```WeakPtr``` does not overload any operators, there for you can only access it with ```get();```.
4. WeakPtr has a ```pin();``` function which returns a SharedPtr to the pointed to object (if there is one).

#### Usage
```C++
class MyObj{
  public:
  int x;
};
agm::SharedPtr<MyObj> mySharedPtr = agm::makeShared(new MyObj());
agm::WeakPtr<MyObj> myWeakPtr = mySharedPtr;

if(myWeakPtr){
  myWeakPtr.get()->x += 1;
  //...
}

{
  agm::SharedPtr<MyObj> myOtherShared = myWeakPtr.pin();
  //...
}

mySharedPtr.reset();
//WeakPtr is no longer valid
if(myWeakPtr){
  //Will not reach this code
}
```

### <a name="SFT"></a> Shared From This
The class ```SharedFromThis``` can be inherited from to allow you to construct ```SharedPtr```s or ```WeakPtr```s.

#### Usage
```C++
class MyObj : public SharedFromThis<MyObj>{
  public:
  int x;
  
  void spawnObj();
};

class ChildObj{
  public:
  agm::WeakPtr<MyObj> owner;
};

void MyObj::spawnObj(){
  agm::SharedPtr<ChildObj> spawnedChild = agm::makeShared(new ChildObj());
  
  //You can use getWeakThis();
  spawnedChild->owner = getWeakThis();
  //Or you can use getSharedThis();
  spawnedChild->owner = getSharedThis();
}
```

```getSharedThis();``` and ```getWeakThis();``` are also templated if you need to return a specific type.

```C++
void MyObj::spawnObj(){
  agm::SharedPtr<ChildObj> spawnedChild = agm::makeShared(new ChildObj());
  
  spawnedChild->owner = getWeakThis<DerivedObj>();
  spawnedChild->owner = getSharedThis<DerivedObj>();
}
```

## <a name="UP"></a> Unique Pointer
The key difference between a ```UniquePtr``` and a ```SharedPtr``` or ```WeakPtr``` is that only one ```UniquePtr``` can be pointing to an object at any one time. Assigning a ```UniquePtr``` to another means the original ```UniqePtr``` has to give up ownership.

#### Usage
```C++
class MyObj{
  public:
  int x;
}

agm::UniquePtr<MyObj> myUnqiue = agm::makeUnique(new MyObj());

if(myUnique){
  //...
}

if(myUnique.isValid()){
  //...
}

myUnique->x += 1;
myUnique.get()->x += 1;

myUnique.reset();
```
```UniquePtr```s have a ```move();``` which is how you assign one ```UniquePtr``` to another

```C++
agm::UniquePtr<MyObj> ptr1 = agm::makeUnique(new MyObj());

//ptr1 is now valid

agm::UniquePtr<MyObj> ptr2 = ptr1.move();

//ptr1 is no longer valid - ptr2 now has ownership and is responsible for the object's life time 
```

## <a name="CD"></a> Custom Deleters
All three pointer types mentioned can have custom deleters assigned to them if your object requires specific functionality to be performed before you delete it

#### Usage
```C++
class MyObj{
  public:
  int x;
}

struct MyObjDeleter{
  void operator(MyObj* obj){
    std::cout << "Custom deleter called" << std::endl;
    obj->x = 0;
    delete obj;
  }
}

agm::SharedPtr<MyObj, MyObjDeleter> sharedPtr = agm::makeShared(new MyObj());
sharedPtr.reset(); //Custom deleter called

agm::UniquePtr<MyObj, MyObjDeleter> uniquePtr = agm::makeUnique(new MyObj());
uniquePtr.reset(); //Custom deleter called
```
