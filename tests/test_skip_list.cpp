#include <gtest/gtest.h>
#include "../skip_list.hpp"
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <stdexcept>

class SkipListTest : public ::testing::Test {
protected:
    SkipList<int> list;
    
    void SetUp() override {
        list = SkipList<int>();
    }
};

// тест конструктора
TEST_F(SkipListTest, DefaultConstructor) {
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

// тест вставки
TEST_F(SkipListTest, InsertElements) {
    list.insert(5);
    list.insert(3);
    list.insert(7);
    
    EXPECT_EQ(list.size(), 3);
    EXPECT_FALSE(list.empty());
}

// тест поиска
TEST_F(SkipListTest, FindElements) {
    list.insert(5);
    list.insert(3);
    list.insert(7);
    
    auto it = list.find(5);
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, 5);
    
    it = list.find(99);
    EXPECT_EQ(it, list.end());
}

// тест удаления
TEST_F(SkipListTest, EraseElements) {
    list.insert(5);
    list.insert(3);
    list.insert(7);
    
    EXPECT_EQ(list.size(), 3);
    
    list.erase(5);
    EXPECT_EQ(list.size(), 2);
    
    auto it = list.find(5);
    EXPECT_EQ(it, list.end());
}

// тест копирования
TEST_F(SkipListTest, CopyConstructor) {
    list.insert(5);
    list.insert(3);
    list.insert(7);
    
    SkipList<int> copy = list;
    EXPECT_EQ(copy.size(), list.size());
    EXPECT_EQ(copy.size(), 3);
}

// тест перемещения
TEST_F(SkipListTest, MoveConstructor) {
    list.insert(5);
    list.insert(3);
    list.insert(7);
    
    SkipList<int> moved = std::move(list);
    EXPECT_EQ(moved.size(), 3);
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
}

// тест очистки
TEST_F(SkipListTest, Clear) {
    list.insert(5);
    list.insert(3);
    list.insert(7);
    
    EXPECT_EQ(list.size(), 3);
    list.clear();
    EXPECT_EQ(list.size(), 0);
    EXPECT_TRUE(list.empty());
}

// тест итераторов
TEST_F(SkipListTest, Iterators) {
    list.insert(5);
    list.insert(3);
    list.insert(7);
    
    std::vector<int> values;
    for (const auto& item : list) {
        values.push_back(item);
    }
    
    EXPECT_EQ(values.size(), 3);
    EXPECT_EQ(values[0], 3);
    EXPECT_EQ(values[1], 5);
    EXPECT_EQ(values[2], 7);
}

// тест оператора присваивания
TEST_F(SkipListTest, AssignmentOperator) {
    list.insert(5);
    list.insert(3);
    list.insert(7);
    
    SkipList<int> other;
    other.insert(10);
    other.insert(20);
    
    other = list;
    EXPECT_EQ(other.size(), 3);
    EXPECT_EQ(list.size(), 3);
}

// тест с большим набором данных
TEST_F(SkipListTest, LargeDataSet) {
    for (int i = 0; i < 100; ++i) {
        list.insert(i);
    }
    
    EXPECT_EQ(list.size(), 100);
    
    for (int i = 0; i < 100; ++i) {
        auto it = list.find(i);
        EXPECT_NE(it, list.end());
        EXPECT_EQ(*it, i);
    }
}

// тест дублирующихся элементов
TEST_F(SkipListTest, DuplicateElements) {
    list.insert(5);
    list.insert(5);
    list.insert(5);
    
    EXPECT_EQ(list.size(), 1); // дубликаты не добавляются
    
    auto it = list.find(5);
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, 5);
}

// тест перфоманса
TEST_F(SkipListTest, PerformanceTest) {
    const int N = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // вставка
    for (int i = 0; i < N; ++i) {
        list.insert(i);
    }
    
    auto insert_end = std::chrono::high_resolution_clock::now();
    
    // поиск
    for (int i = 0; i < N; i += 100) {
        auto it = list.find(i);
        EXPECT_NE(it, list.end());
    }
    
    auto search_end = std::chrono::high_resolution_clock::now();
    
    // удаление
    for (int i = 0; i < N; i += 100) {
        list.erase(i);
    }
    
    auto delete_end = std::chrono::high_resolution_clock::now();
    
    auto insert_time = std::chrono::duration_cast<std::chrono::milliseconds>(insert_end - start);
    auto search_time = std::chrono::duration_cast<std::chrono::milliseconds>(search_end - insert_end);
    auto delete_time = std::chrono::duration_cast<std::chrono::milliseconds>(delete_end - search_end);
    
    // проверяем, что операции выполняются за разумное время (< 1 секунды)
    EXPECT_LT(insert_time.count(), 1000);
    EXPECT_LT(search_time.count(), 1000);
    EXPECT_LT(delete_time.count(), 1000);
    
    std::cout << "Performance test results:\n";
    std::cout << "Insert " << N << " elements: " << insert_time.count() << "ms\n";
    std::cout << "Search " << N/100 << " elements: " << search_time.count() << "ms\n";
    std::cout << "Delete " << N/100 << " elements: " << delete_time.count() << "ms\n";
}

