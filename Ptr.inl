/////////POINTER BASE
template<typename Type, typename DeleterType>
inline Type* ptr::PtrBase<Type, DeleterType>::get() const{
	return isValid() ? object : nullptr;
}

template<typename Type, typename DeleterType>
inline bool ptr::PtrBase<Type, DeleterType>::isValid() const{
	return object != nullptr;
}

template<typename Type, typename DeleterType>
inline void ptr::PtrBase<Type, DeleterType>::reset(){
	free();
}

template<typename Type, typename DeleterType>
inline ptr::PtrBase<Type, DeleterType>::operator bool() const{
	return isValid();
}

/////////REFERENCE POINTER BASE
template<typename Type, typename DeleterType>
inline bool ptr::RefPtrBase<Type, DeleterType>::isValid() const{
	return (ref && ref->check() > 0) ? object != nullptr : false;
}

/////////SHARED POINTER
template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(Type* inObject){
	if(inObject){
		init(inObject);
	}
}

template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(const ptr::SharedPtr<Type, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(const ptr::SharedPtr<Type, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(const ptr::WeakPtr<Type, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(const ptr::WeakPtr<Type, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(const ptr::SharedPtr<OtherType, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(const ptr::SharedPtr<OtherType, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(const ptr::WeakPtr<OtherType, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(const ptr::WeakPtr<OtherType, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}


template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::SharedPtr<Type, DeleterType>::SharedPtr(const ptr::SharedPtr<OtherType, DeleterType>& ptr, Type* obj){
	if(ptr.isValid() && obj){
		init(obj, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType>::~SharedPtr(){
	free();
}

template<typename Type, typename DeleterType>
inline Type* ptr::SharedPtr<Type, DeleterType>::operator->(){
	return get();
}

template<typename Type, typename DeleterType>
inline Type* ptr::SharedPtr<Type, DeleterType>::operator->() const{
	return get();
}

template<typename Type, typename DeleterType>
inline Type& ptr::SharedPtr<Type, DeleterType>::operator*(){
	return *get();
}

template<typename Type, typename DeleterType>
inline Type& ptr::SharedPtr<Type, DeleterType>::operator*() const{
	return *get();
}

template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType>& ptr::SharedPtr<Type, DeleterType>::operator=(Type* inObject){
	if(object != inObject){
		free();
		if(inObject){
			init(inObject);
		}
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType>& ptr::SharedPtr<Type, DeleterType>::operator=(const ptr::SharedPtr<Type, DeleterType>& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType>& ptr::SharedPtr<Type, DeleterType>::operator=(const ptr::WeakPtr<Type, DeleterType>& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline void ptr::SharedPtr<Type, DeleterType>::free(){
	if(ref && ref->release() == 0){
		deleter(get());
		if(ref->fullCheck() == 0){
			delete ref;
		}
	}
	object = nullptr;
	ref = nullptr;
}

template<typename Type, typename DeleterType>
inline void ptr::SharedPtr<Type, DeleterType>::init(Type* inObject, Counter* inRef){
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

/////////WEAK POINTER
template<typename Type, typename DeleterType>
inline ptr::WeakPtr<Type, DeleterType>::WeakPtr(const ptr::WeakPtr<Type, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline ptr::WeakPtr<Type, DeleterType>::WeakPtr(const ptr::WeakPtr<Type, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline ptr::WeakPtr<Type, DeleterType>::WeakPtr(const ptr::SharedPtr<Type, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline ptr::WeakPtr<Type, DeleterType>::WeakPtr(const ptr::SharedPtr<Type, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::WeakPtr<Type, DeleterType>::WeakPtr(const ptr::WeakPtr<OtherType, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::WeakPtr<Type, DeleterType>::WeakPtr(const ptr::WeakPtr<OtherType, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::WeakPtr<Type, DeleterType>::WeakPtr(const ptr::SharedPtr<OtherType, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::WeakPtr<Type, DeleterType>::WeakPtr(const ptr::SharedPtr<OtherType, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::WeakPtr<Type, DeleterType>::WeakPtr(const ptr::WeakPtr<OtherType, DeleterType>& ptr, Type* obj){
	if(ptr.isValid() && obj){
		init(obj, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline ptr::WeakPtr<Type, DeleterType>::~WeakPtr(){
	free();
}

template<typename Type, typename DeleterType>
inline ptr::SharedPtr<Type, DeleterType> ptr::WeakPtr<Type, DeleterType>::pin(){
	return SharedPtr<Type, DeleterType>(*this);
}

template<typename Type, typename DeleterType>
inline ptr::WeakPtr<Type, DeleterType>& ptr::WeakPtr<Type, DeleterType>::operator=(const ptr::WeakPtr<Type, DeleterType>& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline ptr::WeakPtr<Type, DeleterType>& ptr::WeakPtr<Type, DeleterType>::operator=(const ptr::SharedPtr<Type, DeleterType>& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline void ptr::WeakPtr<Type, DeleterType>::free(){
	if(ref && ref->weakRelease() == 0 && ref->fullCheck() == 0){
		delete ref;
	}
	object = nullptr;
	ref = nullptr;
}

template<typename Type, typename DeleterType>
inline void ptr::WeakPtr<Type, DeleterType>::init(Type* inObject, Counter* inRef){
	object = inObject;
	if(inRef){
		ref = inRef;
		ref->weakGrab();
	}
}

/////////SHARED FROM THIS
template<typename Type>
inline ptr::WeakPtr<Type> ptr::SharedFromThis<Type>::getWeakThis() const{
	return weakThis;
}

template<typename Type>
inline ptr::SharedPtr<Type> ptr::SharedFromThis<Type>::getSharedThis() const{
	return SharedPtr<Type>(getWeakThis());
}

template<typename Type>
template<typename OtherType>
inline ptr::WeakPtr<OtherType> ptr::SharedFromThis<Type>::getWeakThis() const{
	return staticCast<OtherType, Type>(weakThis);
}

template<typename Type>
template<typename OtherType>
inline ptr::SharedPtr<OtherType> ptr::SharedFromThis<Type>::getSharedThis() const{
	return SharedPtr<OtherType>(getWeakThis<OtherType>());
}

namespace ptr{
	template<typename Type>
	inline void enable(typename Type::SharedType* ptr, ptr::SharedPtr<Type>* shptr){
		if(ptr){
			ptr->doEnable(ptr, shptr);
		}
	}

	inline void enable(const void* ptr, const void* shptr){
		//Not of type SharedFromThis - Do nothing
	}
}

template<typename Type>
template<typename PtrType>
inline void ptr::SharedFromThis<Type>::doEnable(Type* ptr, ptr::SharedPtr<PtrType>* shptr){
	if(ptr && shptr){
		ptr->weakThis.init(shptr->get(), shptr->ref);
	}
}

/////////UNIQUE POINTER
template<typename Type, typename DeleterType>
inline ptr::UniquePtr<Type, DeleterType>::UniquePtr(Type* inObject){
	object = inObject;
}

template<typename Type, typename DeleterType>
inline ptr::UniquePtr<Type, DeleterType>::UniquePtr(ptr::UniquePtr<Type, DeleterType>&& ptr){
	object = ptr.object;
	ptr.object = nullptr;
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline ptr::UniquePtr<Type, DeleterType>::UniquePtr(ptr::UniquePtr<OtherType, DeleterType>&& ptr){
	object = ptr.object;
	ptr.object = nullptr;
}

template<typename Type, typename DeleterType>
inline ptr::UniquePtr<Type, DeleterType>::~UniquePtr(){
	free();
}

template<typename Type, typename DeleterType>
inline ptr::UniquePtr<Type, DeleterType> ptr::UniquePtr<Type, DeleterType>::move(){
	UniquePtr<Type, DeleterType> out(object);
	object = nullptr;
	return out;
}

template<typename Type, typename DeleterType>
inline Type* ptr::UniquePtr<Type, DeleterType>::operator->(){
	return get();
}

template<typename Type, typename DeleterType>
inline Type* ptr::UniquePtr<Type, DeleterType>::operator->() const{
	return get();
}

template<typename Type, typename DeleterType>
inline Type& ptr::UniquePtr<Type, DeleterType>::operator*(){
	return *get();
}

template<typename Type, typename DeleterType>
inline Type& ptr::UniquePtr<Type, DeleterType>::operator*() const{
	return *get();
}

template<typename Type, typename DeleterType>
inline ptr::UniquePtr<Type, DeleterType>& ptr::UniquePtr<Type, DeleterType>::operator=(ptr::UniquePtr<Type, DeleterType>&& ptr){
	if(this != &ptr){
		if(isValid()){
			free();
		}
		object = ptr.object;
		ptr.object = nullptr;
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline void ptr::UniquePtr<Type, DeleterType>::free(){
	if(isValid()){
		deleter(get());
	}
	object = nullptr;
}

/////////HELPER FUNCTIONS
template<typename Type>
ptr::UniquePtr<Type> ptr::makeUnique(Type* object){
	return UniquePtr<Type>(object);
}

template<typename Type>
ptr::SharedPtr<Type> ptr::makeShared(Type* object){
	return SharedPtr<Type>(object);
}

template<typename ReturnType, typename CurrentType>
ptr::SharedPtr<ReturnType> ptr::staticCast(const ptr::SharedPtr<CurrentType>& ptr){
	ReturnType* otherObj = static_cast<ReturnType*>(ptr.get());
	SharedPtr<ReturnType> outPtr(ptr, otherObj);
	return outPtr;
}

template<typename ReturnType, typename CurrentType>
ptr::SharedPtr<ReturnType> ptr::dynamicCast(const ptr::SharedPtr<CurrentType>& ptr){
	if(ReturnType* otherObj = dynamic_cast<ReturnType*>(ptr.get())){
		return SharedPtr<ReturnType>(ptr, otherObj);
	}
	return SharedPtr<ReturnType>();
}

template<typename ReturnType, typename CurrentType>
ptr::SharedPtr<ReturnType> ptr::constCast(const ptr::SharedPtr<CurrentType>& ptr){
	ReturnType* otherObj = const_cast<ReturnType*>(ptr.get());
	SharedPtr<ReturnType> outPtr(ptr, otherObj);
	return outPtr;
}

template<typename ReturnType, typename CurrentType>
ptr::SharedPtr<ReturnType> ptr::reinterpretCast(const ptr::SharedPtr<CurrentType>& ptr){
	ReturnType* otherObj = reinterpret_cast<ReturnType*>(ptr.get());
	SharedPtr<ReturnType> outPtr(ptr, otherObj);
	return outPtr;
}
