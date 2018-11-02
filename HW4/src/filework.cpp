#include <fstream>
#include <cstdio>
#include <iostream>
#include "huffman.h"
using std::string;
const int bsize = 1 << 10;
char buf[bsize];

void read(const string input, huffman &algo, std::ifstream &ifs) {
    ifs.open(input);
    ifs.seekg(0, std::ios::end);
    long long size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    if (size == -1) {
        throw std::runtime_error("No such file, specify input");
    }
    try {
        int counter = 0;
        while (size > bsize) {
            ifs.read(buf, bsize);
            algo.add_data(buf);
            size -= bsize;
            std::cout << counter++ << " ";
        }
        std::cout << "\n";
        ifs.read(buf, size);
        buf[size] = 1;
        algo.add_data(buf);

        ifs.close();
    }
    catch(...) {
        ifs.close();
    }
    int a = 5;
}

void compress(const string input, const string output, huffman &algo, std::ifstream &ifs, std::ofstream &ofs) {
    ifs.open(input, std::ios::binary);
    ofs.open(output, std::ios::binary);

    ifs.seekg(0, std::ios::end);
    long long size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector <char> data;
    int counter = 0;
    while (size > bsize) {
        ifs.read(buf, bsize);
        data = algo.encode(std::string(buf, bsize));
        char array_data[data.size()];
        std::copy(data.begin(), data.end(), array_data);
        ofs.write(array_data, sizeof(array_data));
        size -= bsize;

        std::cout << counter++ << " ";
    }
    std::cout << "\n";
    ifs.read(buf, size);
    data = algo.encode(std::string(buf, size));
    char array_data[data.size()];
    std::copy(data.begin(), data.end(), array_data);
    ofs.write(array_data, sizeof(array_data));
    ifs.close();
    ofs.close();
}

void decompress(const string input, const string output, huffman &algo, std::ifstream &ifs, std::ofstream &ofs) {
    ifs.open(input, std::ios::binary);
    ofs.open(output, std::ios::binary);

    ifs.seekg(0, std::ios::end);
    long long size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);


    while (size > bsize) {
        ifs.read(buf, bsize);
        std::vector <char> code(1024);
        std::copy(buf, buf + 1024, code.begin());
        std::vector <char> data;
        algo.decode(code, data);

        char array_data[data.size()];
        std::copy(data.begin(), data.end(), array_data);
        ofs.write(array_data, sizeof(array_data));
        size -= bsize;
    }
    ifs.read(buf, size);
    std::vector <char> code(size);
    std::copy(buf, buf + size, code.begin());
    std::vector <char> data;
    algo.decode(code, data);

    char array_data[data.size()];
    std::copy(data.begin(), data.end(), array_data);
    ofs.write(array_data, sizeof(array_data) - 1);

    ifs.close();
    ofs.close();
}
