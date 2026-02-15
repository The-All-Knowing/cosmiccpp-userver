#pragma once

#include "Ports/IMessage.hpp"


namespace Allocation::Domain
{
    /// @brief Абстрактная команда.
    struct AbstractCommand : public IMessage
    {
        /// @brief Деструктор.
        ~AbstractCommand() override = default;

        /// @brief Возвращает тип сообщения.
        /// @return Тип команды.
        [[nodiscard]] Type GetType() const override;

    protected:
        AbstractCommand() = default;
    };

    using CommandPtr = std::shared_ptr<AbstractCommand>;

    /// @brief Команда "Распределить позицию заказа".
    struct Allocate final : public AbstractCommand
    {
        /// @brief Конструктор.
        /// @param orderid Идентификатор заказа клиента.
        /// @param sku Артикул товара для распределения.
        /// @param qty Количество распределяемого товара.
        Allocate(const std::string& orderid, const std::string& sku, size_t qty);

        /// @brief Возвращает имя команды.
        /// @return Имя команды.
        [[nodiscard]] std::string Name() const override;

        std::string orderid;
        std::string sku;
        size_t qty;
    };

    /// @brief Команда "Изменить количество товара в партии заказа".
    struct ChangeBatchQuantity final : public AbstractCommand
    {
        /// @brief Конструктор.
        /// @param ref Ссылка на партию поставки.
        /// @param qty Новое количество товара.
        ChangeBatchQuantity(const std::string& ref, size_t qty);

        /// @brief Возвращает имя команды.
        /// @return Имя команды.
        [[nodiscard]] std::string Name() const override;

        std::string ref;
        size_t qty;
    };

    /// @brief Команда "Создать партию поставки".
    struct CreateBatch final : public AbstractCommand
    {
        /// @brief Конструктор.
        /// @param ref Ссылка новой партии заказа.
        /// @param sku Артикул товара новой партии.
        /// @param qty Количество товара в партии.
        /// @param eta Ожидаемая дата поступления партии.
        CreateBatch(const std::string& ref, const std::string& sku, size_t qty,
            std::optional<std::chrono::year_month_day> eta = std::nullopt);

        /// @brief Возвращает имя команды.
        /// @return Имя команды.
        [[nodiscard]] std::string Name() const override;

        std::string ref;
        std::string sku;
        size_t qty;
        std::optional<std::chrono::year_month_day> eta;
    };
}