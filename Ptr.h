#pragma once

namespace ptr{
	/////////COUNTER
	class Counter{
		//VARIALBES
	private:
		int strongCount = 0;
		int weakCount = 0;

		//FUNCTIONS
	public:
		inline void grab(){ ++strongCount; }
		inline void weakGrab(){ ++weakCount; }

		inline int check(){ return strongCount; }
		inline int fullCheck(){ return strongCount + weakCount; }

		inline int release(){ return --strongCount; }
		inline int weakRelease(){ return --weakCount; }
	};

	/////////DEFAULT DELETER
	struct DefaultDeleter{
		void operator()(void* ptr){
			delete ptr;
		}
	};

	/////////POINTER TYPES
	template<typename Type, typename DeleterType> class RefPtrBase;
	template<typename Type, typename DeleterType> class SharedPtr;
	template<typename Type, typename DeleterType> class WeakPtr;
	template<typename Type, typename DeleterType> class UniquePtr;

	/////////POINTER BASE
	template<typename Type, typename DeleterType = DefaultDeleter>
	class PtrBase{
		template<typename OtherType, typename DeleterType> friend class PtrBase;

		//VARIABLES
	protected:
		Type* object = nullptr;

		DeleterType deleter;

		//FUNCTIONS	
	public:
		virtual ~PtrBase() = default;

		Type* get() const{
			return isValid() ? object : nullptr;
		}
		virtual bool isValid() const{
			return object != nullptr;
		}

		void reset(){
			free();
		}

		explicit operator bool() const{
			return isValid();
		}

	protected:
		virtual void free() = 0;
	};

	/////////REFERENCE POINTER BASE
	template<typename Type, typename DeleterType = DefaultDeleter>
	class RefPtrBase : public PtrBase<Type, DeleterType>{
		template<typename OtherType, typename DeleterType> friend class RefPtrBase;

		//VARIABLES
	protected:
		Counter* ref = nullptr;

		//FUNCTIONS	
	public:
		virtual ~RefPtrBase() = default;

		virtual bool isValid() const override{
			return ref ? ref->check() > 0 ? object != nullptr : false : false;
		}
	};

	/////////SHARED POINTER
	template<typename Type, typename DeleterType = DefaultDeleter>
	class SharedPtr : public RefPtrBase<Type, DeleterType>{
		template<typename OtherType, typename DeleterType> friend class SharedPtr;

		template<typename Type, typename DeleterType> friend class WeakPtr;
		template<typename OtherType, typename DeleterType> friend class WeakPtr;

		template<typename Type> friend class SharedFromThis;
		template<typename OtherType> friend class SharedFromThis;

		//FUNCTIONS
	public:
		explicit SharedPtr() = default;
		explicit SharedPtr(Type* inObject){
			if(inObject){
				init(inObject);
			}
		}

		SharedPtr(const SharedPtr<Type, DeleterType>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}
		SharedPtr(const WeakPtr<Type, DeleterType>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}

		template <typename OtherType> SharedPtr(const SharedPtr<OtherType, DeleterType>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}
		template <typename OtherType> SharedPtr(const WeakPtr<OtherType, DeleterType>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}

		template <typename OtherType> SharedPtr(const SharedPtr<OtherType, DeleterType>& ptr, Type* obj){
			if(ptr.isValid() && obj){
				init(obj, ptr.ref);
			}
		}

		~SharedPtr(){
			free();
		}

		Type* operator->(){
			return get();
		}
		Type* operator->() const{
			return get();
		}

		Type& operator*(){
			return *get();
		}
		Type& operator*() const{
			return *get();
		}

		SharedPtr<Type, DeleterType>& operator=(Type* inObject){
			if(object != inObject){
				free();
				if(inObject){
					init(inObject);
				}
			}
			return *this;
		}
		SharedPtr<Type, DeleterType>& operator=(const SharedPtr<Type, DeleterType>& ptr){
			if(this != &ptr){
				free();
				if(ptr.isValid()){
					init(ptr.object, ptr.ref);
				}
			}
			return *this;
		}
		SharedPtr<Type, DeleterType>& operator=(const WeakPtr<Type, DeleterType>& ptr){
			free();
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
			return *this;
		}

	protected:
		virtual void free() override{
			if(ref && ref->release() == 0){
				deleter(get());
				if(ref->fullCheck() == 0){
					delete ref;
				}
			}
			object = nullptr;
			ref = nullptr;
		}

