#pragma once

namespace jellybench
{
	struct Config;
}

namespace jellybench::Write
{

	class IWriteBackend;

	void	WriteBenchmark(
				const Config*		aConfig,
				IWriteBackend*		aBackend);

}