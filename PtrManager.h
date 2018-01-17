#pragma once

#include <list>

template<typename T> class BasePtr;
class Counter;

struct TypeRegistrationInfo{
	void* type = nullptr;
	Counter* counter = nullptr;

	TypeRegistrationInfo() = default;
	TypeRegistrationInfo(void* inType, Counter* inCounter){
		type = inType;
		counter = inCounter;
	}

	bool operator==(const TypeRegistrationInfo &other){
		return other.type == type && other.counter == counter;
	}
};

class PtrManager{
	//VARIABLES
private:
	std::list<TypeRegistrationInfo> registeredTypes;

	//FUNCTIONS
public:
	PtrManager() = default;
	~PtrManager() = default;

	static PtrManager* get();

	Counter* registerType(void* type);
	void unregisterType(void* type);

	int getRegisteredTypeCount() const{	return registeredTypes.size(); }
};