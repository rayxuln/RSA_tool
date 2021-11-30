//
// Created by Raiix on 2021/11/30.
//

#ifndef RSA_TOOL_INTEGER_H
#define RSA_TOOL_INTEGER_H

#include <stack>
#include <cmath>
#include <sstream>
#include <vector>
#include <stack>

template<typename T>
short math_sign(T x)
{
    if (x > T(0)) return 1;
    if (x < T(0)) return -1;
    return 0;
}

template<typename T>
void swap(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}

template<int DIGIT_NUM=2050, int DIGIT_VAL=10000, typename ElementType=int>
class Integer{
    ElementType digit_list[DIGIT_NUM];
    size_t digit_size;
    short sign;

    void _stream_element(std::stringstream &ss, size_t index, bool padding) const
    {
        if (!padding && digit_list[index] == 0) {
            ss<<'0';
            return;
        }

        std::stack<char> stack;
        int t = DIGIT_VAL/10;
        int x = digit_list[index];
        while ((padding) ? t : x)
        {
            stack.push('0'+x%10);
            t /= 10;
            x /= 10;
        }

        while (!stack.empty())
        {
            ss<<stack.top();
            stack.pop();
        }
    }

    ElementType _find_max_factor(Integer &b)
    {
        for (ElementType i=DIGIT_VAL-1; i>=1; --i)
        {
            auto c = b.multiply_fast(i);
            if (compare_abs(c) > 0)
            {
                return i;
            }
        }
        return 0;
    }

    ElementType _find_max_factor_bin(const Integer &b)
    {
        ElementType l=1, r=DIGIT_VAL;
        while((r-l) > 1)
        {
            auto m = (l+r)/2;
            auto c = b.multiply_fast(m);
            auto cmp = c.compare_abs(*this);
            if (cmp > 0) r = m;
            else l = m;
        }
        return l;
    }
public:
    inline size_t get_digit_size() {
        return digit_size;
    }

    Integer() {
        digit_size = 1;
        digit_list[0] = 0;
        sign = 0;
    }

    Integer(long long x) {
        if (x == 0) {
            digit_size = 1;
            digit_list[0] = 0;
            sign = 0;
            return;
        }
        digit_size = 0;
        sign = math_sign(x);
        while (x) {
            auto q = x % DIGIT_VAL;
            x /= DIGIT_VAL;
            digit_list[digit_size] = q;
            digit_size += 1;
        }
    }

    Integer(const std::string &s, bool raw = false) {
        if (raw) {
            for (size_t i=0; i<s.size(); ++i) digit_list[i] = s[i];
            digit_size = s.size();
            sign = 1;
            return;
        }
        short has_sign = 0;
        if (!s.empty()) has_sign = (s[0] == '-' || s[0] == '+') ? 1 : 0;
        digit_size = 0;
        int x = 0;
        int t = 1;
        for (long long i=((long long)s.size())-1; i>=has_sign; --i)
        {
            x += (s[i]-'0') * t;
            if (t >= DIGIT_VAL)
            {
                digit_list[digit_size++] = x % DIGIT_VAL;
                x /= DIGIT_VAL;
                t = 1;
            }
            t *= 10;
        }
        if (x || digit_size == 0) digit_list[digit_size++] = x;

        if (s.empty()) sign = 0;
        else sign = (digit_size==1&&digit_list[0]==0) ? 0 : (s[0] == '-' ? -1 : 1);
    }

    Integer(const Integer& b)
    {
        digit_size = b.digit_size;
        sign = b.sign;
        for (size_t i=0; i<b.digit_size; ++i)
        {
            digit_list[i] = b.digit_list[i];
        }
    }

    long long to_int() const {
        unsigned long long x = 0;
        for (size_t i=digit_size-1; i>=0; --i)
        {
            x += digit_list[i];
            if (i == 0) break;
            else x *= DIGIT_VAL;
        }
        return x * sign;
    }

    std::string to_string(bool raw=false) const {
        if (raw)
        {
            std::string s;
            s.resize(digit_size);
            for (size_t i=0; i<digit_size; ++i) s[i] = digit_list[i];
            return s;
        }

        std::stringstream ss;
        if (sign < 0) ss<<"-";

        for (size_t i=digit_size-1;; --i)
        {
            _stream_element(ss, i, i<digit_size-1 && digit_size!=1);
            if (i == 0) break;
        }
        return ss.str();
    }

