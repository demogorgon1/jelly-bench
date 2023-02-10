#include "Pcheader.h"

#include "Config.h"
#include "RandomBlobCache.h"

namespace jellybench
{

	RandomBlobCache::RandomBlobCache(
		const Config*	aConfig,
		std::mt19937&	aRandom)
	{
		std::uniform_int_distribution<size_t> size(aConfig->m_minBlobSize, aConfig->m_maxBlobSize);

		for(size_t i = 0; i < aConfig->m_randomBlobCacheSize; i++)
		{
			std::unique_ptr<Blob> blob = std::make_unique<Blob>();			

			size_t blobSize = size(aRandom);
			blob->reserve(blobSize);
			for(size_t j = 0; j < blobSize; j++)
				blob->push_back((uint8_t)(aRandom() % 64)); // Not completely random, give the compressor a chance

			m_blobs.push_back(std::move(blob));
		}
	}
	
	RandomBlobCache::~RandomBlobCache()
	{

	}

	const RandomBlobCache::Blob&
	RandomBlobCache::GetRandomBlob(
		std::mt19937&	aRandom) const
	{
		JELLY_ASSERT(m_blobs.size() > 0);

		std::uniform_int_distribution<size_t> distribution(0, m_blobs.size() - 1);
		return *m_blobs[distribution(aRandom)];
	}


}