#include "AbstractUnitOfWork.hpp"


namespace Allocation::ServiceLayer::UoW
{
    void AbstractUnitOfWork::Commit()
    {
        if (!_trackingRepository)
            throw std::runtime_error("Repository is not set");
        for (auto& product : getSeenProducts())
        {
            auto messages = product->Messages();
            _newMessages.insert(_newMessages.end(), messages.begin(), messages.end());
            product->ClearMessages();
        }
        commit();
        _isCommitted = true;
        _trackingRepository->Clear();
    }

    void AbstractUnitOfWork::RollBack()
    {
        if (!_trackingRepository)
            throw std::runtime_error("Repository is not set");
        rollBack();
        _isCommitted = false;
        _trackingRepository->Clear();
    }

    bool AbstractUnitOfWork::IsCommitted() const
    {
        if (!_trackingRepository)
            throw std::runtime_error("Repository is not set");
        return _isCommitted;
    }

    Domain::IRepository& AbstractUnitOfWork::GetProductRepository()
    {
        if (!_trackingRepository)
            throw std::runtime_error("Repository is not set");
        return *_trackingRepository;
    }

    std::vector<Domain::IMessagePtr> AbstractUnitOfWork::GetNewMessages()
    {
        if (!_trackingRepository)
            throw std::runtime_error("Repository is not set");
        return std::move(_newMessages);
    }

    std::vector<Domain::ProductPtr> AbstractUnitOfWork::getSeenProducts() const
    {
        if (!_trackingRepository)
            throw std::runtime_error("Repository is not set");

        return _trackingRepository->GetSeen();
    }

    void AbstractUnitOfWork::setRepository(Domain::ICommonRepository& repository)
    {
        _trackingRepository =
            std::make_unique<Adapters::Repository::TrackingRepository>(repository);
    }
}
