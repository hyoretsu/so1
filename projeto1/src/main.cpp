#include <fstream>
#include <iostream>
#include <vector>

#include "./classes/Scheduler.cpp"
#include "./utils/split.cpp"

// {
//     [Arrival time]: duration[]
// }
std::vector<std::vector<Process>> processes;

struct SchedulerParams {
    std::vector<std::string> algorithms;
    std::vector<int> params;
};

struct Schedulers {
    Scheduler fcfs;
    Scheduler sjf;
    Scheduler rr;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Por favor, informe o caminho do arquivo de entrada." << std::endl;

        return 1;
    }

    // Queue fcfsQueue = Queue("fcfs"), sjfQueue = Queue("sjf"), rrQueue = Queue("rr", 2);
    Schedulers schedulers;
    schedulers.fcfs = Scheduler({ "fcfs" });
    schedulers.sjf = Scheduler({ "sjf" });
    schedulers.rr = Scheduler({ "rr" }, { 2 });

    std::ifstream input(argv[1]);
    std::string line;

    while (std::getline(input, line)) {
        std::vector<string> info = split(line, " ");

        int arrivalTime = stoi(info[0]);
        int duration = stoi(info[1]);

        if ((int)processes.size() < arrivalTime + 1) {
            processes.resize(arrivalTime + 1);
        }

        processes[arrivalTime].push_back(Process(duration));
    }

    int currentTime = 0;
    // While the schedulers haven't finished running or new processes will run in the future
    while (
        !schedulers.fcfs.finished() ||
        !schedulers.sjf.finished() ||
        !schedulers.rr.finished() ||
        currentTime < (int)processes.size()
        ) {
        // std::cout << currentTime << std::endl;

        // If there are still processes waiting to get queued, and there are some scheduled to arrive at this moment
        if ((int)processes.size() > currentTime && processes[currentTime].size() > 0) {
            // Get every process that arrived at the current time
            for (Process process : processes[currentTime]) {
                // And add it to the schedulers
                schedulers.fcfs.insert(process, 0);
                schedulers.sjf.insert(process, 0);
                schedulers.rr.insert(process, 0);

                // std::cout << process.getPid() << " chegada: " << currentTime << std::endl;
            }
        }

        schedulers.fcfs.tick();
        schedulers.sjf.tick();
        schedulers.rr.tick();

        currentTime += 1;
    }

    std::cout << "FCFS ";
    schedulers.fcfs.printStatistics();
    std::cout << "SJF ";
    schedulers.sjf.printStatistics();
    std::cout << "RR ";
    schedulers.rr.printStatistics();

    return 0;
}
