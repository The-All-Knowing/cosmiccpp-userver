#include "SqlUnitOfWork.hpp"

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>


namespace Allocation::ServiceLayer::UoW
{
    SqlUnitOfWork::SqlUnitOfWork()
        : _transaction(userver::component<userver::storages::postgres::Cluster>().GetTransaction()),
          _repository(_transaction),
          AbstractUnitOfWork(_repository)
    {
    }

    void SqlUnitOfWork::Commit()
    {
        AbstractUnitOfWork::Commit();
        for (const auto& product : GetUpdatedProducts())
            _repository.IncrementVersion(product);
        _transaction.Commit();
        _transaction = userver::component<userver::storages::postgres::Cluster>().GetTransaction();
    }

    void SqlUnitOfWork::RollBack()
    {
        _transaction.Rollback();
        AbstractUnitOfWork::RollBack();
        _transaction = userver::component<userver::storages::postgres::Cluster>().GetTransaction();
    }
}