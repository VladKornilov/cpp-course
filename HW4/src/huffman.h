#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <vector>
#include <map>

const int volume = 256;
class huffman
{
public:
    huffman();
    void add_data(char *data);
    std::vector <char> encode(std::string data);
    void decode(std::vector <char> &data, std::vector <char> &code);


private:
    int symbols[volume];
    bool alpha_is_encoded;
    bool alpha_is_decoded;

    char pack;
    int pack_pos;

    struct Node {
        Node* left;
        Node* right;
        int size;
        char symbol;
        bool operator <(const Node &other) const {
            return this->size > other.size;
        }
    };
    Node* tree;
    Node* last_decoding_node;

    std::map <char, std::string> symbol_map;
    std::vector <char> symbol_list;
    std::vector <char> symbol_len;
    std::vector <char> encoded_data;

    void create_bit_alpha();
    void set_symbol_map(Node* node, std::string code);
    void encode_tree();
    std::vector <char> encode_alpha();
    void decode_tree(Node* node, int len, int &it);
    int decode_alpha(std::vector <char> &data);

};

#endif // HUFFMAN_H
