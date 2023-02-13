#pragma once

#include "../../SQLiteBlobNode.h"

#include "../IWriteBackend.h"

namespace jellybench::Write::SQLite
{

	class SQLiteWrite
		: public IWriteBackend
	{
	public:
				SQLiteWrite(
					const Config*		aConfig);
				~SQLiteWrite();

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
		SQLiteBlobNode		m_blobNode;
	};	

}