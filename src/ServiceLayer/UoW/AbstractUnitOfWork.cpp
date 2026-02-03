#include "AbstractUnitOfWork.hpp"


namespace Allocation::ServiceLayer::UoW
{
    AbstractUnitOfWork::AbstractUnitOfWork(Domain::IRepository& repository)
        : _trackingRepository(repository)
    {
    }

    void AbstractUnitOfWork::Commit() { _isCommitted = true; }

    void AbstractUnitOfWork::RollBack() { _isCommitted = false; }

    bool AbstractUnitOfWork::IsCommitted() const noexcept { return _isCommitted; }

    Domain::IRepository& AbstractUnitOfWork::GetProductRepository() { return _trackingRepository; }

    std::vector<Domain::IMessagePtr> AbstractUnitOfWork::GetNewMessages() noexcept
    {
        std::vector<Domain::IMessagePtr> newMessages;
        for (const auto& [_, product] : _trackingRepository.GetSeen())
        {
            auto messages = product->Messages();
            newMessages.insert(newMessages.end(), messages.begin(), messages.end());
            product->ClearMessages();
        }
        return newMessages;
    }

    std::unordered_set<Domain::ProductPtr> AbstractUnitOfWork::GetUpdatedProducts() const noexcept
    {
        std::unordered_set<Domain::ProductPtr> result;
        for (const auto& [_, product] : _trackingRepository.GetSeen())
            result.insert(product);
        return result;
    }
}