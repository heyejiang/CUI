#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

class RSA {
private:
    long long p, q;  // 用于生成密钥的质数
    long long n;     // 模数 n = p * q 或直接提供的模数 n
    long long phi;   // 欧拉函数 φ(n)，仅在生成密钥时计算
    long long e;     // 公钥指数
    long long d;     // 私钥指数
    int blockSize;   // 每个加密块的大小
    // 辅助函数，用于求最大公约数
    long long gcd(long long a, long long b);
    // 辅助函数，用于快速幂取模计算 (x^y) % p
    long long mod_pow(long long x, long long y, long long p);
    // 辗转相除法求逆元，用于计算私钥 d
    long long mod_inverse(long long a, long long m);
    // 辅助函数，判断一个数是否为质数
    bool is_prime(long long num);
    // 随机生成一个大质数
    long long generate_prime();
    // 将字符串分块为整数表示
    std::vector<long long> string_to_blocks(const std::string& message);
    // 将整数块转回字符串
    std::string blocks_to_string(const std::vector<long long>& blocks);
public:
    // 默认构造函数：生成新的公钥和私钥
    RSA();
    // 构造函数：使用指定的公钥（用于加密）
    RSA(long long publicKey, long long mod);
    // 构造函数：使用指定的私钥（用于解密）
    RSA(long long privateKey, long long mod, bool isPrivateKey);
    // 获取公钥
    std::pair<long long, long long> GetPublicKey() const;
    // 获取私钥
    std::pair<long long, long long> GetPrivateKey() const;
    // 加密函数，接受明文字符串并返回密文块
    std::vector<long long> Encrypt(const std::string& message);
    // 解密函数，接受密文块并返回明文字符串
    std::string Decrypt(const std::vector<long long>& encrypted_blocks);
};