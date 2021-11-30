//
// Created by Raiix on 2021/11/30.
//

#include "RSA.h"

const size_t DIGIT_NUM_OF_ONE_BYTE = 3;


std::vector<uint8_t> string_to_bytes(const std::string &s)
{
    std::vector<uint8_t> bytes;
    bytes.resize(s.size());
    for (size_t i=0; i<s.size(); ++i) bytes[i] = s[i];
    return bytes;
}

std::string bytes_to_string(const std::vector<uint8_t> &bytes)
{
    std::stringstream ss;
    for (auto &b : bytes) ss<<b;
    return ss.str();
}

const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
std::string bytes_to_base64(const std::vector<uint8_t> &bytes)
{
    std::stringstream ss;
    for (size_t i=0; i<bytes.size(); i += 3)
    {
        if (i + 2 < bytes.size())
        {
            auto a = bytes[i];
            auto b = bytes[i+1];
            auto c = bytes[i+2];

            ss.put(base64_table[(a&0xfc)>>2]);
            ss.put(base64_table[((b&0xf0)>>4) | ((a&0x03)<<4)]);
            ss.put(base64_table[((c&0xc0)>>6) | ((b&0x0f)<<2)]);
            ss.put(base64_table[(c&0x3f)]);
        } else if (i + 1 < bytes.size())
        {
            auto a = bytes[i];
            auto b = bytes[i+1];

            ss.put(base64_table[(a&0xfc)>>2]);
            ss.put(base64_table[((b&0xf0)>>4) | ((a&0x03)<<4)]);
            ss.put(base64_table[(b&0x0f)<<2]);
            ss.put('=');
        } else {
            auto a = bytes[i];

            ss.put(base64_table[(a&0xfc)>>2]);
            ss.put(base64_table[(a&0x03)<<4]);
            ss.put('=');
            ss.put('=');
        }
    }
    return ss.str();
}
std::vector<uint8_t> base64_to_bytes(const std::string &s)
{
    auto _inv_base64_table = [](uint8_t x){
        if (x == '=') return x;
        uint8_t index = 0;
        for (auto &a : base64_table)
        {
            if (x == a) return index;
            index += 1;
        }
        return index;
    };

    std::vector<uint8_t> res;
    for (size_t i=0; i<s.size(); i += 4)
    {
        if (i+3 < s.size())
        {
            uint8_t a = _inv_base64_table(s[i]);
            uint8_t b = _inv_base64_table(s[i+1]);
            uint8_t c = _inv_base64_table(s[i+2]);
            uint8_t d = _inv_base64_table(s[i+3]);

            res.push_back(((a&0x3f)<<2) | ((b&0x30)>>4));
            if (c != '=') {
                res.push_back(((b&0x0f)<<4) | ((c&0x3c)>>2));
                if (d != '=') {
                    res.push_back(((c&0x03)<<6) | ((d&0x3f)));
                }
            }
        }
    }
    return res;
}