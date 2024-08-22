#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_set>

namespace fs = std::filesystem;

const std::string RESET = "\033[0m";
const std::string RED = "\033[38;2;241;107;22m";
const std::string YELLOW = "\033[38;2;246;255;146m";
const std::string BLUE = "\033[38;2;13;152;185m";

std::unordered_set<std::string> visitedPaths;

void iterateFolder(const std::string& path, const std::string& prefix = "") {
    if (visitedPaths.find(path) != visitedPaths.end()) {
        return;
    }

    visitedPaths.insert(path);

    try {
        for (auto it = fs::directory_iterator(path); it != fs::directory_iterator(); ++it) {
            const auto& entry = *it;
            bool isLast = (std::next(it) == fs::directory_iterator());

            std::cout << prefix << "|__";

            try {
                std::cout << BLUE << entry.path().filename().string();

                if (fs::is_regular_file(entry.path())) {
                    auto fileSize = fs::file_size(entry.path()) / 1024.0;
                    std::cout << YELLOW << " (" << fileSize << " KB)";
                }
                std::cout << RESET << std::endl;

                if (fs::is_directory(entry)) {
                    iterateFolder(entry.path().string(), prefix + (isLast ? "   " : "|  "));
                }
            } catch (const fs::filesystem_error& e) {
                std::cout << RED << "INVALID PERMISSION" << RESET << std::endl;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cout << prefix << RED << "INVALID PERMISSION (Directory)" << RESET << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::string path = (argc > 1) ? argv[1] : "./";
    try {
        iterateFolder(path);
    } catch (const fs::filesystem_error& e) {
        std::cerr << RED << "Failed to access the directory: " << e.what() << RESET << std::endl;
    }
    return 0;
}