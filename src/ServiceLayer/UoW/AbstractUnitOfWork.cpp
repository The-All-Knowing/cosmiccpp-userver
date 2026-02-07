#include "AbstractUnitOfWork.hpp"
#include <cassert>


namespace Allocation::ServiceLayer::UoW
{
    void AbstractUnitOfWork::Commit()
    {
        _isCommitted = true;
        if (_trackingRepository)
            _trackingRepository->Clear();
    }

    void AbstractUnitOfWork::RollBack()
    {
        _isCommitted = false;
        if (_trackingRepository)
            _trackingRepository->Clear();
    }

    bool AbstractUnitOfWork::IsCommitted() const noexcept { return _isCommitted; }

    Domain::IRepository& AbstractUnitOfWork::GetProductRepository() 
    {
        if (_trackingRepository)
            return *_trackingRepository;
        throw std::runtime_error("Repository is not set");
    }

    std::vector<Domain::IMessagePtr> AbstractUnitOfWork::GetNewMessages() noexcept
    {
        std::vector<Domain::IMessagePtr> newMessages;
        for (auto& product : getUpdatedProducts())
        {
            auto messages = product->Messages();
            newMessages.insert(newMessages.end(), messages.begin(), messages.end());
            product->ClearMessages();
        }
        return newMessages;
    }

    std::vector<Domain::ProductPtr> AbstractUnitOfWork::getUpdatedProducts() const noexcept
    {
        if (_trackingRepository)
            return _trackingRepository->GetUpdated();
        return {};
    }

    void AbstractUnitOfWork::setRepository(Domain::IRepository& repository) noexcept
    {
        _trackingRepository = std::make_unique<Adapters::Repository::TrackingRepository>(repository);
    }
}