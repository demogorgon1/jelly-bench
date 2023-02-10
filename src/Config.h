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
				std::string arg = aArgs[i];
				std::optional<std::string> value;
				if(i + 1 < aNumArgs && aArgs[i + 1][0] != '-')
					value = aArgs[i + 1];

				if(arg == "min_blob_size" && value.has_value())
					m_minBlobSize = (size_t)atoi(value.value().c_str());
				else if (arg == "max_blob_size" && value.has_value())
					m_maxBlobSize = (size_t)atoi(value.value().c_str());
				else if (arg == "random_blob_cache_size" && value.has_value())
					m_randomBlobCacheSize = (size_t)atoi(value.value().c_str());
				else if (arg == "blob_key_count" && value.has_value())
					m_blobKeyCount = (size_t)atoi(value.value().c_str());
				else if (arg == "write_backend" && value.has_value())
					m_writeBackend = value.value();
				else if (arg == "write_rate" && value.has_value())
					m_writeRate = (size_t)atoi(value.value().c_str());
				else if (arg == "write_run_seconds" && value.has_value())
					m_writeRunSeconds = (uint32_t)atoi(value.value().c_str());
				else if(value.has_value())
					m_jellyConfig.SetString(arg.c_str(), value.value().c_str());
				else
					JELLY_FATAL_ERROR("Syntax error.");
			}
		}
			
		size_t							m_minBlobSize = 5000;
		size_t							m_maxBlobSize = 15000;
		size_t							m_randomBlobCacheSize = 1000;
		size_t							m_blobKeyCount = 10000;
		jelly::DefaultConfigSource		m_jellyConfig;

		// Write
		std::string						m_writeBackend = "jelly";
		size_t							m_writeRate = 1000;
		uint32_t						m_writeRunSeconds = 10;
	};

}