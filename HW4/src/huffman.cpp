#include "huffman.h"
#include <string.h>
#include <queue>
#include <vector>
#include <string>
#include <map>
#include <iostream>

huffman::huffman() {
    memset(symbols, 0, sizeof(symbols));
    alpha_is_encoded = false;
    alpha_is_decoded = false;
    pack = 0;
    pack_pos = 0;
}
void huffman::add_data(char *data) {
    while (*data) {
        ++symbols[*data];
        ++data;
    }
}

void huffman::encode_tree() {
    std::priority_queue<Node, std::vector<Node> > q;
    for (int i = 0; i < volume; i++) {
        if (symbols[i] > 0) {
            Node node = {nullptr, nullptr, symbols[i], (char)i};
            q.push(node);
        }
    }
    while (q.size() > 1) {
        Node *a = new Node(q.top()); q.pop();
        Node *b = new Node(q.top()); q.pop();
        Node node = {a, b, a->size + b->size, 0};
        q.push(node);
    }
    if (q.empty()) return;
    tree = new Node(q.top());
    q.pop();
    std::string code;
    set_symbol_map(tree, code);
}

void huffman::set_symbol_map(Node* node, std::string code) {
    if (node->symbol != 0) {
        symbol_map[node->symbol] = code;
        symbol_list.push_back(node->symbol);
        return;
    }
    code += "0";
    set_symbol_map(node->left, code);
    code.back() = '1';
    set_symbol_map(node->right, code);
    return;
}



std::vector <char> huffman::encode_alpha() {
    encode_tree();
    std::vector <char> code;

    code.push_back((char)symbol_list.size());

    code.insert(code.end(), symbol_list.begin(), symbol_list.end());

    bool flag = false;
    for (char i : symbol_list) {
        int len = symbol_map[i].size();
        if (flag == false) {
            code.push_back(len << 4);
            flag = true;
        }
        else {
            code.back() += len;
            flag = false;
        }
    }
    return code;
}

std::vector <char> huffman::encode(std::string data) {
    std::vector <char> code;
    if (!alpha_is_encoded) {
        code = encode_alpha();
        alpha_is_encoded = true;
    }
    //return code;

    int pos = pack_pos;
    for (char i : data) {
        std::string i_code = symbol_map[i];
        for (char j: i_code) {
            if (pos == 8) {
                code.push_back(pack);
                pack = 0;
                pos = 0;
            }

            pack |= (j == '1') << (7 - pos);
            pos++;
        }

    }
    pack_pos = pos;
    return code;
}

void huffman::decode_tree(Node* node, int len, int &it) {
    if (len == symbol_len[it]) {
        node->symbol = symbol_list[it];
        it++;
        return;
    }
    node->symbol = 0;
    node->left = new Node();
    node->right = new Node();
    decode_tree(node->left, len + 1, it);
    decode_tree(node->right, len + 1, it);
}

int huffman::decode_alpha(std::vector <char> &data) {
    int size = data[0];
    if (size == 0) return 1;
    for (int i = 1; i <= size; i++)
        symbol_list.push_back(data[i]);
    for (int i = size + 1; i <= size + (size + 1) / 2; i++) {
        symbol_len.push_back(data[i] >> 4);
        symbol_len.push_back((char)(data[i] << 4) >> 4);
    }
    int it = 0;
    decode_tree(tree, 0, it);
    return size + size / 2 + 1;
}

void huffman::decode(std::vector <char> &data, std::vector <char> &code) {
    int start = 0;
    if (!alpha_is_decoded) {
        start = decode_alpha(data);
        last_decoding_node = tree;
        alpha_is_decoded = true;
    }
    Node* node = last_decoding_node;
    int pos = pack_pos;
    for (int i = start; i < data.size(); i++) {
        while (pos < 8) {
            if (node->symbol != 0) {
                code.push_back(node->symbol);
                node = tree;
            }
            //bool where = data[i] & (1 << pos);
            bool where = data[i] & (1 << (7 - pos));
            if (where) node = node->right;
            else node = node->left;
            pos++;
        }
        pos = 0;
    }
    if (node->symbol != 0) {
        code.push_back(node->symbol);
        node = tree;
    }
    pack_pos = pos;
    last_decoding_node = node;
}
