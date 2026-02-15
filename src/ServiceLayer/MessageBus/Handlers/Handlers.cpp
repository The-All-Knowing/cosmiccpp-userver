#include "Handlers.hpp"

#include <Allocation/sql_queries.hpp>

#include "Common.hpp"
#include "Domain/Product/Product.hpp"
#include "ServiceLayer/Exceptions.hpp"


namespace Allocation::ServiceLayer::Handlers
{
    void AddBatch(UoW::IUnitOfWork& uow, std::shared_ptr<Domain::CreateBatch> message)
    {
        auto& repo = uow.GetProductRepository();
        auto product = repo.Get(message->sku);
        if (!product)
        {
            product = std::make_shared<Domain::Product>(message->sku);
            repo.Add(product);
        }
        product->AddBatch(
            Allocation::Domain::Batch(message->ref, message->sku, message->qty, message->eta));
        uow.Commit();
    }

    void Allocate(UoW::IUnitOfWork& uow, std::shared_ptr<Domain::Allocate> command)
    {
        Domain::OrderLine line(command->orderid, command->sku, command->qty);
        auto& repo = uow.GetProductRepository();
        auto product = repo.Get(command->sku);
        if (!product)
            throw Exceptions::InvalidSku(command->sku);
        product->Allocate(line);
        uow.Commit();
    }

    void Reallocate(UoW::IUnitOfWork& uow, std::shared_ptr<Domain::Deallocated> event)
    {
        Allocate(uow, Make<Domain::Allocate>(event->orderid, event->sku, event->qty));
    }

    void ChangeBatchQuantity(
        UoW::IUnitOfWork& uow, std::shared_ptr<Domain::ChangeBatchQuantity> command)
    {
        auto& repo = uow.GetProductRepository();
        auto product = repo.GetByBatchRef(command->ref);
        if (!product)
            throw std::invalid_argument("Invalid batch reference.");
        product->ChangeBatchQuantity(command->ref, command->qty);
        uow.Commit();
    }

    void AddAllocationToReadModel(userver::storages::postgres::ClusterPtr cluster,
        std::shared_ptr<Domain::Allocated> event)
    {
        cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
            Allocation::sql::kInsertallocationsview, event->orderid, event->sku, event->batchref);
    }

    void RemoveAllocationFromReadModel(userver::storages::postgres::ClusterPtr cluster,
        std::shared_ptr<Domain::Deallocated> event)
    {
        cluster->Execute(userver::storages::postgres::ClusterHostType::kMaster,
            Allocation::sql::kDeleteallocationsview, event->orderid, event->sku);
    }
}