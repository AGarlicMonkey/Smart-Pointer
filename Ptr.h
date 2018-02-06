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

template<typename T>
struct DefaultDeleter{
	void operator()(T* ptr){
		delete ptr;
	}
};

//Pointer types
template<typename T> class RefPtrBase;
template<typename T> class SharedPtr;
template<typename T> class WeakPtr;
template<typename T> class UniquePtr;

template<typename T>
class PtrBase{
	template<typename U> friend class PtrBase;

	//VARIABLES
protected:
	T* object = nullptr;

	DefaultDeleter<T> deleter;

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

template<typename T>
class RefPtrBase : public PtrBase<T>{
	template<typename U> friend class RefPtrBase;

	//VARIABLES
protected:
	Counter* ref = nullptr;

	//FUNCTIONS	
public:
	virtual ~RefPtrBase() = default;

	virtual bool isValid() const override;
};

template<typename T>
class SharedPtr : public RefPtrBase<T>{
	template<typename U> friend class SharedPtr;

	template<typename T> friend class WeakPtr;
	template<typename U> friend class WeakPtr;

	//FUNCTIONS
public:
	explicit SharedPtr() = default;
	explicit SharedPtr(T* inObject);

	SharedPtr(const SharedPtr<T>& ptr);
	SharedPtr(const WeakPtr<T>& ptr);
	
	template <typename U> SharedPtr(const SharedPtr<U>& ptr);
	template <typename U> SharedPtr(const WeakPtr<U>& ptr);

	~SharedPtr();

	T* operator->();
	T* operator->() const;

	T& operator*();
	T& operator*() const;

	SharedPtr<T>& operator=(T* inObject);
	SharedPtr<T>& operator=(const SharedPtr<T>& ptr);
	SharedPtr<T>& operator=(const WeakPtr<T>& ptr);

protected:
	virtual void free() override;

private:
	void init(T* inObject, Counter* inRef = nullptr);
};

template<typename T>
class WeakPtr : public RefPtrBase<T>{
	template<typename U> friend class WeakPtr;

	template<typename T> friend class SharedPtr;
	template<typename U> friend class SharedPtr;

	//FUNCTIONS
public:
	explicit WeakPtr() = default;

	WeakPtr(const WeakPtr<T>& ptr);
	WeakPtr(const SharedPtr<T>& ptr);
	
	template<typename U> WeakPtr(const WeakPtr<U>& ptr);
	template<typename U> WeakPtr(const SharedPtr<U>& ptr);

	~WeakPtr();

	SharedPtr<T> pin();

	WeakPtr<T>& operator=(const WeakPtr<T>& ptr);
	WeakPtr<T>& operator=(const SharedPtr<T>& ptr);

protected:
	virtual void free() override;

private:
	void init(T* inObject, Counter* inRef);
};

template<typename T>
class SharedFromThis{
	//VARIABLES
private:
	WeakPtr<T> weakThis;

	//FUNCTIONS
public:
	WeakPtr<T> getWeakThis() const;
	SharedPtr<T> getSharedThis() const;

private:
	template<typename T, typename U>
	friend void enable(SharedFromThis<T>* ptr, SharedPtr<U>* shptr);
	friend void enable(const volatile void* Ptr, const volatile void* shptr);
	
	void doEnable(SharedPtr<T>* shptr);
};

template<typename T>
class UniquePtr : public PtrBase<T>{
	template<typename U> friend class UniquePtr;

	//FUNCTIONS
public:
	explicit UniquePtr() = default;
	explicit UniquePtr(T* inObject);
	UniquePtr(UniquePtr<T>&& ptr);

	~UniquePtr();

	UniquePtr<T> move();
	template<typename U> UniquePtr<U> move();

	T* operator->();
	T* operator->() const;

	T& operator*();
	T& operator*() const;

protected:
	virtual void free() override;
};

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