    Integer abs(){
        Integer c(*this);
        if (c.sign < 0) c.sign = 1;
        return c;
    }

    short compare_abs(const Integer &b) const
    {
        if (digit_size > b.digit_size) return 1;
        else if (digit_size < b.digit_size) return -1;
        for (size_t i=digit_size-1;; --i)
        {
            if (digit_list[i] > b.digit_list[i]) return 1;
            else if(digit_list[i] < b.digit_list[i]) return -1;
            if (i == 0) break;
        }
        return 0;
    }

    short compare(const Integer &b) const
    {
        if (sign > 0 && b.sign <= 0) return 1;
        if (sign == 0 && b.sign < 0) return 1;
        if (sign < 0 && b.sign >= 0) return -1;
        if (sign == 0 && b.sign > 0) return -1;
        if (sign == 0 && b.sign == 0) return 0;

        short res = compare_abs(b);
        if (sign > 0) return res;
        if (sign < 0) return -res;
    }

    Integer add_abs(const Integer &_b) const
    {
        Integer _tb(_b);
        Integer _ta(*this);
        Integer *ap = &_ta;
        Integer *bp = &_tb;
        if (ap->compare_abs(*bp) < 0) swap(ap, bp);
        Integer &a = *ap;
        Integer &b = *bp;
        Integer c = a;

        for (size_t i=0; i<b.digit_size; ++i)
        {
            c.digit_list[i] += b.digit_list[i];
            if (c.digit_list[i] >= DIGIT_VAL) {
                if (i+1 == c.digit_size) {
                    c.digit_size += 1;
                    c.digit_list[c.digit_size-1] = 0;
                }
                c.digit_list[i+1] += c.digit_list[i] / DIGIT_VAL;
                c.digit_list[i] %= DIGIT_VAL;
            }
        }

        for (size_t i=b.digit_size; i<c.digit_size-1; ++i)
        {
            if (c.digit_list[i] >= DIGIT_VAL) {
                c.digit_list[i+1] += c.digit_list[i] / DIGIT_VAL;
                c.digit_list[i] %= DIGIT_VAL;
            } else break;
        }

        while (c.digit_list[c.digit_size-1] >= DIGIT_VAL) {
            c.digit_list[c.digit_size] = c.digit_list[c.digit_size-1] / DIGIT_VAL;
            c.digit_list[c.digit_size-1] %= DIGIT_VAL;
            c.digit_size += 1;
        }

        return c;
    }

    Integer sub_abs(const Integer &_b) const
    {
        Integer _tb(_b);
        Integer _ta(*this);
        Integer *ap = &_ta;
        Integer *bp = &_tb;
        if (ap->compare_abs(*bp) < 0) swap(ap, bp);
        Integer &a = *ap;
        Integer &b = *bp;
        Integer c = a;

        for (size_t i=0; i<b.digit_size; ++i)
        {
            c.digit_list[i] -= b.digit_list[i];
            if (c.digit_list[i] < 0)
            {
                c.digit_list[i+1] -= 1;
                c.digit_list[i] += DIGIT_VAL;
            }
        }

        for (size_t i=b.digit_size; i<c.digit_size-1; ++i)
        {
            if (c.digit_list[i] < 0)
            {
                c.digit_list[i+1] -= 1;
                c.digit_list[i] += DIGIT_VAL;
            }
        }

        while (c.digit_list[c.digit_size-1] == 0 && c.digit_size > 1) c.digit_size -= 1;

        return c;
    }

    Integer add(const Integer &b) const
    {
        if (sign == 0) return b;
        if (b.sign == 0) return *this;
        if (sign == b.sign) {
            Integer c = add_abs(b);
            c.sign = sign;
            return c;
        }
        Integer c = sub_abs(b);
        if (compare_abs(b) > 0){
            c.sign = sign;
        } else c.sign = b.sign;
        return c;
    }

