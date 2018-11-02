#include <fstream>
#include <iostream>
#include <queue>
#include <ctime>
#include "../src/huffman.h"
using std::string;
extern void read(const string input, huffman &algo, std::ifstream &ifs);
extern void compress(const string input, const string output, huffman &algo, std::ifstream &ifs, std::ofstream &ofs);
extern void decompress(const string input, const string output, huffman &algo, std::ifstream &ifs, std::ofstream &ofs);

string input = "pack SRC DEST";

void incorrect_input() {
    std::cout << "Incorrect input\n";
    std::cout << "Usage:\n" << input;
}

void test() {

    std::ofstream out("bigdata.txt", std::ios::binary);
    char buf[1024];
    int time = clock();
    for (int i = 0; i < 100; i++)
        for (int j = 0; j < 1024; j++) {

            for (int k = 0; k < 1024; k++) {
                buf[k] = rand() % 255;
            }
            out.write(buf, 1024);
        }
    out.write("hahahah", 7);
    std::cout << "time: " << (double)(clock() - time) / CLOCKS_PER_SEC << "ms\n";

    //in.close();
    out.close();

}

int main(int argc, char *argv[]) {
    //test();
    //return 0;
    if (argc < 3 || argc > 4) {
        incorrect_input();
        return 0;
    }
    std::ifstream ifs;
    std::ofstream ofs;
    string command = argv[1];
    string src = argv[2];
    string dst;
    huffman h{};

    int timer1;
    int timer2;
    switch (command[0]) {
    case 'p':
        dst = src + ".pack";

        //add custom archive

        std::cout << "compressing " << src << " to " << dst << "...\n";
        timer1 = clock();
        read(src, h, ifs);
        compress(src, dst, h, ifs, ofs);
        timer2 = clock();
        std::cout << "compressed in " << (timer2 - timer1) / (double)CLOCKS_PER_SEC << "ms\n";
        break;
    case 'u':
        if (src.substr(src.find_last_of('.')) != ".pack") {
            incorrect_input();
        }
        dst = src.substr(0, src.find_last_of('.')) + ".unpack";
        std::cout << "decompress " << src << " to " << dst << "...\n";
        timer1 = clock();
        decompress(src, dst, h, ifs, ofs);
        timer2 = clock();
        std::cout << "decompressed in " << (timer2 - timer1) / (double)CLOCKS_PER_SEC << "ms\n";
        break;
    default:
        incorrect_input();
        return 0;
    }
}