	private:
		void init(Type* inObject, Counter* inRef = nullptr){
			object = inObject;
			if(inRef){
				ref = inRef;
				ref->grab();
			} else{
				ref = new Counter();
				ref->grab();

				enable(inObject, this);
			}
		}
	};

	/////////WEAK POINTER
	template<typename Type, typename DeleterType = DefaultDeleter>
	class WeakPtr : public RefPtrBase<Type, DeleterType>{
		template<typename OtherType, typename DeleterType> friend class WeakPtr;

		template<typename Type, typename DeleterType> friend class SharedPtr;
		template<typename OtherType, typename DeleterType> friend class SharedPtr;

		template<typename Type> friend class SharedFromThis;
		template<typename OtherType> friend class SharedFromThis;

		//FUNCTIONS
	public:
		explicit WeakPtr() = default;

		WeakPtr(const WeakPtr<Type, DeleterType>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}
		WeakPtr(const SharedPtr<Type, DeleterType>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}

		template<typename OtherType> WeakPtr(const WeakPtr<OtherType, DeleterType>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}
		template<typename OtherType> WeakPtr(const SharedPtr<OtherType, DeleterType>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}

		template <typename OtherType> WeakPtr(const WeakPtr<OtherType, DeleterType>& ptr, Type* obj){
			if(ptr.isValid() && obj){
				init(obj, ptr.ref);
			}
		}

		~WeakPtr(){
			free();
		}

		SharedPtr<Type, DeleterType> pin(){
			return SharedPtr<Type, DeleterType>(*this);
		}

		WeakPtr<Type, DeleterType>& operator=(const WeakPtr<Type, DeleterType>& ptr){
			if(this != &ptr){
				free();
				if(ptr.isValid()){
					init(ptr.object, ptr.ref);
				}
			}
			return *this;
		}
		WeakPtr<Type, DeleterType>& operator=(const SharedPtr<Type, DeleterType>& ptr){
			free();
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
			return *this;
		}

	protected:
		virtual void free() override{
			if(ref && ref->weakRelease() == 0 && ref->fullCheck() == 0){
				delete ref;
			}
			object = nullptr;
			ref = nullptr;
		}

	private:
		void init(Type* inObject, Counter* inRef){
			object = inObject;
			if(inRef){
				ref = inRef;
				ref->weakGrab();
			}
		}
	};

	/////////SHARED FROM THIS
	template<typename Type>
	class SharedFromThis{
		typedef Type shT;

		//VARIABLES
	private:
		WeakPtr<Type> weakThis;

		//FUNCTIONS
	public:
		WeakPtr<Type> getWeakThis() const{
			return weakThis;
		}
		SharedPtr<Type> getSharedThis() const{
			return SharedPtr<Type>(weakThis);
		}

		template<typename OtherType> WeakPtr<OtherType> getWeakThis() const{
			return ptr::staticCast<OtherType, Type>(weakThis);
		}
		template<typename OtherType> SharedPtr<OtherType> getSharedThis() const{
			return SharedPtr<OtherType>(getWeakThis<OtherType>());
		}

	private:
		template<typename Type>
		friend void enable(typename Type::shT* ptr, SharedPtr<Type>* shptr){
			if(ptr){
				ptr->doEnable(ptr, shptr);
			}
		}
		friend void enable(const volatile void* Ptr, const volatile void* shptr){
			//Not of type SharedFromThis - do nothing
		}

		template<typename OtherType>
		void doEnable(Type* ptr, SharedPtr<OtherType>* shptr){
			if(ptr && shptr){
				ptr->weakThis.init(shptr->get(), shptr->ref);
			}
		}
	};

	template<typename Type, typename DeleterType = DefaultDeleter>
	class UniquePtr : public PtrBase<Type, DeleterType>{
		template<typename OtherType, typename DeleterType> friend class UniquePtr;

		//FUNCTIONS
	public:
		explicit UniquePtr() = default;
		explicit UniquePtr(Type* inObject){
			object = inObject;
		}

		UniquePtr(UniquePtr<Type, DeleterType>&& ptr){
			object = ptr.object;
			ptr.object = nullptr;
		}

		template<typename OtherType> UniquePtr(UniquePtr<OtherType, DeleterType>&& ptr){
			object = ptr.object;
			ptr.object = nullptr;
		}

		~UniquePtr(){
			free();
		}

