#include "TrackingRepository.hpp"


namespace Allocation::Adapters::Repository
{
    TrackingRepository::TrackingRepository(Domain::IRepository& repo) : _repo(repo) {}

    void TrackingRepository::Add(Domain::ProductPtr product)
    {
        _repo.Add(product);
        _seenProducts.insert(product);
    }

    void TrackingRepository::Update(Domain::ProductPtr product)
    {
        _repo.Update(product);
        _seenProducts.insert(product);
    }

    Domain::ProductPtr TrackingRepository::Get(std::string_view sku)
    {
        return _repo.Get(sku);
    }

    Domain::ProductPtr TrackingRepository::GetByBatchRef(std::string_view batchRef)
    {
        return _repo.GetByBatchRef(batchRef);
    }

    std::unordered_set<Domain::ProductPtr> TrackingRepository::GetSeen() const noexcept
    {
        return _seenProducts;
    }

    void TrackingRepository::Clear() noexcept { _seenProducts.clear(); }
}
