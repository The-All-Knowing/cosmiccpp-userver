#pragma once

#include "Domain/Ports/IMessage.hpp"
#include "Domain/Ports/IRepository.hpp"


namespace Allocation::ServiceLayer::UoW
{
    /// @brief Интерфейс единицы работы.
    class IUnitOfWork
    {
    public:
        /// @brief Деструктор.
        virtual ~IUnitOfWork() = default;

        /// @brief Подтверждает изменения.
        virtual void Commit() = 0;

        /// @brief Откатывает изменения.
        virtual void RollBack() = 0;

        /// @brief Проверяет, были ли изменения зафиксированы.
        [[nodiscard]] virtual bool IsCommitted() const = 0;

        /// @brief Возвращает репозиторий хранилища продуктов.
        /// @return Репозиторий продуктов.
        [[nodiscard]] virtual Domain::IRepository& GetProductRepository() = 0;

        /// @brief Возвращает новые сообщения из обработанных агрегатов.
        /// @return Новые сообщения порождённые в агрегатах.
        [[nodiscard]] virtual std::vector<Domain::IMessagePtr> GetNewMessages() = 0;
    };
}