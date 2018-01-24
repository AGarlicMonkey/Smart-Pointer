/////////BASE
template<typename T>
inline T* BasePtr<T>::get() const{
	return isValid() ? object : nullptr;
}

template<typename T>
inline bool BasePtr<T>::isValid() const{
	return ref ? ref->check() > 0 ? object != nullptr : false : false;
}

template<typename T>
inline void BasePtr<T>::reset(){
	free();
}

template<typename T>
inline bool BasePtr<T>::operator==(const T* inObject){
	return get() == inObject;
}

template<typename T>
inline bool BasePtr<T>::operator==(const SharedPtr<T>& ptr){
	return get() == ptr.object;
}

template<typename T>
inline bool BasePtr<T>::operator==(const WeakPtr<T>& ptr){
	return get() == ptr.object;
}

template<typename T>
inline bool BasePtr<T>::operator!=(const T* inObject){
	return get() != inObject;
}

template<typename T>
inline bool BasePtr<T>::operator!=(const SharedPtr<T>& ptr){
	return get() != ptr.object;
}

template<typename T>
inline bool BasePtr<T>::operator!=(const WeakPtr<T>& ptr){
	return get() != ptr.object;
}

template<typename T>
template<typename U>
inline bool BasePtr<T>::operator==(const SharedPtr<U>& ptr){
	return get() == ptr.object;
}

template<typename T>
template<typename U>
inline bool BasePtr<T>::operator==(const WeakPtr<U>& ptr){
	return get() == ptr.object;
}

template<typename T>
template<typename U>
inline bool BasePtr<T>::operator!=(const SharedPtr<U>& ptr){
	return get() != ptr.object;
}

template<typename T>
template<typename U>
inline bool BasePtr<T>::operator!=(const WeakPtr<U>& ptr){
	return get() != ptr.object;
}

template<typename T>
inline BasePtr<T>::operator bool() const{
	return isValid();
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
			if(PtrManager* manager = PtrManager::get()){
				manager->unregisterType(static_cast<void*>(object));
			}
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
	} else{
		if(PtrManager* manager = PtrManager::get()){
			ref = manager->registerType(static_cast<void*>(object));
		}
	}
	ref->grab();
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
	return SharedPtr<T>(this);
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
	if(ref && ref->weakRelease() == 0){
		if(ref->fullCheck() == 0){
			if(PtrManager* manager = PtrManager::get()){
				manager->unregisterType(static_cast<void*>(object));
			}
			delete ref;
		}
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