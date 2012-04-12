#include "cpgf/metatraits/gmetaserializer_string.h"
#include "cpgf/serialization/gmetaarchivecommon.h"
#include "cpgf/serialization/gmetaarchivereader.h"
#include "cpgf/serialization/gmetaarchivewriter.h"

#include <string>


namespace cpgf {

class GMetaSerializerString : public IMetaSerializer
{
	G_INTERFACE_IMPL_OBJECT
	G_INTERFACE_IMPL_EXTENDOBJECT
	
public:
	virtual const char * G_API_CC getClassTypeName(IMetaArchiveWriter * /*archiveWriter*/, const void * /*instance*/, IMetaClass * /*metaClass*/) {
		return "meta_ser_std_string";
	}
	
	virtual void G_API_CC writeObject(IMetaArchiveWriter * archiveWriter, IMetaSerializerWriter * /*serializerWriter*/, GMetaArchiveWriterParam * param) {
		GScopedInterface<IMetaWriter> metaWriter(archiveWriter->getMetaWriter());
		archiveWriter->trackPointer(param->archiveID, param->instance, param->metaClass, this, param->pointers);
		metaWriter->writeString(param->name, param->archiveID, static_cast<const std::string *>(param->instance)->c_str());
	}
	
	virtual void * G_API_CC allocateObject(IMetaArchiveReader * /*archiveReader*/, IMetaClass * /*metaClass*/) {
		return new std::string;
	}

	virtual void G_API_CC readObject(IMetaArchiveReader * archiveReader, IMetaSerializerReader * /*serializerReader*/, GMetaArchiveReaderParam * param) {
		GScopedInterface<IMetaReader> metaReader(archiveReader->getMetaReader());
		uint32_t archiveID;
		char * s = metaReader->readString(param->name, archiveReader->getAllocator(), &archiveID);
		*static_cast<std::string *>(param->instance) = s;
		archiveReader->getAllocator()->free(s);
		
		archiveReader->trackPointer(archiveID, param->instance);
	}
};


IMetaSerializer * GMetaTraitsCreateSerializer<std::string>::createSerializer(const GMetaTraitsParam &)
{
	return new cpgf::GMetaSerializerString;
}



} // namespace cpgf

