//BASE
template<typename T>
inline T *BasePtr<T>::get(){
	return isValid() ? object : nullptr;
}

template<typename T>
inline bool BasePtr<T>::isValid() const{
	return ref ? ref->check() > 0 ? object != nullptr : false : false;
}

template<typename T>
inline bool BasePtr<T>::operator==(const T *inObject){
	return object == inObject;
}

template<typename T>
inline bool BasePtr<T>::operator==(const SharedPtr<T> &ptr){
	return object == ptr.object;
}

template<typename T>
inline bool BasePtr<T>::operator==(const WeakPtr<T> &ptr){
	return object == ptr.object;
}

template<typename T>
inline bool BasePtr<T>::operator!=(const T *inObject){
	return object != inObject;
}

template<typename T>
inline bool BasePtr<T>::operator!=(const SharedPtr<T> &ptr){
	return object != ptr.object;
}

template<typename T>
inline bool BasePtr<T>::operator!=(const WeakPtr<T> &ptr){
	return object != ptr.object;
}

template<typename T>
template<typename U>
inline bool BasePtr<T>::operator==(const SharedPtr<U> &ptr){
	return object == ptr.object;
}

template<typename T>
template<typename U>
inline bool BasePtr<T>::operator==(const WeakPtr<U> &ptr){
	return object == ptr.object;
}

template<typename T>
template<typename U>
inline bool BasePtr<T>::operator!=(const SharedPtr<U> &ptr){
	return object != ptr.object;
}

template<typename T>
template<typename U>
inline bool BasePtr<T>::operator!=(const WeakPtr<U> &ptr){
	return object != ptr.object;
}

template<typename T>
inline BasePtr<T>::operator bool() const{
	return isValid();
}

//SHARED
template<typename T>
inline SharedPtr<T>::SharedPtr(){
	init(nullptr);
}

template<typename T>
inline SharedPtr<T>::SharedPtr(T *inObject){
	init(inObject);
}

template<typename T>
inline SharedPtr<T>::SharedPtr(const SharedPtr<T> &ptr){
	init(ptr.object, ptr.ref);
}

template<typename T>
inline SharedPtr<T>::SharedPtr(const WeakPtr<T>& ptr){
	init(ptr.object, ptr.ref);
}

template<typename T>
template<typename U>
inline SharedPtr<T>::SharedPtr(const SharedPtr<U> &ptr){
	init(ptr.object, ptr.ref);
}

template<typename T>
template<typename U>
inline SharedPtr<T>::SharedPtr(const WeakPtr<U>& ptr){
	init(ptr.object, ptr.ref);
}

template<typename T>
inline SharedPtr<T>::~SharedPtr(){
	free();
}

template<typename T>
inline T *SharedPtr<T>::operator->() const{
	return object;
}

template<typename T>
inline T &SharedPtr<T>::operator*() const{
	return *object;
}

template<typename T>
inline SharedPtr<T> &SharedPtr<T>::operator=(T *inObject){
	if(object != inObject){
		free();
		init(inObject);
	}
	return *this;
}

template<typename T>
inline SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr<T> &ptr){
	if(this != &ptr){
		free();
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename T>
inline SharedPtr<T>& SharedPtr<T>::operator=(const WeakPtr<T>& ptr){
	free();
	init(ptr.object, ptr.ref);
	return *this;
}

template<typename T>
inline void SharedPtr<T>::init(T *inObject, Counter *inRef){
	object = inObject;
	if(inRef){
		ref = inRef;
	} else{
		ref = new Counter();
		enable<T>(object, this);
	}
	ref->grab();
}

template<typename T>
inline void SharedPtr<T>::free(){
	if(ref->release() == 0){
		if(ref->fullCheck() == 0){
			delete ref;
		}
		delete object;
	}
}

//WEAK
template<typename T>
inline WeakPtr<T>::WeakPtr(){
}

template<typename T>
inline WeakPtr<T>::WeakPtr(const WeakPtr<T> &ptr){
	init(ptr.object, ptr.ref);
}

template<typename T>
inline WeakPtr<T>::WeakPtr(const SharedPtr<T> &ptr){
	init(ptr.object, ptr.ref);
}

template<typename T>
template<typename U>
inline WeakPtr<T>::WeakPtr(const WeakPtr<U> &ptr){
	init(ptr.object, ptr.ref);
}

template<typename T>
template<typename U>
inline WeakPtr<T>::WeakPtr(const SharedPtr<U> &ptr){
	init(ptr.object, ptr.ref);
}

template<typename T>
inline WeakPtr<T>::~WeakPtr(){
	free();
}

template<typename T>
inline SharedPtr<T> WeakPtr<T>::pin(){
	return SharedPtr<T>(this);
}

template<typename T>
inline WeakPtr<T> &WeakPtr<T>::operator=(const WeakPtr<T>& ptr){
	if(this != &ptr){
		free();
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename T>
inline WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& ptr){
	free();
	init(ptr.object, ptr.ref);
	return *this;
}

template<typename T>
inline void WeakPtr<T>::init(T *inObject, Counter *inRef){
	object = inObject;
	if(inRef){
		ref = inRef;
		ref->weakGrab();
	}
}

template<typename T>
inline void WeakPtr<T>::free(){
	//Ref can be nullptr on WeakPtr
	if(ref){
		ref->weakRelease();
		if(ref->fullCheck() == 0){
			delete ref;
		}
	}
}

//SHARED FROM THIS
template<typename T>
inline SharedPtr<T> SharedFromThis<T>::getSharedThis(){
	if(!weakThis.isValid()){
		weakThis = SharedPtr<T>(this);
		return weakThis;
	} else{
		return weakThis;
	}
}

template<typename T>
inline WeakPtr<T> SharedFromThis<T>::getWeakThis(){
	return weakThis;
}

template<typename T>
void enable(T *ptr, SharedPtr<T> *shPtr){
	if(ptr){
		ptr->weakThis(*shPtr);
	}
}