    Integer sub(const Integer &b) const
    {
        if (sign == 0) {
            Integer c = b;
            c.sign *= -1;
            return c;
        }
        if (b.sign == 0) return *this;
        Integer new_b = b;
        new_b.sign *= -1;
        return add(new_b);
    }

    Integer multiply(const Integer &b) const
    {
        const Integer &a = *this;
        Integer c;
        c.digit_size = a.digit_size + b.digit_size;
        for (size_t i=0; i<c.digit_size; ++i) c.digit_list[i] = 0;

        for (size_t i=0; i<a.digit_size; ++i)
        {
            for (size_t j=0; j<b.digit_size; ++j)
            {
                c.digit_list[i+j] += a.digit_list[i] * b.digit_list[j];
            }
        }

        for (size_t i=0; i<c.digit_size; ++i)
        {
            if (c.digit_list[i] >= DIGIT_VAL)
            {
                if (i == c.digit_size-1)
                {
                    c.digit_list[c.digit_size++] = 0;
                }
                c.digit_list[i+1] += c.digit_list[i] / DIGIT_VAL;
                c.digit_list[i] %= DIGIT_VAL;
            }
        }

        while (c.digit_list[c.digit_size-1] == 0 && c.digit_size > 1) c.digit_size -= 1;

        c.sign = a.sign * b.sign;
        return c;
    }

    Integer left_shift(int n) const
    {
        const Integer &a = *this;
        Integer c;
        c.sign = a.sign;
        c.digit_size = a.digit_size + n;
        if (c.digit_size > DIGIT_NUM) {
            c.digit_size = 0;
            c.digit_list[0] = 0;
            c.sign = 0;
            return c;
        }
        for (int i=0; i<a.digit_size; ++i)
        {
            c.digit_list[i+n] = a.digit_list[i];
        }
        for (int i=0; i<n; ++i)
        {
            c.digit_list[i] = 0;
        }

        while (c.digit_list[c.digit_size-1] == 0 && c.digit_size > 1) c.digit_size -= 1;
        return c;
    }

    Integer right_shift(int n) const
    {
        const Integer &a = *this;
        Integer c;
        c.sign = a.sign;
        c.digit_size = a.digit_size - n;
        if (a.digit_size <= n) {
            c.digit_size = 1;
            c.digit_list[0] = 0;
            c.sign = 0;
            return c;
        }
        for (int i=0; i<c.digit_size; ++i)
        {
            c.digit_list[i] = a.digit_list[i+n];
        }
        return c;
    }

    Integer extract(int n) const
    {
        if (n <= 0) return Integer(0);
        const Integer &a = *this;
        if (a.digit_size <= n) return a;
        Integer c;
        c.digit_size = n;
        c.sign = a.sign;
        for (size_t i=0; i<c.digit_size; ++i)
        {
            c.digit_list[i] = a.digit_list[i];
        }
        return c;
    }

    Integer extract_digit(int i) const
    {
        Integer c;
        c.digit_size = 1;
        c.digit_list[0] = digit_list[i];
        c.sign = 1;
        return c;
    }

    Integer multiply_fast(ElementType _b) const
    {
        Integer b(0);
        b.digit_list[0] = _b;
        if (_b > 0) b.sign = 1;
        return multiply_fast(b);
    }

