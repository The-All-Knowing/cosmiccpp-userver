#pragma once

#include "IUnitOfWork.hpp"
#include "Adapters/Repository/TrackingRepository.hpp"


namespace Allocation::ServiceLayer::UoW
{
    /// @brief Абстрактный базовый класс для реализации паттерна "Единица работы".
    /// @note Отвечает за контроль транзакций и отслеживание изменений в агрегатах через TrackingRepository.
    class AbstractUnitOfWork : public IUnitOfWork
    {
    public:
        AbstractUnitOfWork(Domain::IRepository& repository);

        /// @brief Подтверждает изменения.
        void Commit() override;

        /// @brief Откатывает изменения.
        void RollBack() override;

        /// @brief Проверяет, были ли изменения зафиксированы.
        /// @return true, если изменения зафиксированы, иначе false.
        bool IsCommitted() const noexcept override;

        /// @brief Возвращает репозиторий для работы с агрегатами-продуктами.
        [[nodiscard]] Domain::IRepository& GetProductRepository() override;

        /// @brief Возвращает новые сообщения, сгенерированные продуктами
        /// в рамках текущей единицы работы.
        /// @return Сообщения сгенерированные в отслеживаемых агрегатах.
        [[nodiscard]] std::vector<Domain::IMessagePtr> GetNewMessages() noexcept override;

    protected:
        [[nodiscard]] std::vector<Domain::ProductPtr> GetUpdatedProducts() const noexcept;

    private:
        Adapters::Repository::TrackingRepository _trackingRepository;
        bool _isCommitted{false};
    };
}
