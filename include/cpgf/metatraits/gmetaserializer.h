#ifndef __GMETASERIALIZER_H
#define __GMETASERIALIZER_H


#include "cpgf/gapiutil.h"


namespace cpgf {

struct IMetaArchiveWriter;
struct IMetaArchiveReader;
struct IMetaClass;
struct GMetaArchiveWriterParam;
struct GMetaArchiveReaderParam;
struct IMetaSerializerWriter;
struct IMetaSerializerReader;
struct GMetaTraitsParam;


struct IMetaSerializer : public IObject
{
	virtual const char * G_API_CC getClassTypeName(IMetaArchiveWriter * archiveWriter, const void * instance, IMetaClass * metaClass) = 0;
	virtual void G_API_CC writeObject(IMetaArchiveWriter * archiveWriter, IMetaSerializerWriter * serializerWriter, GMetaArchiveWriterParam * param) = 0;
	
	virtual void * G_API_CC allocateObject(IMetaArchiveReader * archiveReader, IMetaClass * metaClass) = 0;
	virtual void G_API_CC readObject(IMetaArchiveReader * archiveReader, IMetaSerializerReader * serializerReader, GMetaArchiveReaderParam * param) = 0;
};


template <typename T>
struct GMetaTraitsCreateSerializer
{
	static IMetaSerializer * createSerializer(const GMetaTraitsParam &) {
		return NULL;
	}
};

template <typename T>
inline IMetaSerializer * metaTraitsCreateSerializer(const T &, const GMetaTraitsParam & param)
{
	return GMetaTraitsCreateSerializer<T>::createSerializer(param);
}



} // namespace cpgf



#endif
