/////////POINTER BASE
template<typename Type, typename DeleterType>
inline Type* agm::PtrBase<Type, DeleterType>::get() const{
	return isValid() ? object : nullptr;
}

template<typename Type, typename DeleterType>
inline bool agm::PtrBase<Type, DeleterType>::isValid() const{
	return object != nullptr;
}

template<typename Type, typename DeleterType>
inline void agm::PtrBase<Type, DeleterType>::reset(){
	free();
}

template<typename Type, typename DeleterType>
inline agm::PtrBase<Type, DeleterType>::operator bool() const{
	return isValid();
}

/////////REFERENCE POINTER BASE
template<typename Type, typename DeleterType>
inline bool agm::RefPtrBase<Type, DeleterType>::isValid() const{
	return (ref && ref->check() > 0) ? object != nullptr : false;
}

/////////SHARED POINTER
template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(Type* inObject){
	if(inObject){
		init(inObject);
	}
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(const agm::SharedPtr<Type, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(const agm::SharedPtr<Type, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(const agm::WeakPtr<Type, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(const agm::WeakPtr<Type, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(const agm::SharedPtr<OtherType, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(const agm::SharedPtr<OtherType, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(const agm::WeakPtr<OtherType, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(const agm::WeakPtr<OtherType, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}


template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::SharedPtr<Type, DeleterType>::SharedPtr(const agm::SharedPtr<OtherType, DeleterType>& ptr, Type* obj){
	if(ptr.isValid() && obj){
		init(obj, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>::~SharedPtr(){
	free();
}

template<typename Type, typename DeleterType>
inline Type* agm::SharedPtr<Type, DeleterType>::operator ->(){
	return get();
}

template<typename Type, typename DeleterType>
inline Type* agm::SharedPtr<Type, DeleterType>::operator ->() const{
	return get();
}

template<typename Type, typename DeleterType>
inline Type& agm::SharedPtr<Type, DeleterType>::operator *(){
	return *get();
}

template<typename Type, typename DeleterType>
inline Type& agm::SharedPtr<Type, DeleterType>::operator *() const{
	return *get();
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>& agm::SharedPtr<Type, DeleterType>::operator =(Type* inObject){
	if(object != inObject){
		free();
		if(inObject){
			init(inObject);
		}
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>& agm::SharedPtr<Type, DeleterType>::operator =(const agm::SharedPtr<Type, DeleterType>& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>& agm::SharedPtr<Type, DeleterType>::operator =(const agm::SharedPtr<Type, DeleterType>&& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>& agm::SharedPtr<Type, DeleterType>::operator =(const agm::WeakPtr<Type, DeleterType>& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType>& agm::SharedPtr<Type, DeleterType>::operator =(const agm::WeakPtr<Type, DeleterType>&& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline void agm::SharedPtr<Type, DeleterType>::free(){
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
inline void agm::SharedPtr<Type, DeleterType>::init(Type* inObject, Counter* inRef){
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
inline agm::WeakPtr<Type, DeleterType>::WeakPtr(const agm::WeakPtr<Type, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline agm::WeakPtr<Type, DeleterType>::WeakPtr(const agm::WeakPtr<Type, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline agm::WeakPtr<Type, DeleterType>::WeakPtr(const agm::SharedPtr<Type, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline agm::WeakPtr<Type, DeleterType>::WeakPtr(const agm::SharedPtr<Type, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::WeakPtr<Type, DeleterType>::WeakPtr(const agm::WeakPtr<OtherType, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::WeakPtr<Type, DeleterType>::WeakPtr(const agm::WeakPtr<OtherType, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::WeakPtr<Type, DeleterType>::WeakPtr(const agm::SharedPtr<OtherType, DeleterType>& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::WeakPtr<Type, DeleterType>::WeakPtr(const agm::SharedPtr<OtherType, DeleterType>&& ptr){
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
}

template<typename Type, typename DeleterType>
inline agm::WeakPtr<Type, DeleterType>::~WeakPtr(){
	free();
}

template<typename Type, typename DeleterType>
inline agm::SharedPtr<Type, DeleterType> agm::WeakPtr<Type, DeleterType>::pin(){
	return SharedPtr<Type, DeleterType>(*this);
}

template<typename Type, typename DeleterType>
inline agm::WeakPtr<Type, DeleterType>& agm::WeakPtr<Type, DeleterType>::operator =(const agm::WeakPtr<Type, DeleterType>& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline agm::WeakPtr<Type, DeleterType>& agm::WeakPtr<Type, DeleterType>::operator =(const agm::WeakPtr<Type, DeleterType>&& ptr){
	if(this != &ptr){
		free();
		if(ptr.isValid()){
			init(ptr.object, ptr.ref);
		}
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline agm::WeakPtr<Type, DeleterType>& agm::WeakPtr<Type, DeleterType>::operator =(const agm::SharedPtr<Type, DeleterType>& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline agm::WeakPtr<Type, DeleterType>& agm::WeakPtr<Type, DeleterType>::operator =(const agm::SharedPtr<Type, DeleterType>&& ptr){
	free();
	if(ptr.isValid()){
		init(ptr.object, ptr.ref);
	}
	return *this;
}

template<typename Type, typename DeleterType>
inline void agm::WeakPtr<Type, DeleterType>::free(){
	if(ref && ref->weakRelease() == 0 && ref->fullCheck() == 0){
		delete ref;
	}
	object = nullptr;
	ref = nullptr;
}

template<typename Type, typename DeleterType>
inline void agm::WeakPtr<Type, DeleterType>::init(Type* inObject, Counter* inRef){
	object = inObject;
	if(inRef){
		ref = inRef;
		ref->weakGrab();
	}
}

/////////SHARED FROM THIS
template<typename Type>
inline agm::WeakPtr<Type> agm::SharedFromThis<Type>::getWeakThis() const{
	return weakThis;
}

template<typename Type>
inline agm::SharedPtr<Type> agm::SharedFromThis<Type>::getSharedThis() const{
	return SharedPtr<Type>(getWeakThis());
}

template<typename Type>
template<typename OtherType>
inline agm::WeakPtr<OtherType> agm::SharedFromThis<Type>::getWeakThis() const{
	return staticCast<OtherType, Type>(weakThis);
}

template<typename Type>
template<typename OtherType>
inline agm::SharedPtr<OtherType> agm::SharedFromThis<Type>::getSharedThis() const{
	return SharedPtr<OtherType>(getWeakThis<OtherType>());
}

namespace agm{
	template<typename Type>
	inline void enable(typename Type::SharedFromThisType* ptr, agm::SharedPtr<Type>* shptr){
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
inline void agm::SharedFromThis<Type>::doEnable(Type* ptr, agm::SharedPtr<PtrType>* shptr){
	if(ptr && shptr){
		ptr->weakThis.init(shptr->get(), shptr->ref);
	}
}

/////////UNIQUE POINTER
template<typename Type, typename DeleterType>
inline agm::UniquePtr<Type, DeleterType>::UniquePtr(Type* inObject){
	object = inObject;
}

template<typename Type, typename DeleterType>
inline agm::UniquePtr<Type, DeleterType>::UniquePtr(agm::UniquePtr<Type, DeleterType>&& ptr){
	object = ptr.object;
	ptr.object = nullptr;
}

template<typename Type, typename DeleterType>
template<typename OtherType>
inline agm::UniquePtr<Type, DeleterType>::UniquePtr(agm::UniquePtr<OtherType, DeleterType>&& ptr){
	object = ptr.object;
	ptr.object = nullptr;
}

template<typename Type, typename DeleterType>
inline agm::UniquePtr<Type, DeleterType>::~UniquePtr(){
	free();
}

template<typename Type, typename DeleterType>
inline agm::UniquePtr<Type, DeleterType> agm::UniquePtr<Type, DeleterType>::move(){
	UniquePtr<Type, DeleterType> out(object);
	object = nullptr;
	return out;
}

template<typename Type, typename DeleterType>
inline Type* agm::UniquePtr<Type, DeleterType>::operator ->(){
	return get();
}

template<typename Type, typename DeleterType>
inline Type* agm::UniquePtr<Type, DeleterType>::operator ->() const{
	return get();
}

template<typename Type, typename DeleterType>
inline Type& agm::UniquePtr<Type, DeleterType>::operator *(){
	return *get();
}

template<typename Type, typename DeleterType>
inline Type& agm::UniquePtr<Type, DeleterType>::operator *() const{
	return *get();
}

template<typename Type, typename DeleterType>
inline agm::UniquePtr<Type, DeleterType>& agm::UniquePtr<Type, DeleterType>::operator =(agm::UniquePtr<Type, DeleterType>&& ptr){
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
inline void agm::UniquePtr<Type, DeleterType>::free(){
	if(isValid()){
		deleter(get());
	}
	object = nullptr;
}

/////////HELPER FUNCTIONS
template<typename Type>
agm::UniquePtr<Type> agm::makeUnique(Type* object){
	return UniquePtr<Type>(object);
}

template<typename Type>
agm::SharedPtr<Type> agm::makeShared(Type* object){
	return SharedPtr<Type>(object);
}

template<typename ReturnType, typename CurrentType>
agm::SharedPtr<ReturnType> agm::staticCast(const agm::SharedPtr<CurrentType>& ptr){
	ReturnType* otherObj = static_cast<ReturnType*>(ptr.get());
	SharedPtr<ReturnType> outPtr(ptr, otherObj);
	return outPtr;
}

template<typename ReturnType, typename CurrentType>
agm::SharedPtr<ReturnType> agm::dynamicCast(const agm::SharedPtr<CurrentType>& ptr){
	if(ReturnType* otherObj = dynamic_cast<ReturnType*>(ptr.get())){
		return SharedPtr<ReturnType>(ptr, otherObj);
	}
	return SharedPtr<ReturnType>();
}

template<typename ReturnType, typename CurrentType>
agm::SharedPtr<ReturnType> agm::constCast(const agm::SharedPtr<CurrentType>& ptr){
	ReturnType* otherObj = const_cast<ReturnType*>(ptr.get());
	SharedPtr<ReturnType> outPtr(ptr, otherObj);
	return outPtr;
}

template<typename ReturnType, typename CurrentType>
agm::SharedPtr<ReturnType> agm::reinterpretCast(const agm::SharedPtr<CurrentType>& ptr){
	ReturnType* otherObj = reinterpret_cast<ReturnType*>(ptr.get());
	SharedPtr<ReturnType> outPtr(ptr, otherObj);
	return outPtr;
}
