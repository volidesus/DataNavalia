#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;

#include "modes.h"
#include "aes.h"
#include "filters.h"

std::string generateKeyOrIV(const std::string& password, size_t length) {
    std::string key_or_iv = password;
    while (key_or_iv.size() < length) {
        key_or_iv += password;
    }
    return key_or_iv.substr(0, length);
}

std::string encrypt(const std::string& plaintext, const CryptoPP::byte key[], const CryptoPP::byte iv[]) {
    std::string ciphertext;

    CryptoPP::AES::Encryption aesEncryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, iv);

    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(ciphertext));
    stfEncryptor.Put(reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.length());
    stfEncryptor.MessageEnd();

    return ciphertext;
}

std::string decrypt(const std::string& ciphertext, const CryptoPP::byte key[], const CryptoPP::byte iv[]) {
    std::string decryptedtext;

    CryptoPP::AES::Decryption aesDecryption(key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, iv);

    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedtext));
    stfDecryptor.Put(reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.size());
    stfDecryptor.MessageEnd();

    return decryptedtext;
}

std::string checkArguments(int argc, char* argv[], const std::string& argument) {
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == argument && i + 1 < argc) {
            return argv[i + 1];
        }
    }
    return "";
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void createFileWithContent(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
}

int main(int argc, char* argv[]) {
    std::string password = checkArguments(argc, argv, "--p");
    if (password.empty()) {
        std::cerr << "Error: No password provided. Use --p argument to provide a password." << std::endl;
        return 1;
    }

    std::string keyStr = generateKeyOrIV(password, CryptoPP::AES::DEFAULT_KEYLENGTH);
    std::string ivStr = generateKeyOrIV(password, CryptoPP::AES::BLOCKSIZE);

    CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];
    std::memcpy(key, keyStr.data(), CryptoPP::AES::DEFAULT_KEYLENGTH);
    std::memcpy(iv, ivStr.data(), CryptoPP::AES::BLOCKSIZE);

    std::string outputPath = checkArguments(argc, argv, "--o");
    bool isEncryption = !checkArguments(argc, argv, "--e").empty();
    bool isDecryption = !checkArguments(argc, argv, "--d").empty();

    if (outputPath.empty()) {
        if (isEncryption) {
            outputPath = fs::path(checkArguments(argc, argv, "--e")).filename().string();
            if (fs::path(outputPath).extension().string() != ".enc") {
                outputPath += ".enc";
            }
        } else if (isDecryption) {
            outputPath = fs::path(checkArguments(argc, argv, "--d")).filename().string();
            if (fs::path(outputPath).extension().string() == ".enc") {
                outputPath = fs::path(outputPath).replace_extension().string();
            }
        }
    }

    if (isEncryption) {
        std::string filePath = checkArguments(argc, argv, "--e");
        std::string text = readFile(filePath);
        if (text.empty()) {
            std::cerr << "Error: Could not read file or file is empty: " << filePath << std::endl;
            return 1;
        }
        std::string ciphertext = encrypt(text, key, iv);

        createFileWithContent(outputPath, ciphertext);
    }
    else if (isDecryption) {
        std::string filePath = checkArguments(argc, argv, "--d");
        std::string text = readFile(filePath);
        if (text.empty()) {
            std::cerr << "Error: Could not read file or file is empty: " << filePath << std::endl;
            return 1;
        }
        std::string decryptedtext = decrypt(text, key, iv);

        createFileWithContent(outputPath, decryptedtext);
    }
    else {
        std::cerr << "Error: No valid operation specified. Use --e or --d." << std::endl;
        return 1;
    }

    return 0;
}
