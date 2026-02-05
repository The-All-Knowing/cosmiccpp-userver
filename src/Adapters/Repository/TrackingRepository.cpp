#include "TrackingRepository.hpp"


namespace Allocation::Adapters::Repository
{
    TrackingRepository::TrackingRepository(Domain::IRepository& repo) : _repo(repo) {}

    void TrackingRepository::Add(Domain::ProductPtr product)
    {
        _repo.Add(product);
        _updatedProducts.try_emplace(product->GetSKU(), product);
        _seenProducts.try_emplace(product->GetSKU(), product);
    }

    void TrackingRepository::Update(Domain::ProductPtr product)
    {
        _repo.Update(product);
        _updatedProducts.try_emplace(product->GetSKU(), product);
    }

    Domain::ProductPtr TrackingRepository::Get(const std::string& sku)
    {
        if (!_seenProducts.contains(sku))
            _seenProducts.emplace(sku, _repo.Get(sku));
        return _seenProducts[sku];
    }

    Domain::ProductPtr TrackingRepository::GetByBatchRef(const std::string& batchRef)
    {
        for (auto& [_, product] : _seenProducts)
            if (product->GetBatch(batchRef).has_value())
                return product;
        auto product = _repo.GetByBatchRef(batchRef);
        _seenProducts.emplace(product->GetSKU(), product);
        return product;
    }

    std::vector<Domain::ProductPtr> TrackingRepository::GetUpdated() const noexcept
    {
        std::vector<Domain::ProductPtr> updatedProducts;
        updatedProducts.reserve(_updatedProducts.size());
        for (const auto& [_, product] : _updatedProducts)
            updatedProducts.push_back(product);
        return updatedProducts;
    }

    void TrackingRepository::Clear() noexcept
    {
        _updatedProducts.clear();
        _seenProducts.clear();
    }
}
