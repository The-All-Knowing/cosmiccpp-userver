#include "MessageBus.hpp"

#include <userver/logging/log.hpp>


namespace Allocation::ServiceLayer
{
    MessageBus& MessageBus::Instance()
    {
        static MessageBus instance;
        return instance;
    }

    void MessageBus::Handle(Domain::IMessagePtr message, ServiceLayer::UoW::IUnitOfWork& uow)
    {
        std::queue<Domain::IMessagePtr> queue;
        queue.push(message);

        while (!queue.empty())
        {
            auto message = queue.front();
            queue.pop();
            if (message->GetType() == Domain::IMessage::Type::Command)
            {
                if (!_commandHandlers.contains(typeid(*message)))
                    throw std::runtime_error(
                        fmt::format("The {} command doesn`t have a handler", message->Name()));

                handleCommand(
                    uow, std::static_pointer_cast<Domain::AbstractCommand>(message), queue);
            }
            else if (message->GetType() == Domain::IMessage::Type::Event &&
                     _eventHandlers.contains(typeid(*message)))
            {
                handleEvent(uow, std::static_pointer_cast<Domain::AbstractEvent>(message), queue);
            }
        }
    }

    void MessageBus::ClearHandlers() noexcept
    {
        _eventHandlers.clear();
        _commandHandlers.clear();
    }

    void MessageBus::handleEvent(ServiceLayer::UoW::IUnitOfWork& uow, Domain::EventPtr event,
        std::queue<Domain::IMessagePtr>& queue) noexcept
    {
        for (auto& handler : _eventHandlers[typeid(*event)])
        {
            try
            {
                LOG_DEBUG("Handling event {} with handler {}", event->Name(),
                    handler.target_type().name());
                handler(uow, event);
                for (auto& newMessage : uow.GetNewMessages())
                    queue.push(newMessage);
            }
            catch (const std::exception&)
            {
                LOG_ERROR("Exception handling event {}", event->Name());
            }
        }
    }

    void MessageBus::handleCommand(ServiceLayer::UoW::IUnitOfWork& uow, Domain::CommandPtr command,
        std::queue<Domain::IMessagePtr>& queue)
    {
        LOG_DEBUG("handling command {}", command->Name());
        try
        {
            _commandHandlers.at(typeid(*command))(uow, command);
            for (auto& newMessage : uow.GetNewMessages())
                queue.push(newMessage);
        }
        catch (const std::exception&)
        {
            LOG_ERROR("Exception handling command {}", command->Name());
            throw;
        }
    }
}