#pragma once

#include <userver/logging/log.hpp>


namespace Allocation::Adapters::Notification
{
    /// @brief Заглушка отправителя email-уведомлений.
    class EmailSenderStub
    {
    public:
        /// @brief Иммитирует отправку email-уведомления.
        /// @param to Адрес получателя.
        /// @param message Текст сообщения.
        void operator()(const std::string& to, const std::string& message) const
        {
            LOG_INFO("Sending email to {}: {}", to, message);
        }
    };
}