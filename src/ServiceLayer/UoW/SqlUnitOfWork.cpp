#include "SqlUnitOfWork.hpp"

#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>


namespace Allocation::ServiceLayer::UoW
{
    SqlUnitOfWork::SqlUnitOfWork(userver::storages::postgres::ClusterPtr pgCluster)
        : AbstractUnitOfWork(),
          _pgCluster(std::move(pgCluster)),
          _transaction(_pgCluster->Begin(userver::storages::postgres::Transaction::RW)),
          _repository(_transaction)
    {
        setRepository(_repository);
    }

    void SqlUnitOfWork::commit()
    {
        for (const auto& product : getSeenProducts())
            _repository.Flush(product);
        _transaction.Commit();
        _transaction = _pgCluster->Begin(userver::storages::postgres::Transaction::RW);
    }

    void SqlUnitOfWork::rollBack()
    {
        _transaction.Rollback();
        _transaction = _pgCluster->Begin(userver::storages::postgres::Transaction::RW);
    }
}