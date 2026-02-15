#pragma once

#include <userver/components/component_base.hpp>

#include "Domain/Events.hpp"
#include "ServiceLayer/UoW/IUnitOfWork.hpp"
#include "ServiceLayer/CommandFactory.hpp"


namespace Allocation::ServiceLayer
{
    /// @brief Концепция для обработчиков событий конкретного типа.
    template <typename F, typename T>
    concept EventHandlerFor =
        std::derived_from<T, Domain::AbstractEvent> &&
        (std::is_invocable_v<F, ServiceLayer::UoW::IUnitOfWork&, std::shared_ptr<T>> ||
            std::is_invocable_v<F, std::shared_ptr<T>>);

    /// @brief Концепция для обработчиков команд конкретного типа.
    template <typename F, typename T>
    concept CommandHandlerFor =
        std::derived_from<T, Domain::AbstractCommand> &&
        std::is_invocable_v<F, ServiceLayer::UoW::IUnitOfWork&, std::shared_ptr<T>>;


    /// @brief Шина сообщений для обработки событий и команд.
    class MessageBus final
    {
        using EventHandler =
            std::function<void(ServiceLayer::UoW::IUnitOfWork&, Domain::EventPtr)>;
        using CommandHandler =
            std::function<void(ServiceLayer::UoW::IUnitOfWork&, Domain::CommandPtr)>;

    public:
        static MessageBus& Instance();

        template<typename CommandDto>
        void HandleMessage(const CommandDto& command, ServiceLayer::UoW::IUnitOfWork& uow)
        {
            Handle(MakeCommand<CommandDto>(command), uow);
        }

        /// @brief Обрабатывает входящее доменное сообщение.
        /// @param message Доменное сообщение.
        /// @param uow Единица работы для обработки сообщения.
        void Handle(Domain::IMessagePtr message, ServiceLayer::UoW::IUnitOfWork& uow);

        /// @brief Подписывает обработчик на событие конкретного типа.
        /// @tparam T Тип события, производный от Domain::Events::AbstractEvent.
        /// @tparam F Тип обработчика.
        /// @param handler Обработчик события.
        template <typename T, typename F>
            requires EventHandlerFor<F, T>
        void SubscribeToEvent(F&& handler) noexcept
        {
            auto& handlers = _eventHandlers[typeid(T)];
            handlers.emplace_back(
                [h = std::forward<F>(handler)](
                    ServiceLayer::UoW::IUnitOfWork& uow, Domain::EventPtr event)
                {
                    if constexpr (std::is_invocable_v<F, ServiceLayer::UoW::IUnitOfWork&,
                                      std::shared_ptr<T>>)
                        h(uow, std::static_pointer_cast<T>(event));
                    else if constexpr (std::is_invocable_v<F, std::shared_ptr<T>>)
                        h(std::static_pointer_cast<T>(event));
                });
        }

        /// @brief Устанавливает обработчик для команды конкретного типа.
        /// @tparam T Тип команды, производный от Domain::Commands::AbstractCommand.
        /// @tparam F Тип функции-обработчика.
        /// @param handler Обработчик команды.
        template <typename T, typename F>
            requires CommandHandlerFor<F, T>
        void SetCommandHandler(F&& handler) noexcept
        {
            _commandHandlers[typeid(T)] =
                [h = std::forward<F>(handler)](ServiceLayer::UoW::IUnitOfWork& uow,
                    Domain::CommandPtr cmd) { h(uow, std::static_pointer_cast<T>(cmd)); };
        }

        /// @brief Очищает все зарегистрированные обработчики событий и команд.
        void ClearHandlers() noexcept;

    private:
        /// @brief Обрабатывает входящее событие.
        /// @param uow Единица работы для обработки события.
        /// @param event Доменное событие.
        /// @param queue Очередь для новых сообщений.
        void handleEvent(ServiceLayer::UoW::IUnitOfWork& uow, Domain::EventPtr event,
            std::queue<Domain::IMessagePtr>& queue) noexcept;

        /// @brief Обрабатывает входящую команду.
        /// @param uow Единица работы для обработки команды.
        /// @param command Доменная команда.
        /// @param queue Очередь для новых сообщений.
        void handleCommand(ServiceLayer::UoW::IUnitOfWork& uow, Domain::CommandPtr command,
            std::queue<Domain::IMessagePtr>& queue);

        std::unordered_map<std::type_index, std::vector<EventHandler>> _eventHandlers;
        std::unordered_map<std::type_index, CommandHandler> _commandHandlers;
    };
}