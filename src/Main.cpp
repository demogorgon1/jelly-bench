#include "Pcheader.h"

#include "Write/Jelly/JellyWrite.h"
#include "Write/WriteBenchmark.h"

#include "Config.h"

int
main(
	int		aNumArgs,
	char**	aArgs)
{
	jellybench::Config config;
	config.InitFromCommandLine(aNumArgs, aArgs);

	std::unique_ptr<jellybench::Write::IWriteBackend> writeBackend = std::make_unique<jellybench::Write::Jelly::JellyWrite>(&config);

	jellybench::Write::WriteBenchmark(&config, writeBackend.get());

	return EXIT_SUCCESS;
}