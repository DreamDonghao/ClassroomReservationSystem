//
// Created by donghao on 25-5-5.
//

#ifndef SHA256_H
#define SHA256_H
#include <sstream>

std::string sha256(const std::string &input);

// 生成随机 session ID
std::string generate_session_id(const std::string &user_id);

std::string get_cookie(const std::string& raw_cookie, const std::string& key);

#endif //SHA256_H
