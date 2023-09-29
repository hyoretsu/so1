#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "./classes/MemoryManagementUnit.cpp"

struct SchedulerParams {
    std::vector<std::string> algorithms;
    std::vector<int> params;
};

struct MMUs {
    MemoryManagementUnit fifo;
    MemoryManagementUnit otm;
    MemoryManagementUnit lru;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Por favor, informe o caminho do arquivo de entrada." << std::endl;

        return 1;
    }

    int framesN = 0;
    std::vector<int> queue;

    std::ifstream input(argv[1]);
    std::string line;
    while (std::getline(input, line)) {
        if (framesN == 0) {
            framesN = stoi(line);
        } else {
            queue.push_back(stoi(line));
        }
    }

    MMUs mmus = { MemoryManagementUnit(framesN, "fifo", queue), MemoryManagementUnit(framesN, "otm", queue), MemoryManagementUnit(framesN, "lru", queue) };

    for (int page : queue) {
        mmus.fifo.getPage(page);
        mmus.otm.getPage(page);
        mmus.lru.getPage(page);
    }

    std::cout << "FIFO " << mmus.fifo.pageFaults() << std::endl;
    std::cout << "OTM " << mmus.otm.pageFaults() << std::endl;
    std::cout << "LRU " << mmus.lru.pageFaults() << std::endl;

    return 0;
}
