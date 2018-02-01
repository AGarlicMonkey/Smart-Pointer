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

//Pointer types
template<typename T> class RefPtrBase;
template<typename T> class SharedPtr;
template<typename T> class WeakPtr;
template<typename T> class UniquePtr;

template<typename T>
class PtrBase{
	template<typename U> friend class PtrBase;

	template<typename T> friend class RefPtrBase;
	template<typename U> friend class RefPtrBase;

	template<typename T> friend class SharedPtr;
	template<typename U> friend class SharedPtr;

	template<typename T> friend class WeakPtr;
	template<typename U> friend class WeakPtr;

	//VARIABLES
protected:
	T* object = nullptr;

	//FUNCTIONS	
public:
	virtual ~PtrBase() = default;

	T* get() const;
	virtual bool isValid() const;

	void reset();

	bool operator==(const T* inObject);

	bool operator!=(const T* inObject);

	explicit operator bool() const;

protected:
	virtual void free() = 0;
};

template<typename T>
class RefPtrBase : public PtrBase<T>{
	//VARIABLES
protected:
	Counter* ref = nullptr;

	//FUNCTIONS	
public:
	virtual ~RefPtrBase() = default;

	virtual bool isValid() const override;
	
	bool operator==(const SharedPtr<T>& ptr);
	bool operator==(const WeakPtr<T>& ptr);

	bool operator!=(const SharedPtr<T>& ptr);
	bool operator!=(const WeakPtr<T>& ptr);

	template<typename U> bool operator==(const SharedPtr<U>& ptr);
	template<typename U> bool operator==(const WeakPtr<U>& ptr);

	template<typename U> bool operator!=(const SharedPtr<U>& ptr);
	template<typename U> bool operator!=(const WeakPtr<U>& ptr);
};

template<typename T>
class SharedPtr : public RefPtrBase<T>{
	//FUNCTIONS
public:
	explicit SharedPtr() = default;
	explicit SharedPtr(T* inObject);

	SharedPtr(const SharedPtr<T>& ptr);
	SharedPtr(const WeakPtr<T>& ptr);
	
	template <typename U> SharedPtr(const SharedPtr<U>& ptr);
	template <typename U> SharedPtr(const WeakPtr<U>& ptr);

	~SharedPtr();

	T* operator->() const;
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
	//FUNCTIONS
public:
	UniquePtr() = default;
	UniquePtr(T* inObject);

	~UniquePtr();

	SharedPtr<T>& operator=(T* inObject);
	SharedPtr<T>& operator=(const SharedPtr<T>& ptr);

protected:
	virtual void free() override;

private:
	void clear();
};

#include "Ptr.inl"