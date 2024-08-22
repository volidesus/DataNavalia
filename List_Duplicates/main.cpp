#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

namespace fs = std::filesystem;

const std::string RED = "\033[38;2;241;107;22m";
const std::string BLUE = "\033[38;2;13;152;185m";
const std::string DARKBLUE = "\033[38;2;115;194;251m";
const std::string RESET = "\033[0m";

struct Item {
    std::string name;
    std::string value;
};

std::vector<std::vector<Item>> findDuplicatesGrouped(const std::vector<Item>& items) {
    std::unordered_map<std::string, std::vector<Item>> valueToItems;

    for (const auto& item : items) {
        valueToItems[item.value].push_back(item);
    }

    std::vector<std::vector<Item>> result;
    for (const auto& [value, itemList] : valueToItems) {
        if (itemList.size() > 1) {
            result.push_back(itemList);
        }
    }

    return result;
}

std::vector<Item> items;

void iterateFolder(const fs::path& path) {
    try {
        std::unordered_map<uintmax_t, std::vector<fs::path>> sizeToPaths;

        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_regular_file(entry.path())) {
                auto fileSize = fs::file_size(entry.path());
                sizeToPaths[fileSize].push_back(entry.path());
            } else if (fs::is_directory(entry)) {
                iterateFolder(entry.path());
            }
        }

        for (const auto& [size, paths] : sizeToPaths) {
            if (paths.size() > 1) {
                std::unordered_map<std::string, std::vector<fs::path>> valueToPaths;
                
                for (const auto& path : paths) {
                    std::ifstream inputFile(path.string());
                    std::string value;
                    if (inputFile) {
                        std::string line;
                        while (std::getline(inputFile, line)) {
                            value += line + '\n';
                        }
                        valueToPaths[value].push_back(path);
                    } else {
                        std::cerr << RED << "Failed to open file: " << path << RESET << std::endl;
                    }
                }
                
                for (const auto& [value, paths] : valueToPaths) {
                    if (paths.size() > 1) {
                        for (const auto& path : paths) {
                            items.push_back({path.filename().string(), value});
                        }
                    }
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << RED << "Filesystem error: " << e.what() << RESET << std::endl;
    }
}

int main() {
    try {
        iterateFolder(fs::current_path());
        auto groupedDuplicates = findDuplicatesGrouped(items);

        bool useBlue = true;
        for (const auto& group : groupedDuplicates) {
            for (const auto& item : group) {
                std::cout << (useBlue ? BLUE : DARKBLUE) << item.name << " " << RESET;
            }
            std::cout << std::endl;
            useBlue = !useBlue;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << RED << "Failed to access the directory: " << e.what() << RESET << std::endl;
    }
    return 0;
}