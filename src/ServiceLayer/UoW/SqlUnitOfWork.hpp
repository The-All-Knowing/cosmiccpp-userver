#pragma once

#include <userver/storages/postgres/cluster.hpp>

#include "AbstractUnitOfWork.hpp"
#include "Adapters/Repository/SqlRepository.hpp"


namespace Allocation::ServiceLayer::UoW
{
    /// @brief Реализация единицы работы для SQL хранилища.
    class SqlUnitOfWork final : public AbstractUnitOfWork
    {
    public:
        SqlUnitOfWork(userver::storages::postgres::ClusterPtr pgCluster);

        void Commit() override;

        /// @brief Откатывает внесённые изменения.
        /// @details После отката изменений запускает новую транзакцию.
        void RollBack() override;

    private:
        userver::storages::postgres::ClusterPtr _pgCluster;
        userver::storages::postgres::Transaction _transaction;
        Adapters::Repository::SqlRepository _repository;
    };
}