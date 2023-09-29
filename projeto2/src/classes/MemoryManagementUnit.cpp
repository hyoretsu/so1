#include <algorithm>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "./RandomAccessMemory.cpp"

std::vector<std::string> supportedAlgorithms = { "fifo", "otm", "lru" };
const char* unsupportedAlgorithmMessage = "Only FIFO, Ótimo and Least Recently Used MMU's are supported.";

struct Page {
    int index;
    int frame;
    bool valid;
    bool dirty;
    int accessed;
};

class MemoryManagementUnit {
private:
    std::string algorithm;
    int frameSize;
    int history;
    int _pageFaults;
    std::vector<Page> pages;
    std::vector<int> queue;
    RandomAccessMemory ram;

    // Returns frame of the given page
    int addPage(const int& pageNumber, const std::string& content = "anything") {
        int frame = this->history;

        // If the page table's full, remove a page according to the MMU's algorithm and catch the frame that it was using
        if (this->pages.size() >= this->ram.maxFrames()) {
            frame = this->removePage();
        }

        // Update the RAM with the given content
        this->ram.setFrame(this->getPhysicalAddress(frame), content);

        // And add the new page to the table
        Page newPage = { pageNumber, frame, true, false, history };
        this->pages.push_back(newPage);

        return frame;
    }

    long getPhysicalAddress(const int& pageNumber) {
        return pageNumber * this->frameSize;
    }

    int removePage() {
        int frame;

        if (this->algorithm == "fifo") {
            frame = this->pages[0].frame;

            this->pages.erase(this->pages.begin());
        } else if (this->algorithm == "otm") {
            int pagesN = this->pages.size();

            // Get the number of all pages currently in the table
            std::vector<int> pages;
            for (Page page : this->pages) {
                pages.push_back(page.index);
            }

            std::vector<int> pagesUsed(pagesN, 0);

            int queueSize = this->queue.size();
            // Find when each page is last used in the queue
            for (int i = 0; i < pagesN; i++) {
                int page = pages[i];

                for (int j = this->history; j < queueSize; j++) {
                    // If the current page in the queue is the one we're looking for and we found a later use of it
                    if (this->queue[j] == page && pagesUsed[i] < j) {
                        pagesUsed[i] = j;
                    }
                }
            }

            // Find the last used page's index only if the first one isn't useless already
            int lastUsedPageIndex = 0;
            if (pagesUsed[lastUsedPageIndex] != 0) {
                for (int i = 1; i < pagesN; i++) {
                    if (pagesUsed[i] > pagesUsed[lastUsedPageIndex]) {
                        lastUsedPageIndex = i;

                        // And quit as soon as there's an useless one
                        if (pagesUsed[i] == 0) {
                            break;
                        }
                    }
                }
            }

            // Retrieve its frame
            frame = this->pages[lastUsedPageIndex].frame;

            // And erase it from the table
            this->pages.erase(this->pages.begin() + lastUsedPageIndex);
        } else {
            int lruPage[2] = { 0, std::numeric_limits<int>::max() };

            for (int i = 0; i < (int)this->pages.size(); i++) {
                Page page = this->pages[i];

                if (page.accessed < lruPage[1]) {
                    lruPage[0] = i;
                    lruPage[1] = page.accessed;
                }
            }

            frame = this->pages[lruPage[0]].frame;

            this->pages.erase(this->pages.begin() + lruPage[0]);
        }

        this->ram.cleanFrame(frame);

        return frame;
    }

public:
    // Constructors
    MemoryManagementUnit(const int& framesN, const std::string& algorithm, const std::vector<int>& queue, const int& frameSize = 1) : algorithm(algorithm), frameSize(frameSize), history(0), _pageFaults(0), queue(queue), ram(framesN* frameSize) {
        if (!std::any_of(supportedAlgorithms.begin(), supportedAlgorithms.end(), [this](std::string supportedAlgorithm) {return this->algorithm == supportedAlgorithm;})) {
            throw std::invalid_argument(unsupportedAlgorithmMessage);
        }
    }

    std::string getPage(const int& pageNumber) {
        int frame;
        bool storedFrame = false;

        // Check if the page's already present
        for (Page& page : this->pages) {
            if (page.index == pageNumber) {
                page.accessed = this->history;
                frame = page.frame;

                storedFrame = true;
                break;
            }
        }

        // If it isn't, add to the number of page faults and then add the page
        if (!storedFrame) {
            _pageFaults += 1;

            frame = this->addPage(pageNumber);
        }

        this->history += 1;

        return this->ram.getFrame(this->getPhysicalAddress(frame));
    }

    int pageFaults() {
        return this->_pageFaults;
    }
};
