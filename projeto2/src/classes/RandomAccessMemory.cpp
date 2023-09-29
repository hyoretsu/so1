#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

class RandomAccessMemory {
private:
    std::vector<std::string> frames;

public:
    // Constructors
    RandomAccessMemory(const long& framesN) : frames(framesN) {}

    void cleanFrame(const long& frame) {
        this->frames[frame] = "";
    }

    std::string getFrame(const int& frame) {
        return this->frames[frame];
    }

    std::size_t maxFrames() const {
        return frames.size();
    }

    /* Returns true in case there was content previously set to the frame */
    std::tuple<bool, std::string> setFrame(const long& frame, const std::string& content) {
        if (frame > (int)this->frames.size() - 1) {
            throw std::invalid_argument("The given frame is bigger than the RAM size.");
        }

        std::string oldContent = this->frames[frame];
        this->frames[frame] = content;

        if (oldContent != "") {
            return { true, oldContent };
        }

        return { false, content };
    }
};
