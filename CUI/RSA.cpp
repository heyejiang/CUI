#include "RSA.h"
/*
Type:         iMac20,2
Serial:       C02FR0WU046M
Board Serial: C02121100CD0000UE
SmUUID:       EDA5307C-264F-466D-871F-2593684DE4EB
Apple ROM:    60A37D648F37
*/
long long RSA::gcd(long long a, long long b) {
    return b == 0 ? a : gcd(b, a % b);
}
long long RSA::mod_pow(long long x, long long y, long long p) {
    long long res = 1;
    x = x % p;
    while (y > 0) {
        if (y & 1)
            res = (res * x) % p;
        y = y >> 1;
        x = (x * x) % p;
    }
    return res;
}
long long RSA::mod_inverse(long long a, long long m) {
    a = a % m;
    for (long long x = 1; x < m; x++) {
        if ((a * x) % m == 1)
            return x;
    }
    return -1;
}
bool RSA::is_prime(long long num) {
    if (num <= 1) return false;
    if (num <= 3) return true;
    if (num % 2 == 0 || num % 3 == 0) return false;
    for (long long i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0)
            return false;
    }
    return true;
}
long long RSA::generate_prime() {
    while (true) {
        long long prime = rand() % 100 + 100; // 生成100到200范围内的随机数
        if (is_prime(prime))
            return prime;
    }
}
std::vector<long long> RSA::string_to_blocks(const std::string& message) {
    std::vector<long long> blocks;
    for (size_t i = 0; i < message.size(); i += blockSize) {
        long long block = 0;
        for (int j = 0; j < blockSize && i + j < message.size(); j++) {
            block = (block << 8) + message[i + j];  // 每个字符占8位
        }
        blocks.push_back(block);
    }
    return blocks;
}
std::string RSA::blocks_to_string(const std::vector<long long>& blocks) {
    std::string message;
    for (long long block : blocks) {
        for (int i = blockSize - 1; i >= 0; i--) {
            char ch = (block >> (i * 8)) & 0xFF;
            if (ch != 0) message += ch;  // 忽略填充的空字符
        }
    }
    return message;
}
RSA::RSA() {
        srand(time(0));
        p = generate_prime();
        q = generate_prime();
        n = p * q;
        phi = (p - 1) * (q - 1);

        // 选择 e，使得 1 < e < phi 且 gcd(e, phi) = 1
        e = 2;
        while (e < phi && gcd(e, phi) != 1) {
            e++;
        }

        // 计算 d，使得 d * e ≡ 1 (mod phi)
        d = mod_inverse(e, phi);

        // 设置块大小，n 的比特长度（字节为单位）
        blockSize = (int)std::log2(n) / 8;
    }
RSA::RSA(long long publicKey, long long mod) : e(publicKey), n(mod) {
    d = 0;  // 未知私钥，不能解密
    blockSize = (int)std::log2(n) / 8;
}
RSA::RSA(long long privateKey, long long mod, bool isPrivateKey) : d(privateKey), n(mod) {
    e = 0;  // 未知公钥，不能加密
    blockSize = (int)std::log2(n) / 8;
}
std::pair<long long, long long> RSA::GetPublicKey() const {
    return { e, n };
}
std::pair<long long, long long> RSA::GetPrivateKey() const {
    return { d, n };
}
std::vector<long long> RSA::Encrypt(const std::string& message) {
    if (e == 0) {
        throw std::invalid_argument("Public key is not available for encryption.");
    }
    std::vector<long long> blocks = string_to_blocks(message);
    std::vector<long long> encrypted_blocks;
    for (long long block : blocks) {
        encrypted_blocks.push_back(mod_pow(block, e, n));
    }
    return encrypted_blocks;
}
std::string RSA::Decrypt(const std::vector<long long>& encrypted_blocks) {
    if (d == 0) {
        throw std::invalid_argument("Private key is not available for decryption.");
    }
    std::vector<long long> decrypted_blocks;
    for (long long block : encrypted_blocks) {
        decrypted_blocks.push_back(mod_pow(block, d, n));
    }
    return blocks_to_string(decrypted_blocks);
}