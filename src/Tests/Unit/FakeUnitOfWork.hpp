#pragma once

#include "FakeRepository.hpp"
#include "ServiceLayer/UoW/AbstractUnitOfWork.hpp"


namespace Allocation::Tests
{
    /// @brief Фейковая реализация единицы работы для тестирования.
    class FakeUnitOfWork final : public ServiceLayer::UoW::AbstractUnitOfWork
    {
    public:
        FakeUnitOfWork()
        {
            setRepository(_repo);
        }

        /// @brief Конструктор.
        FakeUnitOfWork(FakeRepository& repo) : AbstractUnitOfWork()
        {
            setRepository(repo);
        }

    private:
        FakeRepository _repo;
    };
}
