#include <algorithm>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

#include "./Process.cpp"

std::vector<std::string> supportedAlgorithms = { "fcfs", "rr", "sjf" };
const char* unsupportedAlgorithmMessage = "Only FCFS, SJF and RR are supported.";

class Queue {
private:
    std::string algorithm = "";
    bool preemptive = false;
    std::vector<Process> processes;
    int quantum = 0;
    bool running = false;
    int timeSinceSwitch = 0;

    void isAlgorithmSupported() {
        if (std::any_of(supportedAlgorithms.begin(), supportedAlgorithms.end(), [this](std::string supportedAlgorithm) {return this->algorithm == supportedAlgorithm;})) {
            return;
        }

        throw std::invalid_argument(unsupportedAlgorithmMessage);
    }

    void removeProcess() {
        this->processes.erase(this->processes.begin());
    }

    void sort(const int& startIndex = 0, const std::string order = "asc") {
        if (order != "asc" && order != "desc") {
            throw std::invalid_argument("Sorting must be done ascendingly or descendingly.");
        }

        // Sort in ascending order
        std::sort(
            processes.begin() + startIndex,
            processes.end(),
            [order](const Process& a, const Process& b) {
                return order == "asc" ? a.timeLeft() < b.timeLeft() : a.timeLeft() > b.timeLeft();
            }
        );
    }

public:
    // Constructors
    Queue() {}
    explicit Queue(const std::string& algorithm) : algorithm(algorithm) {
        this->isAlgorithmSupported();

        if (algorithm == "rr") {
            throw std::invalid_argument("Round-robin queues need a quantum parameter. Please provide one.");
        }

    }
    explicit Queue(const std::string& algorithm, const bool& preemptive) : algorithm(algorithm), preemptive(preemptive) {
        if (algorithm != "sjf") {
            throw std::invalid_argument("Only SJF queues may be preemptive.");
        }
    }
    Queue(const std::string& algorithm, const int quantum) : algorithm(algorithm), quantum(quantum) {
        if (algorithm != "rr") {
            throw std::invalid_argument("Only round-robin queues need a quantum parameter.");
        }
    }

    void add(const Process& process) {
        this->processes.push_back(process);

        if (this->algorithm == "sjf") {
            sort(!preemptive && running ? 1 : 0);
        }
    }

    std::string getCurrentProcess() {
        return this->processes.front().getPid();
    }

    std::vector<Process> getProcesses() {
        return this->processes;
    }

    int remainingProcessCount() {
        return this->processes.size();
    }

    bool tick() {
        if (!running) {
            this->running = true;
        }
        this->processes.front().reduceTime();

        // If the process is done
        if (this->processes.front().timeLeft() == 0) {
            // Remove it from the queue
            this->removeProcess();

            if (this->algorithm == "sjf") {
                this->running = false;
            } else if (this->algorithm == "rr") {
                // Reset time since switch, otherwise the next process won't use all its quantum
                this->timeSinceSwitch = 0;
            }

            return true;
        }

        if (this->algorithm == "rr") {
            this->timeSinceSwitch += 1;

            // If the process has spent all its available time (quantum)
            if (this->timeSinceSwitch >= this->quantum) {
                // Push it to the end of the queue
                this->processes.push_back(this->processes.front());
                // Stop its execution
                this->removeProcess();
                // And reset time since switch
                this->timeSinceSwitch = 0;
            }
        }

        return false;
    }
};
