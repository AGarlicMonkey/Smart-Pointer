#pragma once

#include <list>

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
	template<typename T, typename D> class RefPtrBase;
	template<typename T, typename D> class SharedPtr;
	template<typename T, typename D> class WeakPtr;
	template<typename T, typename D> class UniquePtr;

	/////////POINTER BASE
	template<typename T, typename D = DefaultDeleter>
	class PtrBase{
		template<typename U, typename D> friend class PtrBase;

		//VARIABLES
	protected:
		T * object = nullptr;

		D deleter;

		//FUNCTIONS	
	public:
		virtual ~PtrBase() = default;

		T* get() const{
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
	template<typename T, typename D = DefaultDeleter>
	class RefPtrBase : public PtrBase<T, D>{
		template<typename U, typename D> friend class RefPtrBase;

		//VARIABLES
	protected:
		Counter * ref = nullptr;

		//FUNCTIONS	
	public:
		virtual ~RefPtrBase() = default;

		virtual bool isValid() const override{
			return ref ? ref->check() > 0 ? object != nullptr : false : false;
		}
	};

	/////////SHARED POINTER
	template<typename T, typename D = DefaultDeleter>
	class SharedPtr : public RefPtrBase<T, D>{
		template<typename U, typename D> friend class SharedPtr;

		template<typename T, typename D> friend class WeakPtr;
		template<typename U, typename D> friend class WeakPtr;

		template<typename T> friend class SharedFromThis;
		template<typename U> friend class SharedFromThis;

		//FUNCTIONS
	public:
		explicit SharedPtr() = default;
		explicit SharedPtr(T* inObject){
			if(inObject){
				init(inObject);
			}
		}

		SharedPtr(const SharedPtr<T, D>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}
		SharedPtr(const WeakPtr<T, D>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}

		template <typename U> SharedPtr(const SharedPtr<U, D>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}
		template <typename U> SharedPtr(const WeakPtr<U, D>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}

		template <typename U> SharedPtr(const SharedPtr<U, D>& ptr, T* obj){
			if(ptr.isValid() && obj){
				init(obj, ptr.ref);
			}
		}

		~SharedPtr(){
			free();
		}

		T* operator->(){
			return get();
		}
		T* operator->() const{
			return get();
		}

		T& operator*(){
			return *get();
		}
		T& operator*() const{
			return *get();
		}

		SharedPtr<T, D>& operator=(T* inObject){
			if(object != inObject){
				free();
				if(inObject){
					init(inObject);
				}
			}
			return *this;
		}
		SharedPtr<T, D>& operator=(const SharedPtr<T, D>& ptr){
			if(this != &ptr){
				free();
				if(ptr.isValid()){
					init(ptr.object, ptr.ref);
				}
			}
			return *this;
		}
		SharedPtr<T, D>& operator=(const WeakPtr<T, D>& ptr){
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
		void init(T* inObject, Counter* inRef = nullptr){
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
	template<typename T, typename D = DefaultDeleter>
	class WeakPtr : public RefPtrBase<T, D>{
		template<typename U, typename D> friend class WeakPtr;

		template<typename T, typename D> friend class SharedPtr;
		template<typename U, typename D> friend class SharedPtr;

		template<typename T> friend class SharedFromThis;
		template<typename U> friend class SharedFromThis;

		//FUNCTIONS
	public:
		explicit WeakPtr() = default;

		WeakPtr(const WeakPtr<T, D>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}
		WeakPtr(const SharedPtr<T, D>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}

		template<typename U> WeakPtr(const WeakPtr<U, D>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}
		template<typename U> WeakPtr(const SharedPtr<U, D>& ptr){
			if(ptr.isValid()){
				init(ptr.object, ptr.ref);
			}
		}

		template <typename U> WeakPtr(const WeakPtr<U, D>& ptr, T* obj){
			if(ptr.isValid() && obj){
				init(obj, ptr.ref);
			}
		}

		~WeakPtr(){
			free();
		}

		SharedPtr<T, D> pin(){
			return SharedPtr<T, D>(*this);
		}

		WeakPtr<T, D>& operator=(const WeakPtr<T, D>& ptr){
			if(this != &ptr){
				free();
				if(ptr.isValid()){
					init(ptr.object, ptr.ref);
				}
			}
			return *this;
		}
		WeakPtr<T, D>& operator=(const SharedPtr<T, D>& ptr){
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
		void init(T* inObject, Counter* inRef){
			object = inObject;
			if(inRef){
				ref = inRef;
				ref->weakGrab();
			}
		}
	};

	/////////SHARED FROM THIS
	template<typename T>
	class SharedFromThis{
		typedef T shT;

		//VARIABLES
	private:
		WeakPtr<T> weakThis;

		//FUNCTIONS
	public:
		WeakPtr<T> getWeakThis() const{
			return weakThis;
		}
		SharedPtr<T> getSharedThis() const{
			return weakThis.pin();
		}

		template<typename U> WeakPtr<U> getWeakThis() const{
			return ptr::staticCast<U, T>(weakThis);
		}
		template<typename U> SharedPtr<U> getSharedThis() const{
			return getWeakThis<U>().pin();
		}

	private:
		template<typename T>
		friend void enable(typename T::shT* ptr, SharedPtr<T>* shptr){
			if(ptr){
				ptr->doEnable(ptr, shptr);
			}
		}
		friend void enable(const volatile void* Ptr, const volatile void* shptr){
			//Not of type SharedFromThis - do nothing
		}

		template<typename U>
		void doEnable(T* ptr, SharedPtr<U>* shptr){
			if(ptr && shptr){
				ptr->weakThis.init(shptr->get(), shptr->ref);
			}
		}
	};

	template<typename T, typename D = DefaultDeleter>
	class UniquePtr : public PtrBase<T, D>{
		template<typename U, typename D> friend class UniquePtr;

		//FUNCTIONS
	public:
		explicit UniquePtr() = default;
		explicit UniquePtr(T* inObject){
			object = inObject;
		}

		UniquePtr(UniquePtr<T, D>&& ptr){
			object = ptr.object;
			ptr.object = nullptr;
		}

		template<typename U> UniquePtr(UniquePtr<U, D>&& ptr){
			object = ptr.object;
			ptr.object = nullptr;
		}

		~UniquePtr(){
			free();
		}

		UniquePtr<T, D> move(){
			UniquePtr<T, D> out(object);
			object = nullptr;
			return out;
		}

		T* operator->(){
			return get();
		}
		T* operator->() const{
			return get();
		}

		T& operator*(){
			return *get();
		}
		T& operator*() const{
			return *get();
		}

		UniquePtr<T, D>& operator=(UniquePtr<T, D>&& ptr){
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
	template<typename T>
	UniquePtr<T> makeUnique(T* object){
		return UniquePtr<T>(object);
	}

	template<typename T>
	SharedPtr<T> makeShared(T* object){
		return SharedPtr<T>(object);
	}

	template<typename RetT, typename CurrT>
	SharedPtr<RetT> staticCast(const SharedPtr<CurrT>& ptr){
		RetT* otherObj = static_cast<RetT*>(ptr.get());
		SharedPtr<RetT> outPtr(ptr, otherObj);
		return outPtr;
	}

	template<typename RetT, typename CurrT>
	SharedPtr<RetT> dynamicCast(const SharedPtr<CurrT>& ptr){
		if(RetT* otherObj = dynamic_cast<RetT*>(ptr.get())){
			return SharedPtr<T>(ptr, otherObj);
		}
		return SharedPtr<T>();
	}

	template<typename RetT, typename CurrT>
	SharedPtr<RetT> constCast(const SharedPtr<CurrT>& ptr){
		RetT* otherObj = const_cast<RetT*>(ptr.get());
		SharedPtr<RetT> outPtr(ptr, otherObj);
		return outPtr;
	}

	template<typename RetT, typename CurrT>
	SharedPtr<RetT> reinterpretCast(const SharedPtr<CurrT>& ptr){
		RetT* otherObj = reinterpret_cast<RetT*>(ptr.get());
		SharedPtr<RetT> outPtr(ptr, otherObj);
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