#include <iostream>
#include <filesystem>
#include <string>
#include <unordered_set>

namespace fs = std::filesystem;

const std::string RESET = "\033[0m";
const std::string RED = "\033[38;2;241;107;22m";
const std::string YELLOW = "\033[38;2;246;255;146m";
const std::string BLUE = "\033[38;2;13;152;185m";

std::unordered_set<std::string> visitedPaths;

bool searchString(const std::string& text, const std::string& query) {
    auto toLowerStr = [](const std::string& str) {
        std::string lowerStr(str);
        std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        return lowerStr;
    };
    return toLowerStr(text).find(toLowerStr(query)) != std::string::npos;
}

void searchDir(const std::string& dir, const std::string& query) {
    if (visitedPaths.find(dir) != visitedPaths.end()) {
        return;
    }

    visitedPaths.insert(dir);

    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            try {
                std::string filename = entry.path().string();
                if (searchString(filename, query)) {
                    std::cout << BLUE << filename;

                    if (fs::is_regular_file(entry.path())) {
                        auto fileSize = fs::file_size(entry.path()) / 1024.0;
                        std::cout << YELLOW << " (" << fileSize << " KB)";
                    }
                    std::cout << RESET << std::endl;
                }

                if (fs::is_directory(entry)) {
                    searchDir(entry.path().string(), query);
                }
            } catch (const fs::filesystem_error& e) {
            }
        }
    } catch (const fs::filesystem_error& e) {
    }
}

int main(int argc, char* argv[]) {
    std::string query = (argc > 1) ? argv[1] : "";
    std::string path = (argc > 2) ? argv[2] : "./";

    try {
        searchDir(path, query);
    } catch (const fs::filesystem_error& e) {
    }

    return 0;
}