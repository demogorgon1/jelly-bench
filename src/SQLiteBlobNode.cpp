#include "Pcheader.h"

#include <sqlite3.h>

#include "SQLiteBlobNode.h"

namespace jellybench
{

	struct SQLiteBlobNode::Internal
	{
		Internal(
			const Config*		aConfig)
			: m_config(aConfig)
			, m_batching(false)
		{
			std::filesystem::remove("sqlite.db");

			{
				int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
				int result = sqlite3_open_v2("sqlite.db", &m_sqlite, flags, NULL);
				JELLY_CHECK(result == SQLITE_OK, "sqlite3_open_v2() failed: %s", sqlite3_errmsg(m_sqlite));
				JELLY_ASSERT(m_sqlite != NULL);
			}

			{
				const char* sql = 
					"CREATE TABLE blobs("
						"key INT PRIMARY KEY NOT NULL,"	
						"seq INT NOT NULL,"	
						"data BLOB NOT NULL"
					");";

				char* errMsg;
				int result = sqlite3_exec(m_sqlite, sql, NULL, NULL, &errMsg);
				JELLY_CHECK(result == SQLITE_OK, "sqlite3_exec() failed: %s", errMsg);
			}

			{
				const char* sql = 
					"INSERT INTO blobs(key, seq, data) "
					"VALUES(?, ?, ?) "
					"ON CONFLICT DO UPDATE SET "
						"data = CASE WHEN EXCLUDED.seq > blobs.seq THEN EXCLUDED.data ELSE NULL END, "
						"seq = EXCLUDED.seq;";

				int flags = SQLITE_PREPARE_PERSISTENT;
				int result = sqlite3_prepare_v3(m_sqlite, sql, -1, flags, &m_stmtSet, NULL);
				JELLY_CHECK(result == SQLITE_OK, "sqlite3_prepare() failed: %s", sqlite3_errmsg(m_sqlite));
				JELLY_ASSERT(m_stmtSet != NULL);
			}

		}

		~Internal()
		{
			JELLY_ASSERT(m_stmtSet != NULL);
			sqlite3_finalize(m_stmtSet);

			JELLY_ASSERT(m_sqlite != NULL);
			sqlite3_close(m_sqlite);
		}
		
		// Public data
		const Config*	m_config;

		sqlite3*		m_sqlite;
		sqlite3_stmt*	m_stmtSet;

		bool			m_batching;
	};

	//--------------------------------------------------------------------------------

	SQLiteBlobNode::SQLiteBlobNode(
		const Config*			aConfig)
	{
		m_internal = std::make_unique<Internal>(aConfig);
	}
	
	SQLiteBlobNode::~SQLiteBlobNode()
	{

	}

	void	
	SQLiteBlobNode::Set(
		uint32_t				aKey,
		uint32_t				aSeq,
		const void*				aBlob,
		size_t					aBlobSize)
	{
		if(!m_internal->m_batching)
		{
			char* errMsg;
			int result = sqlite3_exec(m_internal->m_sqlite, "BEGIN", NULL, NULL, &errMsg);
			JELLY_CHECK(result == SQLITE_OK, "sqlite3_exec() failed: %s", errMsg);

			m_internal->m_batching = true;
		}

		{
			int result = sqlite3_bind_int64(m_internal->m_stmtSet, 1, (sqlite3_int64)aKey);
			JELLY_CHECK(result == SQLITE_OK, "sqlite3_bind_int64() failed: %d", result);
		}

		{
			int result = sqlite3_bind_int64(m_internal->m_stmtSet, 2, (sqlite3_int64)aSeq);
			JELLY_CHECK(result == SQLITE_OK, "sqlite3_bind_int64() failed: %d", result);
		}

		{
			int result = sqlite3_bind_blob(m_internal->m_stmtSet, 3, aBlob, (int)aBlobSize, SQLITE_STATIC);
			JELLY_CHECK(result == SQLITE_OK, "sqlite3_bind_blob() failed: %d", result);
		}

		{
			int result = sqlite3_step(m_internal->m_stmtSet);
			JELLY_CHECK(result == SQLITE_DONE, "sqlite3_step() failed: %d", result);
		}

		{
			int result = sqlite3_reset(m_internal->m_stmtSet);
			JELLY_CHECK(result == SQLITE_OK, "sqlite3_reset() failed: %d", result);
		}
	}

	void	
	SQLiteBlobNode::Flush()
	{
		if (m_internal->m_batching)
		{
			char* errMsg;
			int result = sqlite3_exec(m_internal->m_sqlite, "COMMIT", NULL, NULL, &errMsg);
			JELLY_CHECK(result == SQLITE_OK, "sqlite3_exec() failed: %s", errMsg);

			m_internal->m_batching = false;
		}
	}

	size_t
	SQLiteBlobNode::GetDiskSpaceUsed()
	{
		size_t bytes = 0;

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("."))
		{
			if (entry.is_regular_file())
			{
				std::string fileName = entry.path().filename().string();

				if(strncmp(fileName.c_str(), "sqlite", 6) == 0)
					bytes += (size_t)entry.file_size();
			}
		}

		return bytes;
	}

}