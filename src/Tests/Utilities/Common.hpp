#pragma once

#include "Domain/Product/Batch.hpp"


namespace Allocation::Tests
{
    /// @brief Создаёт партию поставки и позицию заказа.
    /// @param sku Артикул продукции.
    /// @param batchQty Количество продукции в партии заказа.
    /// @param lineQty Количество в продукции в позиции заказа.
    /// @return Пара: партия заказа - позиция заказа.
    std::pair<Domain::Batch, Domain::OrderLine> MakeBatchAndLine(
        const std::string& sku, size_t batchQty, size_t lineQty);

    /// @brief Генерирует случайный артикул.
    /// @param name Имя для генерации.
    /// @return Случайный артикул.
    std::string RandomSku(const std::string& name = "");

    /// @brief Генерирует случайную ссылку на партию.
    /// @param name Имя для генерации.
    /// @return Случайная ссылка на партию.
    std::string RandomBatchRef(const std::string& name = "");

    /// @brief Генерирует случайный идентификатор заказа.
    /// @param name Имя для генерации.
    /// @return Случайный идентификатор заказа.
    std::string RandomOrderId(const std::string& name = "");
}