#include "Pcheader.h"

#include "Write/Jelly/JellyWrite.h"
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
		else
			JELLY_FATAL_ERROR("Invalid write backend: %s", aConfig->m_writeBackend.c_str());
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

	std::unique_ptr<jellybench::Write::IWriteBackend> writeBackend(jellybench::_CreateWriteBackend(&config));

	jellybench::Write::WriteBenchmark(&config, writeBackend.get());

	return EXIT_SUCCESS;
}