#include <iostream>
#include <fstream>
#include <cmath>

#include "Integer.h"
#include "RSA.h"

void generate_key_pair(int size=50, const std::string &pk_file_name="pk.txt", const std::string &sk_file_name="sk.txt")
{
    using IntegerType = Integer<>;
    PublicKey<IntegerType> pk;
    SecreteKey<IntegerType> sk;
    gen_key_pair<IntegerType, 255>(size, pk, sk);

    std::cout<<"Writing pk file..."<<std::endl;
    std::ofstream pk_f(pk_file_name);
    pk_f<<pk.e<<std::endl;
    pk_f<<pk.n<<std::endl;
    pk_f<<pk.fragment_size<<std::endl;
    pk_f<<pk.encrypt_fragment_size<<std::endl;
    pk_f<<pk.encrypt_byte_val<<std::endl;
    pk_f.flush();
    pk_f.close();
    std::cout<<"Writing to pk file done. "<<pk_file_name<<std::endl;

    std::cout<<"Writing sk file..."<<std::endl;
    std::ofstream sk_f(sk_file_name);
    sk_f<<sk.d<<std::endl;
    sk_f<<sk.n<<std::endl;
    sk_f<<sk.fragment_size<<std::endl;
    sk_f<<sk.encrypt_fragment_size<<std::endl;
    sk_f<<sk.encrypt_byte_val<<std::endl;
    std::cout<<"Writing to sk file done. "<<sk_file_name<<std::endl;
}

void encrypt_cmd(const std::string &stuff, std::string output = "", bool is_stuff_path=false, bool is_output_path=false, const std::string &pk_file_name="pk.txt", bool base64=true)
{
    if (output.empty() && is_output_path) output = stuff + ".e";

    std::cout<<"Reading pk file..."<<std::endl;
    std::ifstream in(pk_file_name);
    if (!in) {
        std::cout<<"Read pk file failed, "<<pk_file_name<<std::endl;
        return;
    }
    PublicKey<Integer<>> pk;
    in>>pk.e;
    in>>pk.n;
    in>>pk.fragment_size;
    in>>pk.encrypt_fragment_size;
    in>>pk.encrypt_byte_val;
    in.close();

    std::cout<<"> pk:"<<std::endl;
    std::cout<<"> e: "<<pk.e<<std::endl;
    std::cout<<"> n: "<<pk.n<<std::endl;
    std::cout<<"> fragment_size: "<<pk.fragment_size<<std::endl;
    std::cout<<"> encrypt_fragment_size: "<<pk.encrypt_fragment_size<<std::endl;
    std::cout<<"> encrypt_byte_val: "<<pk.encrypt_byte_val<<std::endl;
    std::cout<<std::endl;

    std::cout<<"Reading stuff: "<<stuff<<std::endl;
    std::vector<uint8_t> M;
    if (is_stuff_path)
    {
        std::ifstream f(stuff, std::ios::in | std::ios::binary);
        char c;
        while (f.get(c)) M.push_back(c);
        f.close();
    } else {
        M = string_to_bytes(stuff);
    }

    auto C = encrypt(M, pk);
    std::cout<<"Encryption done!"<<std::endl;

    if (base64) {
        auto C_base64 = bytes_to_base64(C);
        if (is_output_path)
        {
            std::ofstream out(output);
            if (!out) {
                std::cout<<"Can\'t open output file."<<std::endl;
                return;
            }
            out << C_base64;
            out.close();
        } else std::cout<<C_base64<<std::endl;
    } else {
        if (is_output_path)
        {
            std::ofstream out(output, std::ios::out | std::ios::binary);
            for (auto &c:C) out.put(c);
            out.close();
        } else std::cout<<bytes_to_base64(C)<<std::endl;
    }
}

void decrypt_cmd(const std::string &stuff, std::string output = "", bool is_stuff_path=false, bool is_output_path=false, const std::string &sk_file_name="sk.txt", bool base64=true)
{
    if (output.empty() && is_output_path) output = stuff + ".d";

    std::cout<<"Reading sk file..."<<std::endl;
    std::ifstream in(sk_file_name);
    if (!in) {
        std::cout<<"Read sk file failed, "<<sk_file_name<<std::endl;
        return;
    }
    SecreteKey<Integer<>> sk;
    in>>sk.d;
    in>>sk.n;
    in>>sk.fragment_size;
    in>>sk.encrypt_fragment_size;
    in>>sk.encrypt_byte_val;
    in.close();

    std::cout<<"> sk:"<<std::endl;
    std::cout<<"> d: "<<sk.d<<std::endl;
    std::cout<<"> n: "<<sk.n<<std::endl;
    std::cout<<"> fragment_size: "<<sk.fragment_size<<std::endl;
    std::cout<<"> encrypt_fragment_size: "<<sk.encrypt_fragment_size<<std::endl;
    std::cout<<"> encrypt_byte_val: "<<sk.encrypt_byte_val<<std::endl;
    std::cout<<std::endl;

    std::cout<<"Reading stuff: "<<stuff<<std::endl;
    std::vector<uint8_t> C;
    if (is_stuff_path)
    {
        std::ifstream f(stuff, std::ios::in | std::ios::binary);
        char c;
        while (f.get(c)) C.push_back(c);
        f.close();

        if (base64) C = base64_to_bytes(bytes_to_string(C));
    } else {
        if (base64)
            C = base64_to_bytes(stuff);
        else
            C = string_to_bytes(stuff);
    }

    auto M = decrypt(C, sk);
    std::cout<<"Decryption done!"<<std::endl;


    if (is_output_path)
    {
        std::ofstream out(output, std::ios::out | std::ios::binary);
        for (auto &c:M) out.put(c);
        out.close();
    } else std::cout<<bytes_to_string(M)<<std::endl;
}

