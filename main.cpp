#include <iostream>
#include <iomanip>
#include "LazySequence.hpp"
#include "HeightMapGenerator.hpp"

// for testing the library in any desired way

void printChunk(const chunk& ch) {
    size_t size = ch.getSize();
    for (size_t row = 0; row < size; row++) {
        for (size_t col = 0; col < size; col++) {
            std::cout << std::fixed << std::setprecision(2) << ch[col][row] << "\t";
        }
        std::cout << std::endl;
    }
}

int main() {
    // Создаем последовательность Фибоначчи через create с функцией генерации
    ArraySequence<int> initial;
    initial.append(0);
    initial.append(1);
    
    initial.append(2);
    initial.append(5);
    initial.append(5);
    initial.append(63);
    initial.append(44);
    
    // auto fibSeq = LazySequence<int>::create(
    //     2, // арность - используем 2 предыдущих элемента
    //     [](ArraySequence<int>& prev) -> int {
    //         return prev[0] + prev[1]; // F(n) = F(n-1) + F(n-2)
    //     },
    //     initial
    // );
    // auto newSeq = fibSeq->where([](const int elem) -> bool {
    //     return elem % 2 == 0;
    // });

    auto seq = LazySequence<int>::create(initial);
    
    for (auto i = 0; i < seq->getSize(); i++) {
        std::cout << (*seq)[i] << " ";
    }
    std::cout << std::endl;

    
    std::cout << "dkjhsfdsk" << std::endl;
    auto seq1 = seq->insertAt(228, 3);

    for (auto i = 0; i < seq1->getSize(); i++) {
        std::cout << (*seq1)[i] << " ";
    }
    std::cout << std::endl;


    return 0;
}