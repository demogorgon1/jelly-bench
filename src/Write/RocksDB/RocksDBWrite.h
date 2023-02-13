#pragma once

#if defined(JELLY_BENCH_ROCKSDB)

#include "../../RocksDBBlobNode.h"

#include "../IWriteBackend.h"

namespace jellybench::Write::RocksDB
{

	class RocksDBWrite
		: public IWriteBackend
	{
	public:
				RocksDBWrite(
					const Config*		aConfig);
				~RocksDBWrite();

		// IWriteBackend implementation
		void	Update() override;
		void	Set(
					uint32_t			aKey,
					uint32_t			aSeq,
					const void*			aBlob,
					size_t				aBlobSize) override;
		size_t	GetDiskSpaceUsed() override;

	private:

		jelly::Timer		m_flushTimer;
		RocksDBBlobNode		m_blobNode;
	};	

}

#endif
