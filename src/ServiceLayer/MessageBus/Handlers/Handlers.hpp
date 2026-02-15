#pragma once

#include <userver/storages/postgres/cluster.hpp>

#include "Adapters/Notification/EmailSenderStub.hpp"
#include "Adapters/Redis/RedisEventPublisher.hpp"
#include "Domain/Commands.hpp"
#include "Domain/Events.hpp"
#include "NotificationHandler.hpp"
#include "PublisherHandler.hpp"
#include "ServiceLayer/UoW/IUnitOfWork.hpp"




namespace Allocation::ServiceLayer::Handlers
{
    /// @brief Добавляет новую партию поставки.
    /// @param uow Единица работы.
    /// @param command Команда "Создать партию поставки".
    void AddBatch(UoW::IUnitOfWork& uow, std::shared_ptr<Domain::CreateBatch> command);

    /// @brief Распределяет позицию заказа в партии заказа.
    /// @param uow Единица работы.
    /// @param command Команда "Распределить позицию заказа".
    void Allocate(UoW::IUnitOfWork& uow, std::shared_ptr<Domain::Allocate> command);

    /// @brief Изменяет количество товара в партии заказа.
    /// @param uow Единица работы.
    /// @param command Команда "Изменить количество товара в партии заказа".
    /// @throw std::invalid_argument Выбрасывается, если передана неверная ссылка на партию
    /// поставки.
    void ChangeBatchQuantity(
        UoW::IUnitOfWork& uow, std::shared_ptr<Domain::ChangeBatchQuantity> command);

    /// @brief Перераспределяет товар из одной партии в другую.
    /// @param uow Единица работы.
    /// @param event Событие "Отменено распределение позиции заказа".
    void Reallocate(UoW::IUnitOfWork& uow, std::shared_ptr<Domain::Deallocated> event);

    /// @brief Отправляет уведомление по электронной почте,
    /// по событию "Нет в наличии товара".
    using SendOutOfStockNotification = NotificationHandler<Domain::OutOfStock,
        Allocation::Adapters::Notification::EmailSenderStub>;

    /*
        /// @brief Публикует событие в Redis "Распределена позиция заказа".
        using PublishAllocatedEvent = PublisherHandler<Domain::Events::Allocated,
            Allocation::Adapters::Redis::RedisEventPublisher<Domain::Events::Allocated>>;
    */

    /// @brief Добавляет в модель чтения распределённую позицию заказа.
    /// @param uow Единица работы.
    /// @param event Событие "Распределена позиция заказа".
    void AddAllocationToReadModel(userver::storages::postgres::ClusterPtr cluster,
        std::shared_ptr<Domain::Allocated> event);

    /// @brief Удаляет в модели чтения распределённую позицию заказа.
    /// @param uow Единица работы.
    /// @param event Событие "Отменено распределение позиции заказа".
    void RemoveAllocationFromReadModel(userver::storages::postgres::ClusterPtr cluster,
        std::shared_ptr<Domain::Deallocated> event);
}