// тест обработки ошибок итераторов
TEST_F(SkipListTest, IteratorErrorHandling) {

    SkipList<int> empty_list;
    
    // проверяем, что end() итератор не может быть разыменован
    auto end_it = empty_list.end();
    EXPECT_EQ(end_it, empty_list.begin());
    
    // добавляем элемент и проверяем корректную работу
    list.insert(42);
    auto it = list.begin();
    EXPECT_EQ(*it, 42);
    EXPECT_EQ(it.operator->(), &(*it));
    
    // проверяем инкремент
    ++it;
    EXPECT_EQ(it, list.end());
}

// тест крайних случаев
TEST_F(SkipListTest, EdgeCases) {
    // тест с отрицательными числами
    list.insert(-5);
    list.insert(-10);
    list.insert(-1);
    
    std::vector<int> values;
    for (const auto& item : list) {
        values.push_back(item);
    }
    
    EXPECT_EQ(values.size(), 3);
    EXPECT_EQ(values[0], -10);
    EXPECT_EQ(values[1], -5);
    EXPECT_EQ(values[2], -1);
    
   
    list.insert(0);
    auto it = list.find(0);
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, 0);
    
    
    list.insert(INT_MAX);
    list.insert(INT_MIN);
    
    it = list.find(INT_MAX);
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, INT_MAX);
}

// тест случайных данных
TEST_F(SkipListTest, RandomDataTest) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000);
    
    std::vector<int> random_values;
    for (int i = 0; i < 100; ++i) {
        int val = dis(gen);
        random_values.push_back(val);
        list.insert(val);
    }
    
    // проверяем, что все вставленные значения найдены
    for (int val : random_values) {
        auto it = list.find(val);
        EXPECT_NE(it, list.end());
        EXPECT_EQ(*it, val);
    }
    
    // проверяем сортировку
    std::vector<int> sorted_values;
    for (const auto& item : list) {
        sorted_values.push_back(item);
    }
    
    EXPECT_TRUE(std::is_sorted(sorted_values.begin(), sorted_values.end()));
}

// Тест концептов (проверка компиляции с различными типами)
TEST_F(SkipListTest, ConceptValidation) {
   
    SkipList<int> int_list;
    int_list.insert(42);
    EXPECT_EQ(int_list.size(), 1);
    
    
    SkipList<double> double_list;
    double_list.insert(3.14);
    EXPECT_EQ(double_list.size(), 1);
    
   
    SkipList<std::string> string_list;
    string_list.insert("test");
    EXPECT_EQ(string_list.size(), 1);
}

// тест со строками
class SkipListCustomTypeTest : public ::testing::Test {
protected:
    SkipList<std::string> list;
    
    void SetUp() override {
        list = SkipList<std::string>();
    }
};

TEST_F(SkipListCustomTypeTest, StringType) {
    list.insert("hello");
    list.insert("world");
    list.insert("test");
    
    EXPECT_EQ(list.size(), 3);
    
    auto it = list.find("hello");
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, "hello");
}

// тест пользовательского типа
struct CustomType {
    int value;
    std::string name;
    
    CustomType() : value(0), name("") {} // Конструктор по умолчанию
    CustomType(int v, const std::string& n) : value(v), name(n) {}
    
    bool operator<(const CustomType& other) const {
        return value < other.value;
    }
    
    bool operator==(const CustomType& other) const {
        return value == other.value && name == other.name;
    }
};

class SkipListCustomObjectTest : public ::testing::Test {
protected:
    SkipList<CustomType> list;
    
    void SetUp() override {
        list = SkipList<CustomType>();
    }
};

TEST_F(SkipListCustomObjectTest, CustomObjectType) {
    list.insert(CustomType(3, "three"));
    list.insert(CustomType(1, "one"));
    list.insert(CustomType(2, "two"));
    
    EXPECT_EQ(list.size(), 3);
    
    auto it = list.find(CustomType(2, "two"));
    EXPECT_NE(it, list.end());
    EXPECT_EQ(it->value, 2);
    EXPECT_EQ(it->name, "two");
    
    // проверяем сортировку
    std::vector<int> values;
    for (const auto& item : list) {
        values.push_back(item.value);
    }
    
    EXPECT_EQ(values[0], 1);
    EXPECT_EQ(values[1], 2);
    EXPECT_EQ(values[2], 3);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

