#include "AbstractUnitOfWork.hpp"


namespace Allocation::ServiceLayer::UoW
{
    AbstractUnitOfWork::AbstractUnitOfWork(Domain::IRepository& repository)
        : _trackingRepository(repository)
    {
    }

    void AbstractUnitOfWork::Commit()
    {
        _isCommitted = true;
        _trackingRepository.Clear();
    }

    void AbstractUnitOfWork::RollBack()
    {
        _isCommitted = false;
        _trackingRepository.Clear();
    }

    bool AbstractUnitOfWork::IsCommitted() const noexcept { return _isCommitted; }

    Domain::IRepository& AbstractUnitOfWork::GetProductRepository() { return _trackingRepository; }

    std::vector<Domain::IMessagePtr> AbstractUnitOfWork::GetNewMessages() noexcept
    {
        std::vector<Domain::IMessagePtr> newMessages;
        for (auto& product : _trackingRepository.GetUpdated())
        {
            auto messages = product->Messages();
            newMessages.insert(newMessages.end(), messages.begin(), messages.end());
            product->ClearMessages();
        }
        return newMessages;
    }

    std::vector<Domain::ProductPtr> AbstractUnitOfWork::GetUpdatedProducts() const noexcept
    {
        return _trackingRepository.GetUpdated();
    }
}