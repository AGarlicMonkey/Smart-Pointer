#include "Ptr.h"
///////BASE
template<typename T>
inline T* PtrBase<T>::get() const{
	return isValid() ? object : nullptr;
}

template<typename T>
inline bool PtrBase<T>::isValid() const{
	return object != nullptr;
}

template<typename T>
inline void PtrBase<T>::reset(){
	free();
}

template<typename T>
inline PtrBase<T>::operator bool() const{
	return isValid();
}

/////////REFBASE 
template<typename T>
inline bool RefPtrBase<T>::isValid() const{
	return ref ? ref->check() > 0 ? object != nullptr : false : false;
}

/////////SHARED
template<typename T>
inline SharedPtr<T>::SharedPtr(T* inObject){
	if(inObject){
		init(inObject);
	}
}

template<typename T>
inline SharedPtr<T>::SharedPtr(const SharedPtr<T>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T>
inline SharedPtr<T>::SharedPtr(const WeakPtr<T>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T>
template<typename U>
inline SharedPtr<T>::SharedPtr(const SharedPtr<U>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T>
template<typename U>
inline SharedPtr<T>::SharedPtr(const WeakPtr<U>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T>
inline SharedPtr<T>::~SharedPtr(){
	free();
}

template<typename T>
inline T* SharedPtr<T>::operator->() const{
	return get();
}

template<typename T>
inline T& SharedPtr<T>::operator*() const{
	return *get();
}

template<typename T>
inline SharedPtr<T>& SharedPtr<T>::operator=(T* inObject){
	if(object != inObject){
		free(); 
		if(inObject){
			init(inObject);
		}
	}
	return *this;
}

template<typename T>
inline SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T>& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename T>
inline SharedPtr<T>& SharedPtr<T>::operator=(const WeakPtr<T>& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename T>
inline void SharedPtr<T>::free(){
	if(ref && ref->release() == 0){
		if(ref->fullCheck() == 0){
			delete ref;
		}
		delete object;
	}
	object = nullptr;
	ref = nullptr;
}

template<typename T>
inline void SharedPtr<T>::init(T* inObject, Counter* inRef){
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

/////////WEAK
template<typename T>
inline WeakPtr<T>::WeakPtr(const WeakPtr<T>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T>
inline WeakPtr<T>::WeakPtr(const SharedPtr<T>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T>
template<typename U>
inline WeakPtr<T>::WeakPtr(const WeakPtr<U>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T>
template<typename U>
inline WeakPtr<T>::WeakPtr(const SharedPtr<U>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T>
inline WeakPtr<T>::~WeakPtr(){
	free();
}

template<typename T>
inline SharedPtr<T> WeakPtr<T>::pin(){
	return SharedPtr<T>(*this);
}

template<typename T>
inline WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<T>& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename T>
inline WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename T>
inline void WeakPtr<T>::free(){
	if(ref && ref->weakRelease() == 0 && ref->fullCheck() == 0){
		delete ref;
	}
	object = nullptr;
	ref = nullptr;
}

template<typename T>
inline void WeakPtr<T>::init(T* inObject, Counter* inRef){
	object = inObject;
	if(inRef){
		ref = inRef;
		ref->weakGrab();
	}
}

/////////SharedFromThis
template<typename T>
inline WeakPtr<T> SharedFromThis<T>::getWeakThis() const{
	return weakThis;
}

template<typename T>
inline SharedPtr<T> SharedFromThis<T>::getSharedThis() const{
	return SharedPtr<T>(weakThis);
}

template<typename T, typename U>
inline void enable(SharedFromThis<T>* ptr, SharedPtr<U>* shptr){
	if(ptr){
		ptr->doEnable(shptr);
	}
}

inline void enable(const volatile void* Ptr, const volatile void* shptr){
	//Not of type SharedFromThis - do nothing
}

template<typename T>
inline void SharedFromThis<T>::doEnable(SharedPtr<T>* shptr){
	weakThis = *shptr;
}

///////UNIQUE
template<typename T>
inline UniquePtr<T>::UniquePtr(T* inObject){
	object = inObject;
}

template<typename T>
inline UniquePtr<T>::UniquePtr(UniquePtr<T>& ptr){
	if(ptr.isValid()){
		object = ptr.object;
		ptr.clear();
	}
}

template<typename T>
inline UniquePtr<T>::~UniquePtr(){
	free();
}

template<typename T>
inline UniquePtr<T>& UniquePtr<T>::operator=(T* inObject){
	object = inObject;
}

template<typename T>
inline UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr<T>& ptr){
	if(ptr.isValid()){
		object = ptr.object;
		ptr.clear();
	}
}

template<typename T>
inline void UniquePtr<T>::free(){
	if(isValid()){
		delete object;
	}
	clear();
}

template<typename T>
inline void UniquePtr<T>::clear(){
	object = nullptr;
}
