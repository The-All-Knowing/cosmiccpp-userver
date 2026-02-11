#pragma once

#include "Batch.hpp"
#include "Domain/Ports/IMessage.hpp"


namespace Allocation::Domain
{
    /// @brief Агрегат-продукт, содержит партии заказа с общим артикулом продукции. 
    /// Реализует бизнес-логику распределения позиций заказа в партиях заказа.
    class Product final
    {
    public:
        /// @brief Создаёт агрегат-продукт.
        /// @param sku Артикул продукции.
        /// @param batches Список партий заказа продукции.
        /// @param versionNumber Номер версии агрегата.
        explicit Product(const std::string& sku, const std::vector<Batch>& batches = {},
            size_t versionNumber = 0);

        /// @brief Добавляет партию поставки к агрегату.
        /// @param batch Добавляемая партия заказа.
        /// @throw std::invalid_argument Выбрасывается, если артикул партии не совпадает с артикулом
        /// агрегата.
        void AddBatch(const Batch& batch);

        /// @brief Добавляет несколько партий заказа к агрегату.
        /// @param batches Добавляемые партии заказа.
        /// @throw std::invalid_argument Выбрасывается, если артикул партии не совпадает с артикулом
        /// агрегата.
        void AddBatches(const std::vector<Batch>& batches);

        /// @brief Распределяет позицию заказа в партии заказа агрегата.
        /// @param line Позиция заказа для распределения.
        /// @throw std::invalid_argument Выбрасывается, если артикул позиции заказа не совпадает с
        /// артикулом продукции агрегата.
        /// @return Ссылка на партию поставки, если распределение успешно, иначе std::nullopt.
        std::optional<std::string> Allocate(const OrderLine& line);

        /// @brief Изменяет количество продукции в партии заказа.
        /// @param ref Ссылка на партию поставки.
        /// @param qty Новое количество продукции.
        /// @return true - если партия найдена и количество изменено, иначе false.
        void ChangeBatchQuantity(const std::string& ref, size_t qty);

        /// @brief Возвращает все партии заказа агрегата.
        /// @return Партии заказа агрегата.
        [[nodiscard]] std::vector<Batch> GetBatches() const noexcept;

        /// @brief Возвращает партию поставки по её ссылке.
        /// @param reference Ссылка на партию поставки.
        /// @return Партия заказа, если найдена, иначе std::nullopt.
        [[nodiscard]] std::optional<Batch> GetBatch(const std::string& reference) const noexcept;

        /// @brief Возвращает номер версии агрегата.
        /// @return Номер версии.
        [[nodiscard]] int64_t GetVersion() const noexcept;

        /// @brief Возвращает артикул продукции агрегата.
        /// @return Артикул продукции.
        [[nodiscard]] std::string GetSKU() const noexcept;

        /// @brief Возвращает сообщения, сгенерированные во время выполнения бизнес-логики.
        /// @return Доменные сообщения.
        [[nodiscard]] const std::vector<Domain::IMessagePtr>& Messages() const noexcept;

        /// @brief Очищает сообщения агрегата.
        void ClearMessages() noexcept;

    private:
        std::string _sku;
        std::unordered_map<std::string, Batch> _referenceByBatches;
        std::vector<Domain::IMessagePtr> _messages;
        size_t _versionNumber;
    };

    using ProductPtr = std::shared_ptr<Product>;

    /// @brief Проверяет равенство двух агрегатов.
    /// @param lhs Левый операнд.
    /// @param rhs Правый операнд.
    /// @return true — если агрегаты равны, иначе false.
    bool operator==(const Product& lhs, const Product& rhs) noexcept;

    /// @brief Проверяет равенство двух агрегатов.
    /// @param lhs Левый операнд.
    /// @param rhs Правый операнд.
    /// @return true — если оба указателя nullptr или агрегаты равны, иначе false.
    bool operator==(const ProductPtr& lhs, const ProductPtr& rhs) noexcept;
}
