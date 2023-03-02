#include "Pcheader.h"

#if defined(JELLY_BENCH_ROCKSDB)

#include <rocksdb/db.h>

#include "Config.h"
#include "RocksDBBlobNode.h"

namespace jellybench
{

	rocksdb::CompressionType
	_StringToCompressionType(
		const std::string&		aString)
	{
		if(aString == "none")
			return rocksdb::kNoCompression;
		else if (aString == "zstd")
			return rocksdb::kZSTD;
		else if (aString == "lz4")
			return rocksdb::kLZ4Compression;
		else if (aString == "snappy")
			return rocksdb::kSnappyCompression;

		JELLY_ALWAYS_ASSERT(false, "Invalid rocksdb compression: %s", aString);
		return rocksdb::CompressionType(0);
	}

	struct RocksDBBlobNode::Internal
	{
		Internal(
			const Config*		aConfig)
			: m_config(aConfig)
		{
			std::filesystem::remove_all("rocksdb");

			// Create database with extra column family for sequence number 
			{
				rocksdb::DB* tempDB;

				{
					rocksdb::Options options;
					options.create_if_missing = true;
					options.error_if_exists = true;
					options.compression = _StringToCompressionType(aConfig->m_rocksDBCompression);

					rocksdb::Status status = rocksdb::DB::Open(options, "rocksdb", &tempDB);
					JELLY_ALWAYS_ASSERT(status.ok(), "rocksdb::DB::Open() failed: %s", status.getState());
					JELLY_ASSERT(tempDB != NULL);
				}

				rocksdb::ColumnFamilyHandle* cf;

				{
					rocksdb::Status status = tempDB->CreateColumnFamily(rocksdb::ColumnFamilyOptions(), "seq", &cf);
					JELLY_ALWAYS_ASSERT(status.ok(), "rocksdb::DB::CreateColumnFamily() failed: %s", status.getState());
					JELLY_ASSERT(cf != NULL);
				}

				{
					rocksdb::Status status = tempDB->DestroyColumnFamilyHandle(cf);
					JELLY_ALWAYS_ASSERT(status.ok(), "rocksdb::DB::DestroyColumnFamilyHandle() failed: %s", status.getState());
				}

				delete tempDB;
			}

			// Default column family (blob itself)
			{
				rocksdb::ColumnFamilyOptions columnFamilyOptions;
				m_columnFamilyDescriptors.push_back(rocksdb::ColumnFamilyDescriptor(
					rocksdb::kDefaultColumnFamilyName, 
					columnFamilyOptions));
			}

			// Sequence number column family
			{
				rocksdb::ColumnFamilyOptions columnFamilyOptions;
				m_columnFamilyDescriptors.push_back(rocksdb::ColumnFamilyDescriptor(
					"seq",
					columnFamilyOptions));
			}

			// Open database
			rocksdb::Options options;
			options.create_if_missing = false;
			options.compression = _StringToCompressionType(aConfig->m_rocksDBCompression);

			rocksdb::Status status = rocksdb::DB::Open(options, "rocksdb", m_columnFamilyDescriptors, &m_columnFamilyHandles, &m_db);
			JELLY_ALWAYS_ASSERT(status.ok(), "rocksdb::DB::Open() failed: %s", status.getState());
			JELLY_ASSERT(m_db != NULL);
		}

		~Internal()
		{
			JELLY_ASSERT(m_db != NULL);

			for(rocksdb::ColumnFamilyHandle* t : m_columnFamilyHandles)
			{
				rocksdb::Status status = m_db->DestroyColumnFamilyHandle(t);
				JELLY_ASSERT(status.ok());
			}

			delete m_db;
		}
		
		// Public data
		const Config*									m_config;

		rocksdb::DB*									m_db;

		std::vector<rocksdb::ColumnFamilyDescriptor>	m_columnFamilyDescriptors;
		std::vector<rocksdb::ColumnFamilyHandle*>		m_columnFamilyHandles;
	};

	//--------------------------------------------------------------------------------

	RocksDBBlobNode::RocksDBBlobNode(
		const Config*			aConfig)
	{
		m_internal = std::make_unique<Internal>(aConfig);
	}
	
	RocksDBBlobNode::~RocksDBBlobNode()
	{

	}

	void	
	RocksDBBlobNode::Set(
		uint32_t				aKey,
		uint32_t				aSeq,
		const void*				aBlob,
		size_t					aBlobSize)
	{
		rocksdb::Slice key((const char*)&aKey, sizeof(aKey));
		rocksdb::Slice seq((const char*)&aSeq, sizeof(aSeq));
		rocksdb::Slice blob((const char*)aBlob, aBlobSize);

		{
			rocksdb::PinnableSlice existingSeq;

			rocksdb::ReadOptions options;
			rocksdb::Status status = m_internal->m_db->Get(options, m_internal->m_columnFamilyHandles[1], key, &existingSeq);
			if(status.ok())
			{
				JELLY_ASSERT(existingSeq.size() == sizeof(uint32_t));
				const uint32_t* p = (const uint32_t*)existingSeq.data();
				uint32_t seqValue = *p;
				if (seqValue >= aSeq)
					return;
			}
		}

		{
			rocksdb::WriteOptions options;
			rocksdb::Status status = m_internal->m_db->Put(options, m_internal->m_columnFamilyHandles[0], key, blob);
			JELLY_ALWAYS_ASSERT(status.ok(), "rocksdb::DB::Put() failed: %s", status.getState());
		}

		{
			rocksdb::WriteOptions options;
			rocksdb::Status status = m_internal->m_db->Put(options, m_internal->m_columnFamilyHandles[1], key, seq);
			JELLY_ALWAYS_ASSERT(status.ok(), "rocksdb::DB::Put() failed: %s", status.getState());
		}
	}

	void	
	RocksDBBlobNode::Flush()
	{
		m_internal->m_db->FlushWAL(true);
	} 

	size_t	
	RocksDBBlobNode::GetDiskSpaceUsed()
	{
		size_t bytes = 0;

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("rocksdb"))
		{
			if (entry.is_regular_file())
				bytes += (size_t)entry.file_size();
		}

		return bytes;
	}

}

#endif
