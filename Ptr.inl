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
	return SharedPtr<T>(getWeakThis());
}

template<typename T>
template<typename U>
inline WeakPtr<U> SharedFromThis<T>::getWeakThis() const{
	WeakPtr<U> outPtr = ptr::weakStaticCast(weakThis);
	return outPtr;
}

template<typename T>
template<typename U>
inline SharedPtr<U> SharedFromThis<T>::getSharedThis() const{
	return SharedPtr<U>(getWeakThis<U>());
}

template<typename T>
inline void enable(typename T::shT* ptr, SharedPtr<T>* shptr){
	if(ptr){
		ptr->doEnable(ptr, shptr);
	}
}

inline void enable(const volatile void* Ptr, const volatile void* shptr){
	//Not of type SharedFromThis - do nothing
}

template<typename T>
template<typename U>
inline void SharedFromThis<T>::doEnable(T* ptr, SharedPtr<U>* shptr){
	if(ptr && shptr){
		ptr->weakThis.init(shptr->get(), shptr->ref);
	}
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
template<typename U>
inline UniquePtr<T, D>::UniquePtr(UniquePtr<U, D>&& ptr){
	object = ptr.object;
	ptr.object = nullptr;
}

template<typename T, typename D>
inline UniquePtr<T, D>::~UniquePtr() {
	free();
}

template<typename T, typename D>
inline UniquePtr<T, D> UniquePtr<T, D>::move(){
	UniquePtr<T, D> out(object);
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
inline UniquePtr<T, D>& UniquePtr<T, D>::operator=(UniquePtr<T, D>&& ptr){
	if(this != &ptr){
		if(isValid()){
			free();
		}
		object = ptr.object;
		ptr.object = nullptr;
	}
	return *this;
}

template<typename T, typename D>
inline void UniquePtr<T, D>::free() {
	if(isValid()){
		deleter(get());
	}
	object = nullptr;
}

///////HELPER FUNCTIONS
template<typename T>
UniquePtr<T> ptr::makeUnique(T* object){
	return UniquePtr<T>(object);
}

template<typename T>
SharedPtr<T>  ptr::makeShared(T* object){
	return SharedPtr<T>(object);
}

template<typename T, typename U>
UniquePtr<T> ptr::staticCast(UniquePtr<U>& otherPtr){
	T* otherObj = static_cast<T*>(other.object);
	UniquePtr<T> outPtr;
	outPtr = makeUnique(otherObj);
	otherPtr.object = nullptr;
	return outPtr;
}

template<typename T, typename U>
SharedPtr<T> ptr::staticCast(const SharedPtr<U>& otherPtr){
	T* otherObj = static_cast<T*>(otherPtr.object);
	SharedPtr<T> outPtr;
	outPtr.init(otherObj, otherPtr.ref);
	return outPtr;
}

template<typename T, typename U>
WeakPtr<T> ptr::staticCast(const WeakPtr<U>& otherPtr){
	T* otherObj = static_cast<T*>(otherPtr.object);
	WeakPtr<T> outPtr;
	outPtr.init(otherObj, otherPtr.ref);
	return outPtr;
}

template<typename T, typename U>
UniquePtr<T> ptr::dynamicCast(UniquePtr<U>& otherPtr){
	UniquePtr<T> outPtr;
	if(T* otherObj = dynamic_cast<T*>(other.object)){
		outPtr = makeUnique(otherObj);
		otherPtr.object = nullptr;
	}
	return outPtr;
}

template<typename T, typename U>
SharedPtr<T> ptr::dynamicCast(const SharedPtr<U>& otherPtr){
	SharedPtr<T> outPtr;
	if(T* otherObj = dynamic_cast<T*>(otherPtr.object)){
		outPtr.init(otherObj, otherPtr.ref);
	}
	return outPtr;
}

template<typename T, typename U>
WeakPtr<T> ptr::dynamicCast(const WeakPtr<U>& otherPtr){
	WeakPtr<T> outPtr;
	if(T* otherObj = dynamic_cast<T*>(otherPtr.object)){
		outPtr.init(otherObj, otherPtr.ref);
	}
	return outPtr;
}
