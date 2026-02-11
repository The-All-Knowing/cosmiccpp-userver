#pragma once

#include "Adapters/Repository/TrackingRepository.hpp"
#include "IUnitOfWork.hpp"


namespace Allocation::ServiceLayer::UoW
{
    /// @brief Абстрактный базовый класс для реализации паттерна "Единица работы".
    /// @note Отвечает за контроль транзакций и отслеживание изменений в агрегатах через
    /// TrackingRepository.
    class AbstractUnitOfWork : public IUnitOfWork
    {
    public:
        /// @brief Подтверждает изменения.
        void Commit() override;

        /// @brief Откатывает изменения.
        void RollBack() override;

        /// @brief Проверяет, были ли изменения зафиксированы.
        /// @return true, если изменения зафиксированы, иначе false.
        bool IsCommitted() const override ;

        /// @brief Возвращает репозиторий для работы с агрегатами-продуктами.
        [[nodiscard]] Domain::IRepository& GetProductRepository() override;

        /// @brief Возвращает новые сообщения, сгенерированные продуктами
        /// в рамках текущей единицы работы.
        /// @return Сообщения сгенерированные в отслеживаемых агрегатах.
        [[nodiscard]] std::vector<Domain::IMessagePtr> GetNewMessages() override;

    protected:
        [[nodiscard]] std::vector<Domain::ProductPtr> getUpdatedProducts() const;

        void setRepository(Domain::IRepository& repository);

        virtual void commit() = 0;
        
        virtual void rollBack() = 0;

    private:
        std::unique_ptr<Adapters::Repository::TrackingRepository> _trackingRepository;
        std::vector<Domain::IMessagePtr> _newMessages;
        bool _isCommitted{false};
    };
}
