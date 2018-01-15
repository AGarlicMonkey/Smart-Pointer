#include "PtrManager.h"
#include "Ptr.h"

PtrManager::PtrManager(){

}

PtrManager::~PtrManager(){

}

PtrManager *PtrManager::get(){
	static PtrManager *instance = nullptr;
	if(!instance){
		instance = new PtrManager();
	}
	return instance;
}

Counter *PtrManager::registerType(void *type){
	Counter *counter = nullptr;

	for(const TypeRegistrationInfo &info : registeredTypes){
		if(info.type == type){
			counter = info.counter;
			break;
		}
	}

	if(!counter){
		counter = new Counter();

		TypeRegistrationInfo info;
		info.type = type;
		info.counter = counter;

		registeredTypes.push_back(info);
	}


	return counter;
}

void PtrManager::unregisterType(void *type){
	TypeRegistrationInfo *registeredInfo = nullptr;
	for(TypeRegistrationInfo &info : registeredTypes){
		if(info.type == type){
			registeredInfo = &info;
			break;
		}
	}
	if(registeredInfo && registeredInfo->counter && registeredInfo->counter->fullCheck() == 0){
		registeredTypes.remove(*registeredInfo);
	}
}
