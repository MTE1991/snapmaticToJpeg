#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>

// Alias for ease of use
namespace fs = std::filesystem;

// Helper function to convert hex string to bytes
std::vector<unsigned char> hexStringToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(strtol(byteString.c_str(), nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

// Function to process and convert a Snapmatic file to JPEG
void convertToJpeg(const fs::path& filePath, const std::vector<unsigned char>& marker) {
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Failed to open file: " << filePath.filename() << std::endl;
        return;
    }

    // Read the entire file into memory
    std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    // Search for the marker in the file data
    auto markerPosition = std::search(fileData.begin(), fileData.end(), marker.begin(), marker.end());
    if (markerPosition == fileData.end()) {
        std::cout << filePath.filename() << " is not a valid Snapmatic file. Skipping..." << std::endl;
        return;
    }

    // Create the output file with "_img" suffix
    fs::path outputFilePath = filePath.stem().string() + "_img.jpeg";
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Failed to create output file: " << outputFilePath << std::endl;
        return;
    }

    // Write the trimmed data (starting from the marker) to the output file
    outputFile.write(reinterpret_cast<const char*>(&(*markerPosition)), fileData.end() - markerPosition);
    outputFile.close();

    std::cout << filePath.filename() << " has been successfully converted into jpeg." << std::endl;
}

// Function to process all files starting with "PGTA" in the given directory
void processPGTAFiles(const fs::path& directory, const std::vector<unsigned char>& marker) {
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            const fs::path& filePath = entry.path();
            if (filePath.filename().string().find("PGTA") == 0) {
                // Process the file
                convertToJpeg(filePath, marker);
            }
        }
    }
}

int main() {
    // Directory to process (current working directory)
    fs::path directory = fs::current_path();

    // Marker to look for (JPEG header)
    std::string markerHex = "FFD8FFE000104A4649460001";
    std::vector<unsigned char> marker = hexStringToBytes(markerHex);

    // Process all files with "PGTA" prefix in the directory
    processPGTAFiles(directory, marker);

    return 0;
}
