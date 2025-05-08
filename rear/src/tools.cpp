//
// Created by donghao on 25-5-5.
//
#include <tools.h>
#include <openssl/sha.h>
#include <iomanip>
#include <random>
std::string sha256(const std::string &input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
    std::ostringstream oss;
    for (const unsigned char i : hash)
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);
    return oss.str();
}

// 生成随机 session ID
std::string generate_session_id(const std::string& user_id) {
    static std::string chars =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string session_id;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(chars.size())-1);
    for (int i = 0; i < 32; ++i)
        session_id += chars[dis(gen)];
    session_id +=sha256(user_id);
    return session_id;
}

std::string get_cookie(const std::string& raw_cookie, const std::string& key) {
    std::istringstream stream(raw_cookie);
    std::string token;

    while (std::getline(stream, token, ';')) {
        if (const auto pos = token.find('='); pos != std::string::npos) {
            std::string name = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            // 去除前导空格
            while (!name.empty() && std::isspace(name[0])) name.erase(0, 1);

            if (name == key) return value;
        }
    }
    return "";
}


