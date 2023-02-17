#pragma once

#include "../IWriteBackend.h"

namespace jellybench::Write::Jelly
{

	class JellyWrite
		: public IWriteBackend
	{
	public:
				JellyWrite(
					const Config*		aConfig);
		virtual	~JellyWrite();

		// IWriteBackend implementation
		void	Update() override;
		void	Set(
					uint32_t			aKey,
					uint32_t			aSeq,
					const void*			aBlob,
					size_t				aBlobSize) override;
		size_t	GetDiskSpaceUsed() override;

	private:

		typedef jelly::BlobNode<jelly::UIntKey<uint32_t>> BlobNodeType;
		typedef jelly::HousekeepingAdvisor<BlobNodeType> HouseKeepingAdvisorType;

		std::unique_ptr<jelly::DefaultHost>					m_host;
		std::unique_ptr<BlobNodeType>						m_blobNode;
		std::unique_ptr<HouseKeepingAdvisorType>			m_housekeepingAdvisor;
		std::vector<std::unique_ptr<BlobNodeType::Request>>	m_requests;
		jelly::Timer										m_processRequestsTimer;

		void	_UpdateRequests();
	};	

}