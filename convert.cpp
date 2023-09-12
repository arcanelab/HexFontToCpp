#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint> // for uint8_t

std::string sanitizeForIdentifier(const std::string &str)
{
    std::string result;
    for (char c : str)
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (result.size() > 0 && c >= '0' && c <= '9'))
            result += c;
        else
            result += "_";
    }
    if (result.empty() || (result[0] >= '0' && result[0] <= '9'))
        result = "_" + result; // Identifiers cannot start with a number

    return result;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <filename> [max_bytes]" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];
    std::string baseFilename = inputFilename.substr(0, inputFilename.find_last_of("."));
    int maxBytes = INT_MAX;
    if (argc > 2)
        maxBytes = std::stoi(argv[2]);

    std::ifstream inputFile(inputFilename);
    if (!inputFile.is_open())
    {
        std::cerr << "Failed to open input file: " << inputFilename << std::endl;
        return 1;
    }

    std::vector<uint8_t> dataArray;

    std::string line;
    int lineCount = 0;
    while (std::getline(inputFile, line) && dataArray.size() < maxBytes)
    {
        lineCount++;
        size_t pos = line.find(":");
        if (pos != std::string::npos)
        {
            std::string hexData = line.substr(pos + 1);
            for (size_t i = 0; i < hexData.length() && dataArray.size() < maxBytes; i += 2)
            {
                uint8_t byte = std::stoul(hexData.substr(i, 2), nullptr, 16);
                dataArray.push_back(byte);
            }
        }
    }

    inputFile.close();

    std::string cppFilename = baseFilename + ".cpp";
    std::string hFilename = baseFilename + ".h";
    std::string arrayName = sanitizeForIdentifier(baseFilename);

    std::ofstream cppFile(cppFilename);
    std::ofstream hFile(hFilename);

    hFile << "#pragma once\n";
    hFile << "#include <cstdint>\n\n";
    hFile << "extern const uint8_t " << arrayName << "[];\n";
    hFile << "extern const int dataSize;\n";

    cppFile << "#include \"" << baseFilename << ".h\"\n\n";
    cppFile << "const uint8_t " << arrayName << "[] = {";
    for (size_t i = 0; i < dataArray.size(); ++i)
    {
        if (i != 0)
            cppFile << ",";
        cppFile << static_cast<int>(dataArray[i]);
    }
    cppFile << "};\n";
    cppFile << "const int dataSize = " << dataArray.size() << ";\n";

    cppFile.close();
    hFile.close();

    std::cout << "Written to files: " << cppFilename << " and " << hFilename << std::endl;
    std::cout << "Number of lines processed: " << lineCount << std::endl;
    std::cout << "Number of bytes in array: " << dataArray.size() << std::endl;

    return 0;
}
