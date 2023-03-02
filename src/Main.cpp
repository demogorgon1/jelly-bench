#include "Pcheader.h"

#include "Write/Jelly/JellyWrite.h"
#include "Write/RocksDB/RocksDBWrite.h"
#include "Write/SQLite/SQLiteWrite.h"
#include "Write/WriteBenchmark.h"

#include "Config.h"

namespace jellybench
{

	jellybench::Write::IWriteBackend*
	_CreateWriteBackend(
		const Config*	aConfig)
	{
		if(aConfig->m_writeBackend == "jelly")
			return new Write::Jelly::JellyWrite(aConfig);
		else if(aConfig->m_writeBackend == "sqlite")
			return new Write::SQLite::SQLiteWrite(aConfig);
#if defined(JELLY_BENCH_ROCKSDB)
		else if (aConfig->m_writeBackend == "rocksdb")
			return new Write::RocksDB::RocksDBWrite(aConfig);
#endif
		else
			JELLY_ALWAYS_ASSERT(false, "Invalid write backend: %s", aConfig->m_writeBackend.c_str());
		return NULL;
	}

}

int
main(
	int		aNumArgs,
	char**	aArgs)
{
	jellybench::Config config;
	config.InitFromCommandLine(aNumArgs, aArgs);

	config.m_writeBackend = "jelly";
	config.m_rocksDBCompression = "none";
	config.m_jellyConfig.SetString("compression_method", "none");
	config.m_jellyConfig.SetString("max_resident_blob_size", "0");
	config.m_writeRate = 10000;

	std::unique_ptr<jellybench::Write::IWriteBackend> writeBackend(jellybench::_CreateWriteBackend(&config));

	jellybench::Write::WriteBenchmark(&config, writeBackend.get());

	system("pause");

	return EXIT_SUCCESS;
}