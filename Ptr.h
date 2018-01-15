#pragma once

#include <list>
#include "PtrManager.h"

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
template<typename T> class SharedPtr;
template<typename T> class WeakPtr;

template<typename T>
class BasePtr{
	friend class PtrManager;

	template<typename T> friend class SharedPtr;
	template<typename U> friend class SharedPtr;

	template<typename T> friend class WeakPtr;
	template<typename U> friend class WeakPtr;

	//VARIABLES
protected:
	T *object		= nullptr;
	Counter *ref	= nullptr;

	//FUNCTIONS	
public:
	virtual ~BasePtr() = default;

	T *get();
	bool isValid() const;

	bool operator==(const T *inObject);
	bool operator==(const SharedPtr<T> &ptr);
	bool operator==(const WeakPtr<T> &ptr);

	bool operator!=(const T *inObject);
	bool operator!=(const SharedPtr<T> &ptr);
	bool operator!=(const WeakPtr<T> &ptr);

	template<typename U> bool operator==(const SharedPtr<U> &ptr);
	template<typename U> bool operator==(const WeakPtr<U> &ptr);

	template<typename U> bool operator!=(const SharedPtr<U> &ptr);
	template<typename U> bool operator!=(const WeakPtr<U> &ptr);

	explicit operator bool() const;
};

template<typename T>
class SharedPtr : public BasePtr<T>{
	friend class PtrManager;

	template<typename T> friend class BasePtr;
	template<typename U> friend class BasePtr;

	template<typename T> friend class WeakPtr;
	template<typename U> friend class WeakPtr;
	
	template<typename U> friend class SharedPtr;

	//FUNCTIONS
public:
	explicit SharedPtr();
	explicit SharedPtr(T *inObject);

	SharedPtr(const SharedPtr<T> &ptr);
	SharedPtr(const WeakPtr<T> &ptr);
	
	template <typename U> SharedPtr(const SharedPtr<U> &ptr);
	template <typename U> SharedPtr(const WeakPtr<U> &ptr);

	~SharedPtr();

	T *operator->() const;
	T &operator*() const;

	SharedPtr<T> &operator=(T *inObject);
	SharedPtr<T> &operator=(const SharedPtr<T> &ptr);
	SharedPtr<T> &operator=(const WeakPtr<T> &ptr);

private:
	void init(T *inObject, Counter *inRef = nullptr);
	void free();
};

template<typename T>
class WeakPtr : public  BasePtr<T>{
	friend class PtrManager;

	template<typename T> friend class BasePtr;
	template<typename U> friend class BasePtr;

	template<typename T> friend class SharedPtr;
	template<typename U> friend class SharedPtr;

	template<typename U> friend class WeakPtr;

	//FUNCTIONS
public:
	explicit WeakPtr();

	WeakPtr(const WeakPtr<T> &ptr);
	WeakPtr(const SharedPtr<T> &ptr);
	
	template<typename U> WeakPtr(const WeakPtr<U> &ptr);
	template<typename U> WeakPtr(const SharedPtr<U> &ptr);

	~WeakPtr();

	SharedPtr<T> pin();

	WeakPtr<T> &operator=(const WeakPtr<T> &ptr);
	WeakPtr<T> &operator=(const SharedPtr<T> &ptr);

private:
	void init(T *inObject, Counter *inRef = nullptr);
	void free();
};

#include "Ptr.inl"