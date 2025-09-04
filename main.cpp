#include "skip_list.hpp"
#include <iostream>
#include <cstdlib>

int main() {
    std::cout << "Skip list\n\n";
    
    // Запускаем Google Test
    std::cout << "Запуск Google Test...\n";
    return system("./skip_list_tests");
}
