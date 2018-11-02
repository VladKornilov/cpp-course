# include <bits/stdc++.h>
# include "vector.h"

using namespace std;

Vector::Vector() {
    small = 0;
    is_small = false;
    is_empty = true;
}

Vector::~Vector() {
    if (!is_empty && !is_small) {
        if (big->cnt > 1)
            big->cnt--;
        else delete big;
    }
}

size_t Vector::size() const {
    if (is_empty)
        return 0;
    if (is_small)
        return 1;
    return big->digits.size();
}

void Vector::push_back(unsigned int val) {
    if (is_empty) {
        small = val;
        is_small = true;
        is_empty = false;
    }
    else {
        if (is_small) {
            auto copy = new cow_vector();
            copy->cnt = 1;
            copy->digits.push_back(small);
            copy->digits.push_back(val);
            swap(copy, big);
            is_small = false;
        }
        else {
            cut();
            big->digits.push_back(val);
        }
    }
}

void Vector::pop_back() {
    if (is_small) {
        is_small = false;
        is_empty = true;
    }
    else {
        cut();
        big->digits.pop_back();
    }
}

unsigned int &Vector::operator[](unsigned int index) {
    if (is_small && index > 0) {
        resize(index + 1);
        return big->digits[index];
    }
    if (is_small)
        return small;
    cut();
    return big->digits[index];
}


const unsigned int &Vector::operator[](unsigned int index) const {
    if (is_small)
        return small;
    return big->digits[index];
}

Vector &Vector::operator=(const Vector &other) {
    if (other.is_empty)
        return *this;
    if (other.is_small) {
        if (!is_empty && !is_small && big->cnt > 1) {
            big->cnt--;
        }
        else if (!is_empty && !is_small) {
            delete big;
        }
        small = other.small;
    }
    else {
        if (big == other.big) {
            is_small = other.is_small;
            is_empty = other.is_empty;
            return *this;
        }
        if (!is_empty && !is_small && big->cnt > 1) {
            big->cnt--;
        }
        else if (!is_empty && !is_small) {
            delete big;
        }
        big = other.big;
        big->cnt++;
    }
    is_small = other.is_small;
    is_empty = other.is_empty;
    return *this;
}

unsigned int &Vector::back() {
    if (is_small)
        return small;
    return big->digits.back();
}

const unsigned int &Vector::back() const {
    if (is_small)
        return small;
    return big->digits.back();
}

void Vector::resize(size_t siz) {
    while (siz > size()) {
        push_back(0);
    }
    while (siz < size()) {
        pop_back();
    }
}

void Vector::cut() {
    if (big->cnt > 1) {
        auto copy = new cow_vector();
        big->cnt--;
        copy->digits = big->digits;
        copy->cnt = 1;
        swap(copy, big);
    }
}
