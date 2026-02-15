#pragma once

#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain
{
    /// @brief Абстрактное событие.
    struct AbstractEvent : public IMessage
    {
        /// @brief Деструктор.
        ~AbstractEvent() override = default;

        /// @brief Возвращает тип сообщения.
        /// @return Тип сообщения.
        [[nodiscard]] Type GetType() const override;
    };

    using EventPtr = std::shared_ptr<AbstractEvent>;

    /// @brief Событие "Распределена позиция заказа".
    struct Allocated final : public AbstractEvent
    {
        /// @brief Конструктор.
        /// @param orderid Идентификатор заказа клиента.
        /// @param sku Артикул товара товара заказа.
        /// @param batchref Ссылка на партию в которой находится заказ.
        /// @param qty Количество распределённого товара.
        Allocated(const std::string& orderid, const std::string& sku, size_t qty,
            const std::string& batchref);

        /// @brief Возвращает имя события.
        /// @return Имя события.
        [[nodiscard]] std::string Name() const override;

        std::string orderid;
        std::string sku;
        size_t qty;
        std::string batchref;
    };

    /// @brief Событие "Отменено распределение позиции заказа".
    struct Deallocated final : public AbstractEvent
    {
        /// @brief Конструктор.
        /// @param orderid Идентификатор заказа клиента.
        /// @param sku Артикул отменённого товара.
        /// @param qty Количество отменённого товара.
        Deallocated(const std::string& orderid, const std::string& sku, size_t qty);

        /// @brief Возвращает имя события.
        /// @return Имя события.
        [[nodiscard]] std::string Name() const override;

        std::string orderid;
        std::string sku;
        size_t qty;
    };

    /// @brief Событие "Нет в наличии товара".
    struct OutOfStock final : public AbstractEvent
    {
        /// @brief Конструктор.
        /// @param sku Артикул товара которого нет в наличии.
        OutOfStock(const std::string& sku);

        /// @brief Возвращает имя события.
        /// @return Имя события.
        [[nodiscard]] std::string Name() const override;

        std::string sku;
    };
}