#pragma once

#include <chrono>
#include <concepts>
#include <memory>


namespace Allocation::Domain
{
    struct IMessage;
}

namespace Allocation
{
    /// @brief Перегружает оператор + для добавления дней к дате.
    /// @param ymd Дата в формате std::chrono.
    /// @param days Количество дней для добавления.
    /// @return Новая корректная дата с добавленными днями.
    [[nodiscard]] std::chrono::year_month_day operator+(
        const std::chrono::year_month_day& ymd, const std::chrono::days& days) noexcept
    {
        return std::chrono::year_month_day{std::chrono::sys_days{ymd} + days};
    }

    /// @brief Перегружает оператор + для добавления дней к дате.
    /// @param days Количество дней для добавления.
    /// @param ymd Дата в формате std::chrono.
    /// @return Новая корректная дата с добавленными днями.
    [[nodiscard]] std::chrono::year_month_day operator+(
        const std::chrono::days& days, const std::chrono::year_month_day& ymd) noexcept
    {
        return ymd + days;
    }

    /// @brief Концепт для типа в Make.
    template <typename T>
    concept Message = std::derived_from<T, Domain::IMessage>;

    /// @brief Создаёт событие или команду.
    /// @tparam T Тип сообщения (команда или событие).
    /// @tparam Args Типы аргументов конструктора T.
    /// @param args Аргументы для конструктора T.
    /// @return Cообщение типа T.
    template <Message T, typename... Args>
    std::shared_ptr<T> Make(Args&&... args)
    {
        static_assert(!std::is_abstract_v<T>,
            "Make<T>: T must be a non-abstract message type (command or event).");
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}