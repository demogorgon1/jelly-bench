#include "../../Pcheader.h"

#if defined(JELLY_BENCH_ROCKSDB)

#include "../../Config.h"

#include "RocksDBWrite.h"

namespace jellybench::Write::RocksDB
{

	RocksDBWrite::RocksDBWrite(
		const Config*		aConfig)
		: IWriteBackend(aConfig)
		, m_blobNode(aConfig)
	{
		m_flushTimer.SetTimeout(500);
	}

	RocksDBWrite::~RocksDBWrite()
	{

	}

	//------------------------------------------------------------------------

	void	
	RocksDBWrite::Update()
	{	
		if(m_flushTimer.HasExpired())
			m_blobNode.Flush();
	}

	void	
	RocksDBWrite::Set(
		uint32_t			aKey,
		uint32_t			aSeq,
		const void*			aBlob,
		size_t				aBlobSize) 
	{
		m_blobNode.Set(aKey, aSeq, aBlob, aBlobSize);
	}

	size_t
	RocksDBWrite::GetDiskSpaceUsed()
	{
		return m_blobNode.GetDiskSpaceUsed();
	}

	//------------------------------------------------------------------------

}

#endif
