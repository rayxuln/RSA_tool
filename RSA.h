//
// Created by Raiix on 2021/11/30.
//

#ifndef RSA_TOOL_RSA_H
#define RSA_TOOL_RSA_H

#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <vector>
#include <sstream>
#include <stack>

template<typename T>
void ext_gcd(T a, T b, T &c, T &x, T &y)
{
    if (b == T(0))
    {
        c = a;
        x = T(1);
        y = T(0);
        return;
    }
    T x1, y1;
    ext_gcd(b, a%b, c, x1, y1);
    x = y1;
    y = x1 - (a/b) * y1;
}

template<typename T>
T mul_with_mod(T a, T b, T c)
{
    T res = T(0);
    while (b > T(0))
    {
        res = (res + (b % T(2) * a) % c) % c;
        a = a * T(2) % c;
        b = b / T(2);
    }
    return res;
}

template<typename T, typename T2>
T pow_fast(T base, T2 pow)
{
    T res = T(1);
    while (pow > T2(0))
    {
        if (pow % T2(2) == T2(1)) {
            res = res * base;
        }
        pow = pow / T2(2);
        base = (base * base);
    }
    return res;
}

template<typename T, typename T2>
T pow_fast_with_mod(T base, T2 pow, T mod)
{
    T res = T(1);
    while (pow > T2(0))
    {
        if (pow % T2(2) == T2(1)) {
            res = res * base % mod;
        }
        pow = pow / T2(2);
        base = (base * base) % mod;
    }
    return res;
}

template<typename T>
bool is_prime(T p)
{
    const int prime_list[] = {1, 2, 3, 5, 7, 11, 13, 17, 19, 23,
                              29, 31, 37, 41, 43, 47, 53, 59, 61, 67,
                              71, 73, 79, 83, 89, 97, 101, 103, 107, 109,
                              113, 127, 131, 137, 139, 149, 151, 157, 163,
                              167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227};
    if (p == T(0)) return false;
    if (p == T(1)) return false;
    if (p == T(2)) return true;
    T t = p - T(1), k = T(0);
    while(t % T(2) == T(0)) {
        k = k + T(1);
        t = t / T(2);
    }
    for (auto _a : prime_list)
    {
        T a = T(_a);
        if (p == a) return true;
        T res = pow_fast_with_mod(a, t, p);
        for (T j=T(0); j < k; j = j + T(1))
        {
            T temp = mul_with_mod(res, res, p);
            if (temp == T(1) && res != T(1) && res != p - T(1)) return false;
            res = temp;
        }
        if (res != T(1)) return false;
    }
    return true;
}

template<typename T>
T gen_integer(int size, std::default_random_engine &e)
{
    T l_mod = pow_fast(T(10), size-1);
    T mod = l_mod * T(10);
    T res = T(1);
    while (res < mod) {
        res = res * T(e());
    }
    T temp = res % mod;
    // check highest digit if is 0
    if (temp / l_mod == T(0)) temp = temp + l_mod;
    return temp;
}

template<typename T>
T gen_prime(int size, std::default_random_engine &e, int max_cnt = -1)
{
    T mod = pow_fast(T(10), size);
    T x = gen_integer<T>(size, e);
    if (x % T(2) == T(0)) x = x + T(1);

    size_t cnt = 0;
    const size_t report_cnt = 1;
    while(!is_prime(x) || (x >= mod)) {
        x = gen_integer<T>(size, e);
        if (x % T(2) == T(0)) x = x + T(1);
        cnt += 1;

        if (max_cnt > 0 && cnt >= max_cnt) {
            std::cout<<"Fail to find a random prime number!"<<std::endl;
            return T(0);
        }

        if (cnt % report_cnt == 0) {
            std::cout<<"Have tested "<<cnt<<" fake prime number!"<<std::endl;
        }
    }

    std::cout<<"Found a prime number, tested "<<cnt<<" times."<<std::endl;
    return x;
}

template<typename T>
struct RSAKey{
    T n;
    size_t fragment_size; // how many bytes that can encode/decode at a time
    size_t encrypt_fragment_size; // how many digits that a encoded fragment need
    int encrypt_byte_val; // how many digits that a encrypted byte can store
};

template<typename T>
struct PublicKey : RSAKey<T>
{
    T e;
};

template<typename T>
struct SecreteKey : RSAKey<T>
{
    T d;
};

extern const size_t DIGIT_NUM_OF_ONE_BYTE;

