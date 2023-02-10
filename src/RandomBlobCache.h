#pragma once

namespace jellybench
{

	struct Config;

	class RandomBlobCache
	{
	public:
		typedef std::vector<uint8_t> Blob;

					RandomBlobCache(
						const Config*		aConfig,
						std::mt19937&		aRandom);
					~RandomBlobCache();
	
		const Blob&	GetRandomBlob(
						std::mt19937&		aRandom) const;

	private:

		std::vector<std::unique_ptr<Blob>>	m_blobs;
	};

}