void print_help()
{
    std::cout<<R"(
|===========================================================|
|                                                           |
|                RSA Tool made by Raiix                     |
|                                                           |
|===========================================================|
command:
g [size=512] [public key file path=pk.txt] [secrete key file path=sk.txt] - Generate RSA key pairs
e <stuff that need to be encrypted> <output> [is_stuff_path=false] [is_output_path=false] [public key file path=pk.txt] [base64=true] - Encrypt file using public key
d <stuff that need to be decrypted> <output> [is_stuff_path=false] [is_output_path=false] [secrete key file path=sk.txt] [base64=true] - Decrypt file using secrete key
)"<<std::endl;
}

template<typename T>
T to_(const std::string &s)
{
    std::stringstream ss;
    ss<<s;
    T x;
    ss>>x;
    return x;
}

template<>
bool to_<bool>(const std::string &s)
{
    if (s == "True" || s == "true" || s == "t" || s == "T" || s =="yes" || s == "Yes" || s == "Y" || s == "y") return true;
    if (s == "False" || s == "false" || s == "f" || s == "F" || s =="no" || s == "No" || s == "N" || s == "n") return false;
    std::stringstream ss;
    ss<<s;
    bool x;
    ss>>x;
    return x;
}



int main(int argv, char** _args) {
    std::vector<std::string> args;
    for (int i=1; i<argv; ++i) args.emplace_back(_args[i]);

    if (args.size() == 0) {
        print_help();
    } else if (args[0] == "g")
    {
        if (args.size() >= 4) generate_key_pair(to_<int>(args[1]), args[2], args[3]);
        else if (args.size() >= 3) generate_key_pair(to_<int>(args[1]), args[2]);
        else if (args.size() >= 2) generate_key_pair(to_<int>(args[1]));
        else generate_key_pair();
    } else if (args[0] == "e")
    {
        if (args.size() >= 7) encrypt_cmd(args[1], args[2], to_<bool>(args[3]), to_<bool>(args[4]), args[5], to_<bool>(args[6]));
        else if (args.size() >= 6) encrypt_cmd(args[1], args[2], to_<bool>(args[3]), to_<bool>(args[4]), args[5]);
        else if (args.size() >= 5) encrypt_cmd(args[1], args[2], to_<bool>(args[3]), to_<bool>(args[4]));
        else if (args.size() >= 4) encrypt_cmd(args[1], args[2], to_<bool>(args[3]));
        else if (args.size() >= 3) encrypt_cmd(args[1], args[2]);
        else if (args.size() >= 2) encrypt_cmd(args[1]);
        else {
            std::cout<<"[ERROR] The argument number is less than 2! args.size = "<<args.size()<<std::endl;
            print_help();
        }
    } else if (args[0] == "d")
    {
        if (args.size() >= 7) decrypt_cmd(args[1], args[2], to_<bool>(args[3]), to_<bool>(args[4]), args[5], to_<bool>(args[6]));
        else if (args.size() >= 6) decrypt_cmd(args[1], args[2], to_<bool>(args[3]), to_<bool>(args[4]), args[5]);
        else if (args.size() >= 5) decrypt_cmd(args[1], args[2], to_<bool>(args[3]), to_<bool>(args[4]));
        else if (args.size() >= 4) decrypt_cmd(args[1], args[2], to_<bool>(args[3]));
        else if (args.size() >= 3) decrypt_cmd(args[1], args[2]);
        else if (args.size() >= 2) decrypt_cmd(args[1]);
        else {
            std::cout<<"[ERROR] The argument number is less than 2! args.size = "<<args.size()<<std::endl;
            print_help();
        }
    } else {
        std::cout<<"[ERROR] Unknown command: "<<args[0]<<std::endl;
        print_help();
    }
    return 0;
}
