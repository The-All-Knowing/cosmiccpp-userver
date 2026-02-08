#pragma once

#include <userver/utils/uuid7.hpp>

#include "Domain/Product/Batch.hpp"


namespace Allocation::Tests
{
    /// @brief Создаёт партию поставки и позицию заказа.
    /// @param sku Артикул продукции.
    /// @param batchQty Количество продукции в партии заказа.
    /// @param lineQty Количество в продукции в позиции заказа.
    /// @return Пара: партия заказа - позиция заказа.
    [[nodiscard]] inline std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& sku, size_t batchQty, size_t lineQty)
    {
        return {Domain::Batch("batch-001", sku, batchQty),
            Domain::OrderLine("order-123", sku, lineQty)};
    }

    /// @brief Генерирует случайный артикул.
    /// @param name Имя для генерации.
    /// @return Случайный артикул.
    [[nodiscard]] inline std::string RandomSku(const std::string& name = "")
    {
        return "sku-" + name + "-" + userver::utils::generators::GenerateUuidV7();
    }

    /// @brief Генерирует случайную ссылку на партию.
    /// @param name Имя для генерации.
    /// @return Случайная ссылка на партию.
    [[nodiscard]] inline std::string RandomBatchRef(const std::string& name = "")
    {
        return "batch-" + name + "-" + userver::utils::generators::GenerateUuidV7();
    }

    /// @brief Генерирует случайный идентификатор заказа.
    /// @param name Имя для генерации.
    /// @return Случайный идентификатор заказа.
    [[nodiscard]] inline std::string RandomOrderId(const std::string& name = "")
    {
        return "order-" + name + "-" + userver::utils::generators::GenerateUuidV7();
    }
}