#include "big_integer.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

uint32_t ones = UINT32_MAX;
uint64_t base = 1ll + ones;
int weight = 32;

//uint64_t base = static_cast<uint64_t>(1000);

big_integer::big_integer()
{
    sign = 1;
    digits.push_back(0);
}

big_integer::big_integer(big_integer const& other)
{
    sign = other.sign;
    digits = other.digits;
}

big_integer::big_integer(int64_t a) {
    sign = (a >= 0 ? 1 : -1);
    auto num = static_cast<uint64_t>(a * sign);
    while (num >= base) {
        digits.push_back(static_cast<uint32_t>(num % base));
        num /= base;
    }
    digits.push_back(static_cast<uint32_t>(num));
}

big_integer::big_integer(std::string const& str)
{
    sign = 1;
    big_integer pow = 1;
    for (size_t i = str.size() - 1; i < str.size(); i--) {
        if (str[i] == '-') {
            if (*this != 0)
                sign = -1;
        }
        else {
            *this += mul_short_abs(pow, static_cast<uint32_t>(str[i] - '0'));
            pow = mul_short_abs(pow, 10);
        }
    }
}

big_integer::~big_integer() = default;

big_integer& big_integer::operator=(big_integer const& other) = default;

big_integer& big_integer::operator+=(big_integer const& rhs)
{
    if (this->sign == 1 && rhs.sign == 1) {
        *this = add_abs(*this, rhs);
    }
    else if (this->sign == 1 && rhs.sign == -1) {
        *this = sub_abs(*this, rhs);
    }
    else if (this->sign == -1 && rhs.sign == 1) {
        *this = sub_abs(rhs, *this);
    }
    else if (this->sign == -1 && rhs.sign == -1) {
        *this = add_abs(*this, rhs);
        this->sign = -1;
    }
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs)
{
    if (this->sign == 1 && rhs.sign == 1) {
        *this = sub_abs(*this, rhs);
    }
    else if (this->sign == 1 && rhs.sign == -1) {
        *this = add_abs(*this, rhs);
    }
    else if (this->sign == -1 && rhs.sign == 1) {
        *this = add_abs(rhs, *this);
        this->sign = -1;
    }
    else if (this->sign == -1 && rhs.sign == -1) {
        *this = sub_abs(rhs, *this);
    }
    return *this;
}

big_integer& big_integer::operator*=(big_integer const& rhs)
{
    int sign = this->sign * rhs.sign;
    *this = mul_abs(*this, rhs);
    if (*this != 0)
        this->sign = sign;
    return *this;
}

big_integer& big_integer::operator/=(big_integer const& rhs)
{
    int sign = this->sign * rhs.sign;
    if (rhs.digits.size() == 1) {
        *this = div_short_abs(*this, rhs[0]).first;
    }
    else {
        *this = div_abs(*this, rhs).first;
    }
    if (*this != 0)
        this->sign = sign;
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs)
{
    int sign = this->sign;
    if (rhs < base) {
        *this = div_short_abs(*this, rhs[0]).second;
    }
    else {
        *this = div_abs(*this, rhs).second;
    }
    if (*this != 0)
        this->sign = sign;
    return *this;
}

big_integer& big_integer::operator&=(big_integer const& rhs)
{
    *this = bitwise(*this, rhs, '&');
    return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs)
{
    *this = bitwise(*this, rhs, '|');
    return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs)
{
    *this = bitwise(*this, rhs, '^');
    return *this;
}

big_integer& big_integer::operator<<=(int rhs)
{
    int offset = rhs / weight;
    rhs %= weight;
    this->digits.resize(static_cast<uint32_t>(this->digits.size() + offset + 1));
    for (size_t i = this->digits.size() - offset - 2; i < this->digits.size(); i--) {
        uint64_t shift = 1ull * (*this)[i] << rhs;
        if (shift >= base) {
            (*this)[i + offset + 1] += static_cast<uint32_t>(shift >> weight);
            shift &= ones;
        }
        (*this)[i + offset] = static_cast<uint32_t>(shift);
    }
    for (int i = 0; i < offset; i++) {
        (*this)[i] = 0;
    }
    shorten(*this);
    return *this;
}

big_integer& big_integer::operator>>=(int rhs)
{
    int offset = (rhs + weight - 1) / weight;
    rhs %= weight;
    (*this)[0] >>= rhs;
    uint32_t one = 1;
    uint32_t rhs_ones = 0;
    for (int i = 0; i < rhs; i++) {
        rhs_ones += one;
        one <<= 1;
    }
    for (int i = 0; i < offset - 1; i++) {
        (*this)[i] = 0;
    }

    for (int i = offset; i < (int)this->digits.size(); i++) {
        uint32_t left = (*this)[i] & rhs_ones;
        (*this)[i] >>= rhs;
        (*this)[i - offset] += left << (32 - rhs);
    }
    shorten(*this);
    if (this->sign == -1) (*this)--;
    return *this;
}

