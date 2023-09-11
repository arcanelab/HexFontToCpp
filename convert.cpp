#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>

const int MAX_BYTES = 65536;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename> [int1] [int2]" << std::endl;
        return 1;
    }

    int int1 = 0, int2 = 35;
    if (argc >= 4) {
        int1 = std::stoi(argv[2]);
        int2 = std::stoi(argv[3]);
    }

    std::ifstream inFile(argv[1]);
    if (!inFile) {
        std::cerr << "Error: Cannot open input file." << std::endl;
        return 1;
    }

    std::string line;
    std::vector<int> output;
    while (std::getline(inFile, line) && output.size() < MAX_BYTES) {
        std::string hexData = line.substr(line.find(":") + 1);
        for (size_t i = 0; i < hexData.size() && output.size() < MAX_BYTES; i += 2) {
            std::string byteStr = hexData.substr(i, 2);
            int byteVal = std::stoi(byteStr, nullptr, 16);
            std::bitset<8> bits(byteVal);
            for (int j = 7; j >= 0 && output.size() < MAX_BYTES; --j) {
                output.push_back(bits[j] ? int2 : int1);
            }
        }
    }

    std::string baseFilename = argv[1];
    size_t pos = baseFilename.find_last_of(".");
    if (pos != std::string::npos) {
        baseFilename = baseFilename.substr(0, pos);
    }
    std::string validName = baseFilename;
    for (char& c : validName) {
        if (!isalnum(c)) {
            c = '_';
        }
    }

    std::ofstream cppFile(baseFilename + ".cpp");
    std::ofstream hFile(baseFilename + ".h");

    hFile << "#ifndef " << validName << "_H" << std::endl;
    hFile << "#define " << validName << "_H" << std::endl;
    hFile << "extern const int " << validName << "[];" << std::endl;
    hFile << "extern const int " << validName << "_length;" << std::endl;
    hFile << "#endif" << std::endl;

    cppFile << "#include \"" << baseFilename << ".h\"" << std::endl;
    cppFile << "const int " << validName << "[] = {";
    for (size_t i = 0; i < output.size(); ++i) {
        cppFile << output[i];
        if (i != output.size() - 1) {
            cppFile << ",";
        }
    }
    cppFile << "};" << std::endl;
    cppFile << "const int " << validName << "_length = " << output.size() << ";" << std::endl;

    std::cout << "Generated files: " << baseFilename << ".cpp and " << baseFilename << ".h" << std::endl;

    return 0;
}
