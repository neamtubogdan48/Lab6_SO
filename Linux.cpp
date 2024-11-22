#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>

bool is_prime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

void calculate_primes(int pipe_write, int start, int end) {
    std::vector<int> primes;
    for (int i = start; i <= end; ++i) {
        if (is_prime(i)) primes.push_back(i);
    }
    write(pipe_write, primes.data(), primes.size() * sizeof(int));
    close(pipe_write);
}

int main() {
    const int num_processes = 10;
    const int range_size = 1000;
    int start = 1;

    int pipes[num_processes][2];

    for (int i = 0; i < num_processes; ++i) {
        pipe(pipes[i]);
        pid_t pid = fork();

        if (pid == 0) {
            close(pipes[i][0]);
            calculate_primes(pipes[i][1], start, start + range_size - 1);
            return 0;
        } else {
            close(pipes[i][1]);
            start += range_size;
        }
    }

    for (int i = 0; i < num_processes; ++i) {
        int primes[range_size];
        read(pipes[i][0], primes, sizeof(primes));
        close(pipes[i][0]);

        std::cout << "Primes from process " << i + 1 << ": ";
        for (int j = 0; j < range_size; ++j) {
            if (primes[j] != 0) std::cout << primes[j] << " ";
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < num_processes; ++i) {
        wait(NULL);
    }

    return 0;
}
