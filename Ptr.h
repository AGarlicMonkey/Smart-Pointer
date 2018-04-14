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

		Type* get() const;

		virtual bool isValid() const;

		void reset();

		explicit operator bool() const;

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

		virtual bool isValid() const override;
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
		explicit SharedPtr(Type* inObject);

		SharedPtr(const SharedPtr<Type, DeleterType>& ptr);
		SharedPtr(const WeakPtr<Type, DeleterType>& ptr);

		template<typename OtherType> SharedPtr(const SharedPtr<OtherType, DeleterType>& ptr);
		template<typename OtherType> SharedPtr(const WeakPtr<OtherType, DeleterType>& ptr);

		template <typename OtherType> SharedPtr(const SharedPtr<OtherType, DeleterType>& ptr, Type* obj);

		~SharedPtr();

		Type* operator->();
		Type* operator->() const;

		Type& operator*();
		Type& operator*() const;

		SharedPtr<Type, DeleterType>& operator=(Type* inObject);
		SharedPtr<Type, DeleterType>& operator=(const SharedPtr<Type, DeleterType>& ptr);
		SharedPtr<Type, DeleterType>& operator=(const WeakPtr<Type, DeleterType>& ptr);

	protected:
		virtual void free() override;

	private:
		void init(Type* inObject, Counter* inRef = nullptr);
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

		WeakPtr(const WeakPtr<Type, DeleterType>& ptr);
		WeakPtr(const SharedPtr<Type, DeleterType>& ptr);

		template<typename OtherType> WeakPtr(const WeakPtr<OtherType, DeleterType>& ptr);
		template<typename OtherType> WeakPtr(const SharedPtr<OtherType, DeleterType>& ptr);

		template <typename OtherType> WeakPtr(const WeakPtr<OtherType, DeleterType>& ptr, Type* obj);

		~WeakPtr();

		SharedPtr<Type, DeleterType> pin();

		WeakPtr<Type, DeleterType>& operator=(const WeakPtr<Type, DeleterType>& ptr);
		WeakPtr<Type, DeleterType>& operator=(const SharedPtr<Type, DeleterType>& ptr);

	protected:
		virtual void free() override;

	private:
		void init(Type* inObject, Counter* inRef);
	};

	/////////SHARED FROM THIS
	template<typename Type>
	class SharedFromThis{
		typedef Type SharedType;

		//VARIABLES
	private:
		WeakPtr<Type> weakThis;

		//FUNCTIONS
	public:
		WeakPtr<Type> getWeakThis() const;
		SharedPtr<Type> getSharedThis() const;

		template<typename OtherType> WeakPtr<OtherType> getWeakThis() const;
		template<typename OtherType> SharedPtr<OtherType> getSharedThis() const;

	private:
		template<typename Type>
		friend void enable(typename Type::SharedType* ptr, SharedPtr<Type>* shptr);
		friend void enable(const volatile void* ptr, const volatile void* shptr);

		template<typename PtrType>
		void doEnable(Type* ptr, SharedPtr<PtrType>* shptr);
	};

	/////////UNIQUE POINTER
	template<typename Type, typename DeleterType = DefaultDeleter>
	class UniquePtr : public PtrBase<Type, DeleterType>{
		template<typename OtherType, typename DeleterType> friend class UniquePtr;

		//FUNCTIONS
	public:
		explicit UniquePtr() = default;
		explicit UniquePtr(Type* inObject);

		UniquePtr(UniquePtr<Type, DeleterType>&& ptr);

		template<typename OtherType> UniquePtr(UniquePtr<OtherType, DeleterType>&& ptr);

		~UniquePtr();

		UniquePtr<Type, DeleterType> move();

		Type* operator->();
		Type* operator->() const;

		Type& operator*();
		Type& operator*() const;

		UniquePtr<Type, DeleterType>& operator=(UniquePtr<Type, DeleterType>&& ptr);

	protected:
		virtual void free() override;
	};

	/////////HELPER FUNCTIONS
	template<typename Type>
	UniquePtr<Type> makeUnique(Type* object);

	template<typename Type>
	SharedPtr<Type> makeShared(Type* object);

	template<typename ReturnType, typename CurrentType>
	SharedPtr<ReturnType> staticCast(const SharedPtr<CurrentType>& ptr);
	template<typename ReturnType, typename CurrentType>
	SharedPtr<ReturnType> dynamicCast(const SharedPtr<CurrentType>& ptr);
	template<typename ReturnType, typename CurrentType>
	SharedPtr<ReturnType> constCast(const SharedPtr<CurrentType>& ptr);
	template<typename ReturnType, typename CurrentType>
	SharedPtr<ReturnType> reinterpretCast(const SharedPtr<CurrentType>& ptr);
}

/////////INLINE INCLUDE
#include "Ptr.inl"

/////////COMPARISON OPERATORS
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