#include "cpgf/metautility/gmetaobjectarray.h"
#include "cpgf/gmetaapiutil.h"

#include <algorithm>

#include <stdlib.h>


namespace cpgf {

class GMetaObjectArrayImplement
{
public:
	explicit GMetaObjectArrayImplement(IMetaClass * metaClass);
	~GMetaObjectArrayImplement();

	void setObject(uint32_t index, void * object);
	void * getObject(uint32_t index) const;
	
	uint32_t getObjectCount() const;
	const GMetaType & getMetaType() const;
	
	void * getBuffer() const;
	
private:
	void * doGetObject(uint32_t index) const;
	void * doGetAddress(void * head, uint32_t index) const;
	void requireMemory(uint32_t count);
	void doSetCapacity(uint32_t newCapacity);
	void doFreeObjects(void * head, uint32_t startIndex, uint32_t endIndex); // [startIndex, endIndex)

private:
	GSharedInterface<IMetaClass> metaClass;
	GMetaType metaType;
	uint32_t objectSize;
	uint32_t objectCount;
	uint32_t capacity;
	void * buffer;
};


GMetaObjectArrayImplement::GMetaObjectArrayImplement(IMetaClass * metaClass)
	: metaClass(metaClass), metaType(metaGetItemType(metaClass)), objectSize(metaClass->getTypeSize()), objectCount(0), capacity(0), buffer(NULL)
{
}

GMetaObjectArrayImplement::~GMetaObjectArrayImplement()
{
	if(this->buffer != NULL) {
		this->doFreeObjects(this->buffer, 0, this->objectCount);

		free(this->buffer);
	}
}

void GMetaObjectArrayImplement::setObject(uint32_t index, void * object)
{
	if(object == NULL) {
		return;
	}

	this->requireMemory(index);
	void * newObject = this->getObject(index);
	this->metaClass->cloneInplace(object, newObject);
	if(this->objectCount < index) {
		this->objectCount = index;
	}
}

void * GMetaObjectArrayImplement::getObject(uint32_t index) const
{
	if(index < this->objectSize) {
		return this->doGetObject(index);
	}
	else {
		return NULL;
	}
}

uint32_t GMetaObjectArrayImplement::getObjectCount() const
{
	return this->objectSize;
}

const GMetaType & GMetaObjectArrayImplement::getMetaType() const
{
	return this->metaType;
}

void * GMetaObjectArrayImplement::getBuffer() const
{
	return this->buffer;
}

void * GMetaObjectArrayImplement::doGetObject(uint32_t index) const
{
	return this->doGetAddress(this->buffer, index);
}

void * GMetaObjectArrayImplement::doGetAddress(void * head, uint32_t index) const
{
	return static_cast<char *>(head) + (index * this->objectSize);
}

void GMetaObjectArrayImplement::requireMemory(uint32_t count)
{
	if(count > this->capacity) {
		uint32_t newCapacity = count;
		if(count > 16) {
			newCapacity += count / 2;
		}
		else {
			newCapacity += count;
		}
		
		this->doSetCapacity(newCapacity);
	}

	while(count > this->objectCount) {
		--count;
		void * object = this->doGetObject(count);
		this->metaClass->createInplace(object);
	}
}

void GMetaObjectArrayImplement::doSetCapacity(uint32_t newCapacity)
{
	if(newCapacity == this->capacity) {
		return;
	}
	
	if(newCapacity < this->objectCount) {
		this->doFreeObjects(this->buffer, newCapacity, this->objectCount);
	}
	
	void * newBuffer = realloc(this->buffer, newCapacity * this->objectSize);

	if(this->buffer != newBuffer) {
		uint32_t minCount = std::min(this->objectCount, newCapacity);
		for(uint32_t i = 0; i < minCount; ++i) {
			this->metaClass->cloneInplace(this->doGetAddress(this->buffer, i), this->doGetAddress(newBuffer, i));
		}
		
		this->doFreeObjects(this->buffer, 0, this->objectCount);
	}
	
	this->buffer = newBuffer;
	this->capacity = newCapacity;
}

void GMetaObjectArrayImplement::doFreeObjects(void * head, uint32_t startIndex, uint32_t endIndex)
{
	while(startIndex < endIndex) {
		void * object = this->doGetAddress(head, startIndex);
		this->metaClass->destroyInplace(object);
		++startIndex;
	}
}


GMetaObjectArray::GMetaObjectArray(IMetaClass * metaClass)
	: implement(new GMetaObjectArrayImplement(metaClass))
{
}

void * GMetaObjectArray::getMemory() const
{
	return this->implement->getBuffer();
}

GVariant GMetaObjectArray::getItem(uint32_t index) const
{
	void * object = this->implement->getObject(index);
	if(object == NULL) {
		return object;
	}
	else {
		return createTypedVariant(object, this->implement->getMetaType());
	}
}

void GMetaObjectArray::setItem(uint32_t index, const GVariant & value)
{
	void * object = objectAddressFromVariant(value);
	this->implement->setObject(index, object);
}

uint32_t GMetaObjectArray::getCount() const
{
	return this->implement->getObjectCount();
}


} // namespace cpgf
