#pragma once

#include "FakeRepository.hpp"
#include "ServiceLayer/UoW/AbstractUnitOfWork.hpp"


namespace Allocation::Tests
{
    /// @brief Фейковая реализация единицы работы для тестирования.
    class FakeUnitOfWork final : public ServiceLayer::UoW::AbstractUnitOfWork
    {
    public:
        FakeUnitOfWork() { setRepository(_repo); }

        FakeUnitOfWork(FakeRepository& repo) : AbstractUnitOfWork() { setRepository(repo); }

    protected:
        void commit() override {}

        void rollBack() override {}

    private:
        FakeRepository _repo;
    };
}
