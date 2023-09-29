#include <cstdlib>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

#include "./Queue.cpp"
#include "../utils/precisionRound.cpp"

struct AverageTimes {
    float responseTime;
    float turnaroundTime;
    float waitingTime;
};

struct ProcessInfo {
    int arrivalTime;
    int responseTime;
    int turnaroundTime;
    int waitingTime;
};

// Currently supports a single queue.
class Scheduler {
private:
    AverageTimes averages = { 0, 0, 0 };
    int _currentTime = 0;
    std::map<std::string, ProcessInfo> processInfo;
    std::vector<Queue> queues;

public:
    // Constructor
    Scheduler() {}
    Scheduler(const std::vector<std::string>& algorithms) : queues(algorithms.size()) {
        int queuesN = algorithms.size();

        // Populate the queues array
        for (int i = 0; i < queuesN; i++) {
            queues[i] = Queue(algorithms[i]);
        }
    }
    Scheduler(const std::vector<std::string>& algorithms, std::vector<int> params) : queues(algorithms.size()) {
        int queuesN = algorithms.size();

        // Populate the queues array
        for (int i = 0; i < queuesN; i++) {
            queues[i] = Queue(algorithms[i], params.front());
            params.erase(params.begin());
        }
    }

    // Copy constructor
    Scheduler(const Scheduler& other) {
        for (Queue queue : other.queues) {
            this->queues.push_back(queue);
        }
    }

    // Copy assignment
    Scheduler& operator=(const Scheduler& other) {
        if (this != &other) {
            this->queues = other.queues;
        }

        return *this;
    }

    int currentTime() {
        return this->_currentTime;
    }

    bool finished() {
        for (Queue queue : queues) {
            if (queue.remainingProcessCount() > 0) {
                return false;
            }
        }

        return true;
    }

    void insert(Process& process, int priority) {
        if (priority < 0 || priority >= (int)queues.size()) {
            throw std::invalid_argument("Given priority is higher than the number of available queues.");
        }

        // Add to requested queue
        this->queues[priority].add(process);

        // std::cout << process.getPid() << std::endl;
        // std::cout << this->_currentTime << std::endl;

        // And initialize its statistics (-1 is a workaround after the separation of main and scheduler)
        ProcessInfo info = { this->_currentTime, -this->_currentTime, -this->_currentTime, this->_currentTime > 0 ? -1 : 0 };
        this->processInfo[process.getPid()] = info;
    }

    void printStatistics() {
        int processesN = this->processInfo.size();

        // Calculate average times
        for (auto info : this->processInfo) {
            // std::cout << info.second.waitingTime << std::endl;
            this->averages.responseTime += (float)info.second.responseTime / processesN;
            this->averages.turnaroundTime += (float)info.second.turnaroundTime / processesN;
            this->averages.waitingTime += (float)info.second.waitingTime / processesN;
        }

        std::cout << precisionRound(this->averages.turnaroundTime, 1, "up") << " " << precisionRound(this->averages.responseTime, 1, "up") << " " << precisionRound(this->averages.waitingTime, 1, "up") << " " << std::endl;
    }

    void tick() {
        int currentQueue = 0;

        // Only run execution logic after the initial second or on the second after a process arrives
        if (this->_currentTime < 1 || queues[currentQueue].remainingProcessCount() == 0 || this->processInfo[queues[currentQueue].getCurrentProcess()].arrivalTime == this->_currentTime) {
            this->_currentTime += 1;

            return;
        }

        std::string currentProcess = queues[currentQueue].getCurrentProcess();
        bool done = queues[currentQueue].tick();

        // std::cout << currentProcess << " tick" << std::endl;

        // For all processes that the scheduler has taken care of
        for (auto procInfo : this->processInfo) {
            std::string pid = procInfo.first;

            // If it already finished, skip it
            if (processInfo[pid].turnaroundTime > 0) {
                continue;
            }

            // std::cout << pid << ", atual: " << currentProcess << std::endl;

            // If this is the running process
            if (pid == currentProcess) {
                // And it just finished
                if (done) {
                    // Calculate turnaround time
                    processInfo[pid].turnaroundTime += this->_currentTime;
                    // std::cout << pid << " finish: " << this->_currentTime << std::endl;
                    // std::cout << pid << " turn: " << processInfo[pid].turnaroundTime << std::endl;
                    continue;
                }

                // Otherwise, if it's its first time running, calculate the response time
                if (processInfo[pid].responseTime <= 0) {
                    processInfo[pid].responseTime += this->_currentTime;
                    // std::cout << pid << " resposta: " << processInfo[pid].responseTime << " chegada: " << processInfo[pid].arrivalTime << std::endl;
                }
            } else {
                // If it isn't running and didn't finish, that means it's waiting
                processInfo[pid].waitingTime += 1;
                // std::cout << pid << " wait: " << processInfo[pid].waitingTime << std::endl;
            }
        }

        this->_currentTime += 1;
    }
};
