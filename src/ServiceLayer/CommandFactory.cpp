#include "CommandFactory.hpp"


namespace Allocation::ServiceLayer
{
    Domain::CommandPtr MakeCommand(const Allocation::AddBatch::CreateBatch& commandDto)
    {
        std::chrono::year_month_day eta;
            if (const auto& src = commandDto.eta)
                eta = std::chrono::year_month_day{src->GetSysDays()};
        return std::make_shared<Domain::CreateBatch>(
            commandDto.ref, commandDto.sku, commandDto.qty, eta);
    }


    Domain::CommandPtr MakeCommand(const Allocation::Allocate::Allocate& commandDto)
    {
        return std::make_shared<Domain::Allocate>(
            commandDto.orderid, commandDto.sku, commandDto.qty);
    }
}