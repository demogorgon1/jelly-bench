#pragma once

namespace jellybench
{

	struct Config
	{
		void
		InitFromCommandLine(
			int		aNumArgs,
			char**	aArgs)
		{
			for (int i = 1; i < aNumArgs; i++)
			{
				const char* arg = aArgs[i];

				JELLY_UNUSED(arg);
			}
		}
			
		size_t							m_minBlobSize = 5000;
		size_t							m_maxBlobSize = 15000;
		size_t							m_randomBlobCacheSize = 1000;
		size_t							m_blobKeyCount = 10000;
		jelly::DefaultConfigSource		m_jellyConfig;

		// Write
		size_t							m_writeRate = 1000;
		uint32_t						m_writeRunSeconds = 10;
	};

}