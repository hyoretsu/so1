#include <string>

#include "../utils/uuid.cpp"

class Process {
private:
    std::string _pid = uuid_v4();
    int _timeLeft;

public:
    explicit Process(const int& peakTime) : _timeLeft(peakTime) {}

    const std::string& getPid() const {
        return this->_pid;
    }

    const int& timeLeft() const {
        return this->_timeLeft;
    }

    void reduceTime(const int& time = 1) {
        this->_timeLeft -= time;
    }
};
