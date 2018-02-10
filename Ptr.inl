///////BASE
template<typename T, typename D>
inline T* PtrBase<T, D>::get() const{
	return isValid() ? object : nullptr;
}

template<typename T, typename D>
inline bool PtrBase<T, D>::isValid() const{
	return object != nullptr;
}

template<typename T, typename D>
inline void PtrBase<T, D>::reset(){
	free();
}

template<typename T, typename D>
inline PtrBase<T, D>::operator bool() const{
	return isValid();
}

/////////REFBASE 
template<typename T, typename D>
inline bool RefPtrBase<T, D>::isValid() const{
	return ref ? ref->check() > 0 ? object != nullptr : false : false;
}

/////////SHARED
template<typename T, typename D>
inline SharedPtr<T, D>::SharedPtr(T* inObject){
	if(inObject){
		init(inObject);
	}
}

template<typename T, typename D>
inline SharedPtr<T, D>::SharedPtr(const SharedPtr<T, D>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T, typename D>
inline SharedPtr<T, D>::SharedPtr(const WeakPtr<T, D>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T, typename D>
template<typename U>
inline SharedPtr<T, D>::SharedPtr(const SharedPtr<U, D>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T, typename D>
template<typename U>
inline SharedPtr<T, D>::SharedPtr(const WeakPtr<U, D>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T, typename D>
inline SharedPtr<T, D>::~SharedPtr(){
	free();
}

template<typename T, typename D>
inline T* SharedPtr<T, D>::operator->(){
	return get();
}

template<typename T, typename D>
inline T* SharedPtr<T, D>::operator->() const{
	return get();
}

template<typename T, typename D>
inline T& SharedPtr<T, D>::operator*(){
	return *get();
}

template<typename T, typename D>
inline T& SharedPtr<T, D>::operator*() const{
	return *get();
}

template<typename T, typename D>
inline SharedPtr<T, D>& SharedPtr<T, D>::operator=(T* inObject){
	if(object != inObject){
		free(); 
		if(inObject){
			init(inObject);
		}
	}
	return *this;
}

template<typename T, typename D>
inline SharedPtr<T, D>& SharedPtr<T, D>::operator=(const SharedPtr<T, D>& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename T, typename D>
inline SharedPtr<T, D>& SharedPtr<T, D>::operator=(const WeakPtr<T, D>& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename T, typename D>
inline void SharedPtr<T, D>::free(){
	if(ref && ref->release() == 0){
		deleter(get());
		if(ref->fullCheck() == 0){
			delete ref;
		}
	}
	object = nullptr;
	ref = nullptr;
}

template<typename T, typename D>
inline void SharedPtr<T, D>::init(T* inObject, Counter* inRef){
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
template<typename T, typename D>
inline WeakPtr<T, D>::WeakPtr(const WeakPtr<T, D>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T, typename D>
inline WeakPtr<T, D>::WeakPtr(const SharedPtr<T, D>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T, typename D>
template<typename U>
inline WeakPtr<T, D>::WeakPtr(const WeakPtr<U, D>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T, typename D>
template<typename U>
inline WeakPtr<T, D>::WeakPtr(const SharedPtr<U, D>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename T, typename D>
inline WeakPtr<T, D>::~WeakPtr(){
	free();
}

template<typename T, typename D>
inline SharedPtr<T, D> WeakPtr<T, D>::pin(){
	return SharedPtr<T, D>(*this);
}

template<typename T, typename D>
inline WeakPtr<T, D>& WeakPtr<T, D>::operator=(const WeakPtr<T, D>& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename T, typename D>
inline WeakPtr<T, D>& WeakPtr<T, D>::operator=(const SharedPtr<T, D>& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename T, typename D>
inline void WeakPtr<T, D>::free(){
	if(ref && ref->weakRelease() == 0 && ref->fullCheck() == 0){
		delete ref;
	}
	object = nullptr;
	ref = nullptr;
}

template<typename T, typename D>
inline void WeakPtr<T, D>::init(T* inObject, Counter* inRef){
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
template<typename T, typename D>
inline UniquePtr<T, D>::UniquePtr(T* inObject){
	object = inObject;
}

template<typename T, typename D>
inline UniquePtr<T, D>::UniquePtr(UniquePtr<T, D>&& ptr){
	object = ptr.object;
	ptr.object = nullptr;
}

template<typename T, typename D>
inline UniquePtr<T, D>::~UniquePtr(){
	free();
}

template<typename T, typename D>
inline UniquePtr<T, D> UniquePtr<T, D>::move(){
	UniquePtr<T, D> out(object);
	object = nullptr;
	return out;
}

template<typename T, typename D>
template<typename U>
inline UniquePtr<U, D> UniquePtr<T, D>::move(){
	UniquePtr<U, D> out(object);
	object = nullptr;
	return out;
}

template<typename T, typename D>
inline T* UniquePtr<T, D>::operator->(){
	return get();
}

template<typename T, typename D>
inline T* UniquePtr<T, D>::operator->() const{
	return get();
}

template<typename T, typename D>
inline T& UniquePtr<T, D>::operator*(){
	return *get();
}

template<typename T, typename D>
inline T& UniquePtr<T, D>::operator*() const{
	return *get();
}

template<typename T, typename D>
inline void UniquePtr<T, D>::free(){
	if(isValid()){
		deleter(get());
	}
	object = nullptr;
}