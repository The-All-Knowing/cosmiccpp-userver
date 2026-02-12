#include "Product.hpp"

#include "Common.hpp"
#include "Domain/Events/Allocated.hpp"
#include "Domain/Events/Deallocated.hpp"
#include "Domain/Events/OutOfStock.hpp"


namespace Allocation::Domain
{
    Product::Product(
        const std::string& sku, const std::vector<Batch>& batches, int64_t versionNumber)
        : _sku(sku), _version(versionNumber)
    {
        for (const auto& batch : batches)
            _referenceByBatches.emplace(batch.GetReference(), batch);
    }

    void Product::AddBatch(const Batch& batch)
    {
        if (batch.GetSKU() != _sku)
            throw std::invalid_argument("Batch SKU does not match Product SKU");

        if (_referenceByBatches.contains(batch.GetReference()))
            throw std::invalid_argument("Batch with the same reference already exists");
        auto batchRef = batch.GetReference();
        _referenceByBatches.emplace(batchRef, batch);
    }

    void Product::AddBatches(const std::vector<Batch>& batches)
    {
        std::unordered_map<std::string, Batch> referenceByBatches;
        for (const auto& batch : batches)
        {
            if (batch.GetSKU() != _sku)
                throw std::invalid_argument("Batch SKU does not match Product SKU");
            if (referenceByBatches.contains(batch.GetReference()))
                throw std::invalid_argument("Batch with the same reference already exists");

            auto batchRef = batch.GetReference();
            referenceByBatches.emplace(batchRef, batch);
        }
        _referenceByBatches = std::move(referenceByBatches);
    }

    std::optional<std::string> Product::Allocate(const OrderLine& line)
    {
        if (line.sku != _sku)
            throw std::invalid_argument("OrderLine SKU does not match Product SKU");

        if (_referenceByBatches.empty())
            throw std::runtime_error("No batches available for allocation");

        using BatchRef = std::reference_wrapper<Batch>;
        std::set<BatchRef> sortedBatches;
        for (auto& [_, batch] : _referenceByBatches)
            sortedBatches.insert(std::ref(batch));

        for (auto& batch : sortedBatches)
        {
            if (!batch.get().CanAllocate(line))
                continue;

            auto ref = batch.get().GetReference();
            batch.get().Allocate(line);
            _messages.push_back(
                Make<Events::Allocated>(line.reference, line.sku, line.quantity, ref));
            return batch.get().GetReference();
        }
        _messages.push_back(Make<Events::OutOfStock>(line.sku));
        return std::nullopt;
    }

    void Product::ChangeBatchQuantity(const std::string& ref, size_t newQty)
    {
        auto it = _referenceByBatches.find(ref);
        if (it == _referenceByBatches.end())
            throw std::invalid_argument("Batch with the given reference not found");
        int32_t difference = static_cast<int32_t>(newQty) +
                             static_cast<int32_t>(it->second.GetAvailableQuantity()) -
                             static_cast<int32_t>(it->second.GetPurchasedQuantity());
        while (difference < 0)
        {
            auto order = it->second.DeallocateOne();
            difference += static_cast<int32_t>(order.quantity);
            _messages.push_back(
                Make<Events::Deallocated>(order.reference, order.sku, order.quantity));
        }
        it->second.SetPurchasedQuantity(newQty);
    }

    std::vector<Batch> Product::GetBatches() const noexcept
    {
        std::vector<Batch> result;
        for (const auto& [_, batch] : _referenceByBatches)
            result.push_back(batch);
        return result;
    }

    std::optional<Batch> Product::GetBatch(const std::string& batchReference) const noexcept
    {
        if (auto it = _referenceByBatches.find(batchReference); it != _referenceByBatches.end())
            return it->second;
        return std::nullopt;
    }

    int64_t Product::GetVersion() const noexcept { return _version; }

    std::string Product::GetSKU() const noexcept { return _sku; }

    const std::vector<Domain::IMessagePtr>& Product::Messages() const noexcept { return _messages; }

    void Product::ClearMessages() noexcept { _messages.clear(); }

    bool operator==(const Product& lhs, const Product& rhs) noexcept
    {
        if (lhs.GetSKU() != rhs.GetSKU() || lhs.GetVersion() != rhs.GetVersion())
            return false;
        auto lhsBatches = lhs.GetBatches();
        auto rhsBatches = rhs.GetBatches();
        if (lhsBatches.size() != rhsBatches.size())
            return false;

        for (const auto& lhsBatch : lhsBatches)
        {
            auto rhsBatch = rhs.GetBatch(lhsBatch.GetReference());
            if (!rhsBatch.has_value() || lhsBatch != rhsBatch)
                return false;
        }
        return true;
    }

    bool operator==(const ProductPtr& lhs, const ProductPtr& rhs) noexcept
    {
        if (!lhs && !rhs)
            return true;
        if (!lhs || !rhs)
            return false;
        return *lhs == *rhs;
    }
}