#include "TrackingRepository.hpp"


namespace Allocation::Adapters::Repository
{
    TrackingRepository::TrackingRepository(Domain::IRepository& repo) : _repo(repo) {}

    void TrackingRepository::Add(Domain::ProductPtr product)
    {
        if (_seenProducts.contains(product->GetSKU()))
            throw std::invalid_argument("Product with the same SKU already exists in the repository");
        _repo.Add(product);
        _updatedProducts.emplace(product->GetSKU(), product);
        _seenProducts.emplace(product->GetSKU(), product);
    }

    void TrackingRepository::Update(Domain::ProductPtr product)
    {
        _repo.Update(product);
        _updatedProducts.insert_or_assign(product->GetSKU(), product);
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
        _seenProducts.clear();
        _updatedProducts.clear();
    }
}