    Integer multiply_fast(const Integer &B) const
    {
        const Integer &A = *this;
        if (A.digit_size == 1 || B.digit_size == 1) return A.multiply(B);

        int an = A.digit_size;
        int a_pivot = an/2;
        Integer a = A.right_shift(a_pivot);
        Integer b = A.extract(a_pivot);

//        std::cout<<">=====| Divide Start |=====<"<<std::endl;
//        std::cout<<A.to_string()<<" = "<<a.to_string()<<"x10^"<<a_pivot<<" + "<<b.to_string()<<std::endl;

        int bn = B.digit_size;
        int b_pivot = bn/2;
        Integer c = B.right_shift(b_pivot);
        Integer d = B.extract(b_pivot);
//        std::cout<<B.to_string()<<" = "<<c.to_string()<<"x10^"<<b_pivot<<" + "<<d.to_string()<<std::endl;

        auto res1 = a.multiply_fast(c).left_shift(a_pivot+b_pivot);
//        std::cout<<"res1: ac10 = "<<a.to_string()<<" x "<<c.to_string()<<" x 10^"<<a_pivot+b_pivot<<" = "<<res1.to_string()<<std::endl;

        auto res2 = a.multiply_fast(d).left_shift(a_pivot);
//        std::cout<<"res2: ad10 = "<<a.to_string()<<" x "<<d.to_string()<<" x 10^"<<a_pivot<<" = "<<res2.to_string()<<std::endl;

        auto res3 = b.multiply_fast(c).left_shift(b_pivot);
//        std::cout<<"res3: bc10 = "<<b.to_string()<<" x "<<c.to_string()<<" x 10^"<<b_pivot<<" = "<<res3.to_string()<<std::endl;

        auto res4 = b.multiply_fast(d);
//        std::cout<<"res4: bd = "<<b.to_string()<<" x "<<d.to_string()<<" = "<<res4.to_string()<<std::endl;

        auto res = res1.add(res2).add(res3).add(res4);
//        std::cout<<"res = "<<res.to_string()<<std::endl;
//        std::cout<<std::endl;

        return res;
    }

    void zerofy()
    {
        digit_size = 1;
        digit_list[0] = 0;
        sign = 0;
    }

    void mod_div(const Integer &b, Integer &mod_res, Integer &div_res) const
    {
        const Integer &a = *this;
        mod_res.zerofy();
        div_res.zerofy();

        for (size_t i=a.digit_size-1;; --i)
        {
            mod_res = mod_res.left_shift(1);
            mod_res.digit_list[0] = a.digit_list[i];
            if (mod_res.digit_list[0] != 0) mod_res.sign = 1;

            div_res = div_res.left_shift(1);

            auto cmp = mod_res.compare_abs(b);
            if (cmp == 0) {
                div_res.digit_list[0] = 1;
                div_res.sign = 1;
                mod_res.zerofy();
            } else if (cmp < 0) {
                div_res.digit_list[0] = 0;
            } else {
                auto factor = mod_res._find_max_factor_bin(b);
                Integer f = b.multiply_fast(factor);
                div_res.digit_list[0] = factor;
                div_res.sign = 1;
                mod_res = mod_res.sub_abs(f);
                if (mod_res.digit_size == 1 && mod_res.digit_list[0] == 0) mod_res.sign = 0;
            }

            if (i == 0) break;
        }
    }

    Integer mod(const Integer &b) const
    {
        const Integer &a = *this;
        auto cmp = a.compare_abs(b);
        if (cmp == 0) return Integer(0);
        else if (cmp < 0) return a;

        Integer mod_res, div_res;
        mod_div(b, mod_res, div_res);

        return mod_res;
    }

    Integer div(const Integer &b) const
    {
        const Integer &a = *this;
        auto cmp = a.compare_abs(b);
        if (cmp == 0) return Integer(1);
        else if (cmp < 0) return Integer(0);

        Integer mod_res, div_res;
        mod_div(b, mod_res, div_res);

        return div_res;
    }

    operator long long() const{
        return to_int();
    }
};

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
bool operator== (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.compare(b) == 0;
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
bool operator!= (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.compare(b) != 0;
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
bool operator> (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.compare(b) > 0;
}
template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
bool operator>= (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.compare(b) >= 0;
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
bool operator<= (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.compare(b) <= 0;
}
template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
bool operator< (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.compare(b) <= 0;
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
auto operator+ (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.add(b);
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
auto operator- (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.sub(b);
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
auto operator* (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.multiply(b);
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
auto operator/ (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.div(b);
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
auto operator% (const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &b)
{
    return a.mod(b);
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
std::ostream &operator<<(std::ostream &out, const Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a)
{
    return out<<a.to_string();
}

template<int DIGIT_NUM, int DIGIT_VAL, typename ElementType>
std::istream &operator>>(std::istream &in, Integer<DIGIT_NUM, DIGIT_VAL, ElementType> &a)
{
    std::string s;
    in>>s;
    a = Integer<DIGIT_NUM, DIGIT_VAL, ElementType>(s);
    return in;
}

#endif //RSA_TOOL_INTEGER_H
