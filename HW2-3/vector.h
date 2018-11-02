#ifndef VECTOR_H
#define VECTOR_H

#include <vector>

struct cow_vector {
    std::vector<unsigned int> digits;
    unsigned int cnt;
};

struct Vector {
public:
    Vector();
    ~Vector();

    size_t size() const;
    void push_back(unsigned int val);
    void pop_back();
    unsigned int& operator[](unsigned int pos);
    const unsigned int& operator[](unsigned int pos) const;
    Vector& operator=(const Vector& other);
    unsigned int& back();
    const unsigned int& back() const;
    void resize(size_t sz);

private:
    void cut();

    union {
        unsigned int small;
        cow_vector* big;
    };

    bool is_small;
    bool is_empty;
};
#endif // VECTOR_H