		UniquePtr<Type, DeleterType> move(){
			UniquePtr<Type, DeleterType> out(object);
			object = nullptr;
			return out;
		}

		Type* operator->(){
			return get();
		}
		Type* operator->() const{
			return get();
		}

		Type& operator*(){
			return *get();
		}
		Type& operator*() const{
			return *get();
		}

		UniquePtr<Type, DeleterType>& operator=(UniquePtr<Type, DeleterType>&& ptr){
			if(this != &ptr){
				if(isValid()){
					free();
				}
				object = ptr.object;
				ptr.object = nullptr;
			}
			return *this;
		}

	protected:
		virtual void free() override{
			if(isValid()){
				deleter(get());
			}
			object = nullptr;
		}
	};

	///////HELPER FUNCTIONS
	template<typename Type>
	UniquePtr<Type> makeUnique(Type* object){
		return UniquePtr<Type>(object);
	}

	template<typename Type>
	SharedPtr<Type> makeShared(Type* object){
		return SharedPtr<Type>(object);
	}

	template<typename ReturnType, typename CurrentType>
	SharedPtr<ReturnType> staticCast(const SharedPtr<CurrentType>& ptr){
		ReturnType* otherObj = static_cast<ReturnType*>(ptr.get());
		SharedPtr<ReturnType> outPtr(ptr, otherObj);
		return outPtr;
	}

	template<typename ReturnType, typename CurrentType>
	SharedPtr<ReturnType> dynamicCast(const SharedPtr<CurrentType>& ptr){
		if(ReturnType* otherObj = dynamic_cast<ReturnType*>(ptr.get())){
			return SharedPtr<ReturnType>(ptr, otherObj);
		}
		return SharedPtr<ReturnType>();
	}

	template<typename ReturnType, typename CurrentType>
	SharedPtr<ReturnType> constCast(const SharedPtr<CurrentType>& ptr){
		ReturnType* otherObj = const_cast<ReturnType*>(ptr.get());
		SharedPtr<ReturnType> outPtr(ptr, otherObj);
		return outPtr;
	}

	template<typename ReturnType, typename CurrentType>
	SharedPtr<ReturnType> reinterpretCast(const SharedPtr<CurrentType>& ptr){
		ReturnType* otherObj = reinterpret_cast<ReturnType*>(ptr.get());
		SharedPtr<ReturnType> outPtr(ptr, otherObj);
		return outPtr;
	}
}

///////COMPARISON OPERATORS
//T == T
template<typename T>
inline bool operator==(const ptr::PtrBase<T>& lptr, const ptr::PtrBase<T>& rptr){
	return lptr.get() == rptr.get();
}
template<typename T>
inline bool operator==(const ptr::PtrBase<T>& ptr, const T* object){
	return ptr.get() == object;
}
template<typename T>
inline bool operator==(const ptr::PtrBase<T>& ptr, const T& object){
	return ptr.get() == *object;
}

//T != T
template<typename T>
inline bool operator!=(const ptr::PtrBase<T>& lptr, const ptr::PtrBase<T>& rptr){
	return !(lptr == rptr);
}
template<typename T>
inline bool operator!=(const ptr::PtrBase<T>& ptr, const T* object){
	return !(ptr == object);
}
template<typename T>
inline bool operator!=(const ptr::PtrBase<T>& ptr, const T& object){
	return !(ptr == object);
}

//T == U
template<typename T, typename U>
inline bool operator==(const ptr::PtrBase<T>& lptr, const ptr::PtrBase<U>& rptr){
	return lptr.get() == rptr.get();
}
template<typename T, typename U>
inline bool operator==(const ptr::PtrBase<T>& ptr, const U* object){
	return ptr.get() == object;
}

//T != U
template<typename T, typename U>
inline bool operator!=(const ptr::PtrBase<T>& lptr, const ptr::PtrBase<U>& rptr){
	return !(lptr == rptr);
}
template<typename T, typename U>
inline bool operator!=(const ptr::PtrBase<T>& ptr, const U* object){
	return !(ptr == object);
}

//T == nullptr_t
template<typename T>
inline bool operator==(const ptr::PtrBase<T>& ptr, nullptr_t object){
	return ptr.get() == object;
}

//T != nullptr_t
template<typename T>
inline bool operator!=(const ptr::PtrBase<T>& ptr, nullptr_t object){
	return !(ptr == object);
}