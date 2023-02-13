#pragma once

#if defined(JELLY_BENCH_ROCKSDB)

namespace jellybench
{

	struct Config;

	class RocksDBBlobNode
	{
	public:
				RocksDBBlobNode(
					const Config*			aConfig);
				~RocksDBBlobNode();

		void	Set(
					uint32_t				aKey,
					uint32_t				aSeq,
					const void*				aBlob,
					size_t					aBlobSize);
		void	Flush();
		size_t	GetDiskSpaceUsed();

	private:

		struct Internal;
		std::unique_ptr<Internal>	m_internal;
	};

}

#endif