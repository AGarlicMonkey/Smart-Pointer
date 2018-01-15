#pragma once

#include <list>

template<typename T> class BasePtr;
class Counter;

struct TypeRegistrationInfo{
	void *type = nullptr;
	Counter *counter = nullptr;

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
	PtrManager();
	~PtrManager();

	static PtrManager *get();

	Counter *registerType(void *type);
	void unregisterType(void *type);

	int getRegisteredTypeCount() const{	return registeredTypes.size(); }
};