big_integer big_integer::operator+() const
{
    big_integer bi = *this;
    bi.sign *= bi.sign;
    return bi;
}

big_integer big_integer::operator-() const
{
    big_integer bi = *this;
    if (bi != 0)
        bi.sign *= -1;
    return bi;
}

big_integer big_integer::operator~() const
{
    big_integer tmp = -(*this + 1);
    return tmp;
}

big_integer& big_integer::operator++()
{
    return *this += 1;
}

big_integer big_integer::operator++(int)
{
    big_integer bi = *this;
    *this += 1;
    return bi;
}

big_integer& big_integer::operator--()
{
    return *this -= 1;
}

big_integer big_integer::operator--(int)
{
    big_integer bi = *this;
    *this -= 1;
    return bi;
}

big_integer operator+(big_integer a, big_integer const& b)
{
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b)
{
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b)
{
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b)
{
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b)
{
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b)
{
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b)
{
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b)
{
    return a ^= b;
}

big_integer operator<<(big_integer a, int b)
{
    return a <<= b;
}

big_integer operator>>(big_integer a, int b)
{
    return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b)
{
    if (a.sign != b.sign || a.digits.size() != b.digits.size()) return false;
    for (size_t i = 0; i < a.digits.size(); i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
    return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b)
{
    if (a.sign != b.sign) return a.sign < b.sign;
    if (a.digits.size() != b.digits.size()) {
        return a.sign * (a.digits.size() - b.digits.size()) < 0;
    }

    for (size_t i = a.digits.size() - 1; i < a.digits.size(); i--) {
        if (a[i] != b[i])
            return a.sign * (0ll + a[i] - b[i]) < 0;
    }
    return false;
}

bool operator>(big_integer const& a, big_integer const& b)
{
    return b < a;
}

bool operator<=(big_integer const& a, big_integer const& b)
{
    return (a < b) || (a == b);
}

bool operator>=(big_integer const& a, big_integer const& b)
{
    return (a == b) || (a > b);
}

std::string to_string(big_integer const& a)
{
    std::string res;
    if (a == 0) res = "0";
    big_integer tmp = a;
    int32_t dec_base = 100000000;
    int zeroes = 8;
    do {
        int32_t mod = big_integer::cast_to_uint(tmp % dec_base);
        if (tmp.digits.size() > 1) {
            for (int i = 0; i < zeroes; i++) {
                res += ('0' + mod % 10);
                mod /= 10;
            }
        }
        else {
            while (mod != 0) {
                res += ('0' + mod % 10);
                mod /= 10;
            }
        }
        tmp /= dec_base;
    } while (tmp != 0);

    if (a.sign == -1) res += '-';
    std::reverse(res.begin(), res.end());
    return res;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
    return s << to_string(a);
}

uint32_t &big_integer::operator[](int const& index) {
    return reinterpret_cast<uint32_t &>(this->digits[index]);
}
const uint32_t big_integer::operator[](int const& index) const {
    return this->digits[index];
}

uint32_t big_integer::cast_to_uint(big_integer const &a) {
    return a[0];
}

big_integer big_integer::abs(big_integer const &a) {
    big_integer tmp = a;
    tmp.sign = 1;
    return tmp;
}

big_integer big_integer::add_abs(big_integer const &a, big_integer const &b) {
    int carry = 0;
    big_integer sum, loc_a = a, loc_b = b;
    size_t len = std::max(a.digits.size(), b.digits.size());
    sum.digits.resize(len);
    loc_a.digits.resize(len);
    loc_b.digits.resize(len);

    for (size_t i = 0; i < len; i++) {
        uint64_t tmp_sum = 0ull + loc_a[i] + loc_b[i] + carry;
        if (tmp_sum >= base) {
            carry = 1;
            tmp_sum &= ones;
        }
        else {
            carry = 0;
        }
        sum[i] = static_cast<uint32_t>(tmp_sum);
    }
    if (carry != 0) sum.digits.push_back(carry);
    return sum;
}

big_integer big_integer::sub_abs(big_integer const &a, big_integer const &b) {
    big_integer sum, loc_a = abs(a), loc_b = abs(b);
    if (loc_a < loc_b) {
        sum.sign = -1;
        std::swap(loc_a, loc_b);
    }
    else {
        sum.sign = 1;
    }
    sum.digits.resize(loc_a.digits.size());
    loc_b.digits.resize(loc_a.digits.size());

    for (size_t i = 0; i < loc_a.digits.size(); i++) {

        uint32_t tmp_sum;
        if (loc_a[i] < loc_b[i]) {
            loc_a[i + 1]--;
            tmp_sum = static_cast<uint32_t>(base + loc_a[i] - loc_b[i]);
        }
        else {
            tmp_sum = loc_a[i] - loc_b[i];
        }
        sum[i] = tmp_sum;
    }
    shorten(sum);
    return sum;
}

big_integer big_integer::mul_abs(big_integer const &a, big_integer const &b) {
    if (a == 0 || b == 0) return 0;
    big_integer ans;
    ans.digits.resize(a.digits.size() + b.digits.size() + 1);

    for (size_t i = 0; i < a.digits.size(); i++) {
        uint32_t carry = 0;
        for (size_t j = 0; j < b.digits.size(); j++) {
            uint64_t tmp_mul = 1ull * a[i] * b[j] + carry + ans[i + j];
            carry = static_cast<uint32_t>(tmp_mul >> weight);
            ans[i + j] = static_cast<uint32_t>(tmp_mul & ones);
        }
        ans[i + b.digits.size()] += carry;
    }
    shorten(ans);
    return ans;
}

big_integer big_integer::mul_short_abs(big_integer const &a, uint32_t b) {
    big_integer ans;
    uint32_t carry = 0;
    ans.digits.resize(a.digits.size() + 1);
    for (size_t i = 0; i < a.digits.size(); i++) {
        uint64_t tmp_mul = 1ull * a[i] * b + carry;
        carry = static_cast<uint32_t>(tmp_mul >> weight);
        ans[i] = static_cast<uint32_t>(tmp_mul & ones);
    }
    ans[a.digits.size()] = carry;
    shorten(ans);
    return ans;
}

bool big_integer::cmp_prefix(big_integer const &r, big_integer const &d, uint32_t k, uint32_t m)
{
    size_t i = m, j = 0;
    while (i != j) {
        if (r[i + k] != d[i]) j = i;
        else i--;
    }
    return r[i + k] < d[i];
}

std::pair<big_integer, big_integer> big_integer::div_abs(big_integer const& a, big_integer const& b) {
    big_integer loc_a = a, loc_b = b;
    big_integer r, q;
    size_t n = loc_a.digits.size(), m = loc_b.digits.size();
    uint32_t f = static_cast<uint32_t>(base / (loc_b[m - 1] + 1));

    r = (f == 0) ? (loc_a << weight) : (mul_short_abs(loc_a, f));
    big_integer const d = (f == 0) ? (loc_b << weight) : (mul_short_abs(loc_b, f));

    q.digits.resize(n - m + 2);
    r.digits.push_back(0);
    big_integer dq;
    for (size_t k = n - m; k < n - m + 1; k--) {
        uint32_t qt;
        size_t km = k + m;
        if (r == 0) {
            qt = 0;
        }
        else {
            uint64_t r1 = ((1ull * r[km]) << weight) + r[km - 1];
            uint64_t d1 = d[m - 1];
            qt = std::min(static_cast<uint32_t>(r1 / d1), ones);
        }
        if (qt == 0)
            continue;

        dq = mul_short_abs(d, qt);
        dq.digits.push_back(0);
        while (cmp_prefix(r, dq, k, m)) {
            qt--;
            dq -= d;
        }
        q[k] = qt;
        r -= (dq << static_cast<int>(weight * k));
    }

    r = (f == 0) ? (r >> weight) : (div_short_abs(r, f).first);
    shorten(q);
    return {q, r};
}

std::pair<big_integer, uint32_t> big_integer::div_short_abs(big_integer const& a, uint32_t b) {
    big_integer div;
    div.digits.resize(a.digits.size());
    uint64_t mod = 0;
    for (size_t i = a.digits.size() - 1; i < a.digits.size(); i--) {
        uint64_t cur = a[i] + (mod << weight);
        div[i] = static_cast<uint32_t>(cur / b);
        mod = cur % b;
    }
    shorten(div);
    return {div, static_cast<uint32_t>(mod)};
}

big_integer big_integer::bitwise(big_integer a, big_integer b, char op) {
    size_t len = std::max(a.digits.size(), b.digits.size());
    a.digits.resize(len);
    b.digits.resize(len);
    if (a.sign == -1) {
        for (size_t i = 0; i < a.digits.size(); i++) {
            a[i] = ~a[i];
        }

        a.sign = 1;
        a++;
    }
    if (b.sign == -1) {
        for (size_t i = 0; i < b.digits.size(); i++) {
            b[i] = ~b[i];
        }
        b.sign = 1;
        b++;
    }

    big_integer sum;

    sum.digits.resize(len);
    for (size_t i = 0; i < len; i++) {
        switch (op) {
            case '&':
                sum[i] = a[i] & b[i];
                break;
            case '|':
                sum[i] = a[i] | b[i];
                break;
            case '^':
                sum[i] = a[i] ^ b[i];
                break;
            default:
                break;
        }
    }
    if (sum.digits.back() > ones >> 1) {
        for (size_t i = 0; i < sum.digits.size(); i++) {
            sum[i] = ~sum[i];
        }
        sum++;
        sum.sign = -1;
    }
    return sum;
}

void big_integer::shorten(big_integer &a) {
    while (a.digits.size() > 1 && a.digits.back() == 0) a.digits.pop_back();
}




