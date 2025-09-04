#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <memory>
#include <random>
#include <stdexcept>
#include <concepts>
#include <iterator>
#include <type_traits>

template<typename T>
concept LessThanComparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
};

template<typename T>
concept CopyConstructible = std::copy_constructible<T>;

template<typename T>
concept MoveConstructible = std::move_constructible<T>;

template<typename T>
concept Destructible = std::destructible<T>;

template<typename T>
concept SkipListElement = LessThanComparable<T> && 
                         CopyConstructible<T> && 
                         MoveConstructible<T> && 
                         Destructible<T>;

template<SkipListElement T>
class SkipList {
private:
    struct Node {
        T value;
        std::vector<std::shared_ptr<Node>> forward;
        
        Node(const T& val, size_t level) 
            : value(val), forward(level + 1, nullptr) {}
        
        Node(T&& val, size_t level) 
            : value(std::move(val)), forward(level + 1, nullptr) {}
    };

    static constexpr size_t MAX_LEVEL = 32;
    static constexpr double P = 0.25;
    
    std::shared_ptr<Node> head;
    size_t current_level;
    size_t size_;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;

    size_t random_level() {
        size_t level = 0;
        while (dist(rng) < P && level < MAX_LEVEL) {
            level++;
        }
        return level;
    }

public:
    // Итератор
    class Iterator {
    private:
        std::shared_ptr<Node> current;
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(std::shared_ptr<Node> node = nullptr) : current(node) {}

        reference operator*() {
            if (!current) {
                throw std::runtime_error("Dereferencing null iterator");
            }
            return current->value;
        }

        pointer operator->() {
            if (!current) {
                throw std::runtime_error("Dereferencing null iterator");
            }
            return &current->value;
        }

        Iterator& operator++() {
            if (current) {
                current = current->forward[0];
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

    // Конструкторы
    SkipList() : head(std::make_shared<Node>(T{}, MAX_LEVEL)), 
                 current_level(0), size_(0), 
                 rng(std::random_device{}()), 
                 dist(0.0, 1.0) {}

    SkipList(const SkipList& other) : SkipList() {
        for (const auto& item : other) {
            insert(item);
        }
    }

    SkipList(SkipList&& other) noexcept 
        : head(std::move(other.head)),
          current_level(other.current_level),
          size_(other.size_),
          rng(std::move(other.rng)),
          dist(std::move(other.dist)) {
        other.current_level = 0;
        other.size_ = 0;
    }

    // Операторы присваивания
    SkipList& operator=(const SkipList& other) {
        if (this != &other) {
            SkipList temp(other);
            std::swap(*this, temp);
        }
        return *this;
    }

    SkipList& operator=(SkipList&& other) noexcept {
        if (this != &other) {
            head = std::move(other.head);
            current_level = other.current_level;
            size_ = other.size_;
            rng = std::move(other.rng);
            dist = std::move(other.dist);
            other.current_level = 0;
            other.size_ = 0;
        }
        return *this;
    }

    // Основные операции
    void insert(const T& value) {
        std::vector<std::shared_ptr<Node>> update(MAX_LEVEL + 1, nullptr);
        auto current = head;

        for (int i = current_level; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (!current || current->value != value) {
            size_t new_level = random_level();
            if (new_level > current_level) {
                for (size_t i = current_level + 1; i <= new_level; i++) {
                    update[i] = head;
                }
                current_level = new_level;
            }

            auto new_node = std::make_shared<Node>(value, new_level);
            for (size_t i = 0; i <= new_level; i++) {
                new_node->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = new_node;
            }
            size_++;
        }
    }

    void insert(T&& value) {
        std::vector<std::shared_ptr<Node>> update(MAX_LEVEL + 1, nullptr);
        auto current = head;

        for (int i = current_level; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (!current || current->value != value) {
            size_t new_level = random_level();
            if (new_level > current_level) {
                for (size_t i = current_level + 1; i <= new_level; i++) {
                    update[i] = head;
                }
                current_level = new_level;
            }

            auto new_node = std::make_shared<Node>(std::move(value), new_level);
            for (size_t i = 0; i <= new_level; i++) {
                new_node->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = new_node;
            }
            size_++;
        }
    }

    bool erase(const T& value) {
        std::vector<std::shared_ptr<Node>> update(MAX_LEVEL + 1, nullptr);
        auto current = head;

        for (int i = current_level; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current && current->value == value) {
            for (size_t i = 0; i <= current_level; i++) {
                if (update[i]->forward[i] != current) {
                    break;
                }
                update[i]->forward[i] = current->forward[i];
            }

            while (current_level > 0 && !head->forward[current_level]) {
                current_level--;
            }
            size_--;
            return true;
        }
        return false;
    }

    Iterator find(const T& value) const {
        auto current = head;
        for (int i = current_level; i >= 0; i--) {
            while (current->forward[i] && current->forward[i]->value < value) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        if (current && current->value == value) {
            return Iterator(current);
        }
        return Iterator();
    }

    // Итераторы
    Iterator begin() const {
        return Iterator(head->forward[0]);
    }

    Iterator end() const {
        return Iterator();
    }

    // Размер и проверка на пустоту
    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    // Очистка
    void clear() {
        head = std::make_shared<Node>(T{}, MAX_LEVEL);
        current_level = 0;
        size_ = 0;
    }
};

#endif // SKIP_LIST_HPP
