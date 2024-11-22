#include <iostream>
#include <string>
#include <sstream>
#include <vector>


bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}


void findPrimesInRange(int start, int end, std::vector<int>& primes) {
    for (int i = start; i < end; ++i) {
        if (isPrime(i)) {
            primes.push_back(i);
        }
    }
}

int main() {
    
    int start, end;
    std::cin >> start >> end;

    std::vector<int> primes;
    findPrimesInRange(start, end, primes);

    
    for (int prime : primes) {
        std::cout << prime << " ";
    }

    return 0;
}