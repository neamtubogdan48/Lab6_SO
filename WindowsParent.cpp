#include <windows.h>
#include <iostream>
#include <vector>
#include <sstream>

bool isPrime(int num) {
    if (num < 2) return false;
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) return false;
    }
    return true;
}

void findPrimesInRange(int start, int end, std::string& primes) {
    std::ostringstream oss;
    for (int i = start; i < end; ++i) {
        if (isPrime(i)) {
            oss << i << " ";
        }
    }
    primes = oss.str();
}

void childProcess(HANDLE readPipe, HANDLE writePipe) {
    SetStdHandle(STD_INPUT_HANDLE, readPipe);
    SetStdHandle(STD_OUTPUT_HANDLE, writePipe);

    CloseHandle(readPipe);
    CloseHandle(writePipe);

    int start, end;
    std::cin >> start >> end;

    std::string primes;
    findPrimesInRange(start, end, primes);

    std::cout << primes;
    exit(0);
}

int main() {
    const int RANGE = 10000;
    const int NUM_PROCESSES = 10;
    const int INTERVAL = RANGE / NUM_PROCESSES;

    HANDLE readPipes[NUM_PROCESSES];
    HANDLE writePipes[NUM_PROCESSES];
    PROCESS_INFORMATION procInfo[NUM_PROCESSES];
    STARTUPINFOA startInfo[NUM_PROCESSES];

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        HANDLE readPipeParent, writePipeChild;
        HANDLE readPipeChild, writePipeParent;

        CreatePipe(&readPipeParent, &writePipeChild, &sa, 0);
        CreatePipe(&readPipeChild, &writePipeParent, &sa, 0);

        readPipes[i] = readPipeParent;
        writePipes[i] = writePipeParent;

        ZeroMemory(&startInfo[i], sizeof(startInfo[i]));
        startInfo[i].cb = sizeof(startInfo[i]);
        startInfo[i].hStdInput = readPipeChild;
        startInfo[i].hStdOutput = writePipeChild;
        startInfo[i].dwFlags = STARTF_USESTDHANDLES;

        std::string command = "ChildProcessExecutable.exe";
        std::vector<char> commandBuffer(command.begin(), command.end());
        commandBuffer.push_back('\0'); 

        if (!CreateProcessA(
            NULL, commandBuffer.data(), NULL, NULL, TRUE, 0, NULL, NULL,
            &startInfo[i], &procInfo[i]
        )) {
            DWORD error = GetLastError();
            std::cerr << "Failed to create process " << i << ". Error code: " << error << std::endl;
            return 1;
        }

        CloseHandle(readPipeChild);
        CloseHandle(writePipeChild);

        int start = i * INTERVAL;
        int end = (i + 1) * INTERVAL;

        std::ostringstream rangeStream;
        rangeStream << start << " " << end << "\n";
        std::string rangeStr = rangeStream.str();
        DWORD written;
        WriteFile(writePipes[i], rangeStr.c_str(), rangeStr.size(), &written, NULL);
        CloseHandle(writePipes[i]);
    }

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        char buffer[1024];
        DWORD bytesRead;
        std::string primes;

        while (ReadFile(readPipes[i], buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            primes += buffer;
        }

        CloseHandle(readPipes[i]);
        std::cout << "Primes from process " << i << ": " << primes << std::endl;
    }

    for (int i = 0; i < NUM_PROCESSES; ++i) {
        WaitForSingleObject(procInfo[i].hProcess, INFINITE);
        CloseHandle(procInfo[i].hProcess);
        CloseHandle(procInfo[i].hThread);
    }

    return 0;
}