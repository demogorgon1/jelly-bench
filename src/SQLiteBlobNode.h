#pragma once

namespace jellybench
{

	struct Config;

	class SQLiteBlobNode
	{
	public:
				SQLiteBlobNode(
					const Config*			aConfig);
				~SQLiteBlobNode();

		void	Set(
					uint32_t				aKey,
					uint32_t				aSeq,
					const void*				aBlob,
					size_t					aBlobSize);
		void	Flush();
		size_t	GetDiskSpaceUsed(); 

	private:

		struct Internal;
		std::unique_ptr<Internal>	m_internal;
	};

}