template<typename T, int encrypt_byte_val=10>
void gen_key_pair(size_t size, PublicKey<T> &pk, SecreteKey<T> &sk)
{
    std::cout<<"> about to generate ras key pair that can handle at least "<<size<<" of digits at a time."<<std::endl;

    std::default_random_engine engine(time(nullptr));
    size_t p_size = size/2;
    size_t q_size = size - p_size;

    T p = gen_prime<T>(p_size, engine);
    T q = gen_prime<T>(q_size, engine);

    std::cout<<"> generated p: "<<p<<std::endl;
    std::cout<<"> generated q: "<<q<<std::endl;

    T n = p * q;
    std::cout<<"> calc n: "<<n<<std::endl;

    T e = T(17);
    std::cout<<"> choose e: "<<e<<std::endl;

    T d;
    {
        T _a = e, _b = (p - T(1)) * (q - T(1)), _c, _x, _y;
        ext_gcd(_a, _b, _c, _x, _y);
        d = (_x + _b) % _b;
        if (_c != T(1)) {
            std::cout<<"something went wrong while calculating d, maybe p or q is not a prime number."<<std::endl;
        }
//        std::cout<<"a: "<<_a<<", b: "<<_b<<", c: "<<_c<<", x: "<<_x<<", _y: "<<_y<<std::endl;
    }
    std::cout<<"> calc d: "<<d<<std::endl;

    T temp = n, temp2 = T(encrypt_byte_val);
    size_t encrypt_fragment_size = 0;
    while (temp > T(0))
    {
        temp = temp / temp2;
        encrypt_fragment_size += 1;
    }

    pk.e = e;
    pk.n = n;
    pk.fragment_size = (size-1)/DIGIT_NUM_OF_ONE_BYTE;
    pk.encrypt_fragment_size = encrypt_fragment_size;
    pk.encrypt_byte_val = encrypt_byte_val;

    sk.d = d;
    sk.n = n;
    sk.fragment_size = (size-1)/DIGIT_NUM_OF_ONE_BYTE;
    sk.encrypt_fragment_size = encrypt_fragment_size;
    sk.encrypt_byte_val = encrypt_byte_val;

    std::cout<<"> frag size: "<<pk.fragment_size<<std::endl;
    std::cout<<"> encrypt frag size: "<<pk.encrypt_fragment_size<<std::endl;
    std::cout<<"> encrypt_byte_val: "<<pk.encrypt_byte_val<<std::endl;
}

template<typename T>
std::string print_array(const std::vector<T> &arr)
{
    std::stringstream ss;
    ss<<"[";
    for (size_t i=0; i<arr.size(); ++i)
    {
        ss<<arr[i];
        if (i<arr.size()-1) ss<<", ";
    }
    ss<<"]";
    return ss.str();
}


template<typename T>
std::vector<uint8_t> encrypt(const std::vector<uint8_t> &row_data, const PublicKey<T> &pk)
{
    T _3_d = pow_fast(T(10), DIGIT_NUM_OF_ONE_BYTE);
    size_t row_data_pos = 0;
    std::vector<T> C_groups;
    while (row_data_pos < row_data.size())
    {
        T M = T(0);
        for (size_t i=0; i<pk.fragment_size; ++i)
        {
            M = M + T(row_data[row_data_pos++]);

            if (row_data_pos >= row_data.size() || i == pk.fragment_size-1) break;

            M = M * _3_d;
        }

        T C = pow_fast_with_mod(M, pk.e, pk.n);
        C_groups.push_back(C);
    }

//    std::cout<<"en: C_group = "<<print_array(C_groups)<<std::endl;

    // use one byte to store 1 digit
    std::vector<uint8_t> res;
    for (size_t i=0; i<C_groups.size(); ++i)
    {
        T C = C_groups[i];
        for (size_t j=0; j<pk.encrypt_fragment_size; ++j)
        {
            res.push_back(C % T(pk.encrypt_byte_val));
            C = C / T(pk.encrypt_byte_val);
        }
    }

    return res;
}

template<typename T>
std::vector<uint8_t> decrypt(const std::vector<uint8_t> &row_data, const SecreteKey<T> &sk)
{
    std::vector<T> C_groups;
    // use one byte to store 1 digit
    for (size_t i=0; i<row_data.size(); i += sk.encrypt_fragment_size)
    {
        T C = T(0);
        for (size_t j=0; j<sk.encrypt_fragment_size; ++j)
        {
            if (i+j >= row_data.size()) {
                std::cout<<"The source data is wrong, may be broken."<<std::endl;
                break;
            }
            auto t = T(row_data[row_data.size()-(i+j)-1]);
            C = C + t;
            if (j == sk.encrypt_fragment_size-1) break;
            C = C * T(sk.encrypt_byte_val);
        }
        C_groups.push_back(C);
    }

//    std::cout<<"de: C_group = "<<print_array(C_groups)<<std::endl;

    T _3_d = pow_fast(T(10), DIGIT_NUM_OF_ONE_BYTE);
    std::stack<uint8_t> res_stack;
    for (size_t i=0; i<C_groups.size(); ++i)
    {
        T M = pow_fast_with_mod(C_groups[i], sk.d, sk.n);

        for (size_t j=0; j<sk.fragment_size; ++j)
        {
            res_stack.push(M % _3_d);
            M = M / _3_d;
            if (M == T(0)) break;
        }
    }

    std::vector<uint8_t> res;
    while (!res_stack.empty())
    {
        res.push_back(res_stack.top());
        res_stack.pop();
    }

    return res;
}

std::vector<uint8_t> string_to_bytes(const std::string &s);
std::string bytes_to_string(const std::vector<uint8_t> &bytes);
std::string bytes_to_base64(const std::vector<uint8_t> &bytes);
std::vector<uint8_t> base64_to_bytes(const std::string &s);

#endif //RSA_TOOL_RSA_H
