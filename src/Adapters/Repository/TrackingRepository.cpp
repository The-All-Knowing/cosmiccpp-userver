#include "TrackingRepository.hpp"


namespace Allocation::Adapters::Repository
{
    TrackingRepository::TrackingRepository(Domain::ICommonRepository& repo) : _repo(repo) {}

    void TrackingRepository::Add(Domain::ProductPtr product)
    {
        if (_seenProducts.contains(product->GetSKU()))
            throw std::invalid_argument(
                "Product with the same SKU already exists in the repository.");
        _seenProducts.emplace(product->GetSKU(), product);
    }

    Domain::ProductPtr TrackingRepository::Get(const std::string& sku)
    {
        if (const auto it = _seenProducts.find(sku); it != _seenProducts.end())
            return it->second;
        if (auto product = _repo.Get(sku))
            return _seenProducts.emplace(sku, product).first->second;
        return nullptr;
    }

    Domain::ProductPtr TrackingRepository::GetByBatchRef(const std::string& batchRef)
    {
        for (auto& [_, product] : _seenProducts)
            if (product->GetBatch(batchRef).has_value())
                return product;
        if (auto product = _repo.GetByBatchRef(batchRef))
            return _seenProducts.emplace(product->GetSKU(), product).first->second;
        return nullptr;
    }

    std::vector<Domain::ProductPtr> TrackingRepository::GetSeen() const noexcept
    {
        std::vector<Domain::ProductPtr> seenProducts;
        seenProducts.reserve(_seenProducts.size());
        for (const auto& [_, product] : _seenProducts)
            seenProducts.push_back(product);
        return seenProducts;
    }

    void TrackingRepository::Clear() noexcept { _seenProducts.clear(); }
}
