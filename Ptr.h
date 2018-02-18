#pragma once

#include <list>

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

struct DefaultDeleter{
	void operator()(void* ptr){
		delete ptr;
	}
};

//Pointer types
template<typename T, typename D> class RefPtrBase;
template<typename T, typename D> class SharedPtr;
template<typename T, typename D> class WeakPtr;
template<typename T, typename D> class UniquePtr;

template<typename T, typename D = DefaultDeleter>
class PtrBase{
	template<typename U, typename D> friend class PtrBase;

	//VARIABLES
protected:
	T* object = nullptr;

	D deleter;

	//FUNCTIONS	
public:
	virtual ~PtrBase() = default;

	T* get() const;
	virtual bool isValid() const;

	void reset();

	explicit operator bool() const;

protected:
	virtual void free() = 0;
};

template<typename T, typename D = DefaultDeleter>
class RefPtrBase : public PtrBase<T, D>{
	template<typename U, typename D> friend class RefPtrBase;

	//VARIABLES
protected:
	Counter* ref = nullptr;

	//FUNCTIONS	
public:
	virtual ~RefPtrBase() = default;

	virtual bool isValid() const override;
};

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
	explicit SharedPtr(T* inObject);

	SharedPtr(const SharedPtr<T, D>& ptr);
	SharedPtr(const WeakPtr<T, D>& ptr);
	
	template <typename U> SharedPtr(const SharedPtr<U, D>& ptr);
	template <typename U> SharedPtr(const WeakPtr<U, D>& ptr);

	~SharedPtr();

	T* operator->();
	T* operator->() const;

	T& operator*();
	T& operator*() const;

	SharedPtr<T, D>& operator=(T* inObject);
	SharedPtr<T, D>& operator=(const SharedPtr<T, D>& ptr);
	SharedPtr<T, D>& operator=(const WeakPtr<T, D>& ptr);

protected:
	virtual void free() override;

private:
	void init(T* inObject, Counter* inRef = nullptr);
};

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

	WeakPtr(const WeakPtr<T, D>& ptr);
	WeakPtr(const SharedPtr<T, D>& ptr);
	
	template<typename U> WeakPtr(const WeakPtr<U, D>& ptr);
	template<typename U> WeakPtr(const SharedPtr<U, D>& ptr);

	~WeakPtr();

	SharedPtr<T, D> pin();

	WeakPtr<T, D>& operator=(const WeakPtr<T, D>& ptr);
	WeakPtr<T, D>& operator=(const SharedPtr<T, D>& ptr);

protected:
	virtual void free() override;

private:
	void init(T* inObject, Counter* inRef);
};

template<typename T>
class SharedFromThis{
	typedef T shT;

	//VARIABLES
private:
	WeakPtr<T> weakThis;

	//FUNCTIONS
public:
	WeakPtr<T> getWeakThis() const;
	SharedPtr<T> getSharedThis() const;

	template<typename U> WeakPtr<U> getWeakThis() const;
	template<typename U> SharedPtr<U> getSharedThis() const;

private:
	template<typename T>
	friend void enable(typename T::shT* ptr, SharedPtr<T>* shptr);
	friend void enable(const volatile void* Ptr, const volatile void* shptr);
	
	template<typename U>
	void doEnable(T* ptr, SharedPtr<U>* shptr);
};

template<typename T, typename D = DefaultDeleter>
class UniquePtr : public PtrBase<T, D>{
	template<typename U, typename D> friend class UniquePtr;

	//FUNCTIONS
public:
	explicit UniquePtr() = default;
	explicit UniquePtr(T* inObject);

	UniquePtr(UniquePtr<T, D>&& ptr);

	template<typename U> UniquePtr(UniquePtr<U, D>&& ptr);

	~UniquePtr();

	UniquePtr<T, D> move();

	T* operator->();
	T* operator->() const;

	T& operator*();
	T& operator*() const;

	UniquePtr<T, D>& operator=(UniquePtr<T, D>&& ptr);

protected:
	virtual void free() override;
};

///////HELPER FUNCTIONS
namespace ptr{
	template<typename T>
	UniquePtr<T> makeUnique(T* object);

	template<typename T>
	SharedPtr<T> makeShared(T* object);

	template<typename T, typename U>
	UniquePtr<T> staticCast(UniquePtr<U>& other);
	template<typename T, typename U>
	SharedPtr<T> staticCast(SharedPtr<U>& other);
	template<typename T, typename U>
	WeakPtr<T> staticCast(WeakPtr<U>& other);

	template<typename T, typename U>
	UniquePtr<T> dynamicCast(UniquePtr<U>& other);
	template<typename T, typename U>
	SharedPtr<T> dynamicCast(SharedPtr<U>& other);
	template<typename T, typename U>
	WeakPtr<T> dynamicCast(WeakPtr<U>& other);
}

///////INLINE INCLUDE
#include "Ptr.inl"

///////COMPARISON OPERATORS
//T == T
template<typename T>
inline bool operator==(const PtrBase<T>& lptr, const PtrBase<T>& rptr){
	return lptr.get() == rptr.get();
}
template<typename T>
inline bool operator==(const PtrBase<T>& ptr, const T* object){
	return ptr.get() == object;
}
template<typename T>
inline bool operator==(const PtrBase<T>& ptr, const T& object){
	return ptr.get() == *object;
}

//T != T
template<typename T>
inline bool operator!=(const PtrBase<T>& lptr, const PtrBase<T>& rptr){
	return !(lptr == rptr);
}
template<typename T>
inline bool operator!=(const PtrBase<T>& ptr, const T* object){
	return !(ptr == object);
}
template<typename T>
inline bool operator!=(const PtrBase<T>& ptr, const T& object){
	return !(ptr == object);
}

//T == U
template<typename T, typename U>
inline bool operator==(const PtrBase<T>& lptr, const PtrBase<U>& rptr){
	return lptr.get() == rptr.get();
}
template<typename T, typename U>
inline bool operator==(const PtrBase<T>& ptr, const U* object){
	return ptr.get() == object;
}

//T != U
template<typename T, typename U>
inline bool operator!=(const PtrBase<T>& lptr, const PtrBase<U>& rptr){
	return !(lptr == rptr);
}
template<typename T, typename U>
inline bool operator!=(const PtrBase<T>& ptr, const U* object){
	return !(ptr == object);
}

//T == nullptr_t
template<typename T>
inline bool operator==(const PtrBase<T>& ptr, nullptr_t object){
	return !ptr.isValid();
}

//T != nullptr_t
template<typename T>
inline bool operator!=(const PtrBase<T>& ptr, nullptr_t object){
	return !(ptr == object);
}