//
// Created by donghao on 25-5-5.
//

#ifndef CORS_H
#define CORS_H
#include <crow.h>
// 中间件,为所有路由自动加上跨域头,解决跨域问题
struct CORS
{
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context&)
    {
        // 预检请求处理：浏览器会先发一个 OPTIONS 请求
        if (req.method == "OPTIONS"_method)
        {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            res.code = 204; // No Content
            res.end();
        }
    }

    void after_handle(crow::request&, crow::response& res, context&)
    {
        // 所有响应都加上允许跨域头
        res.set_header("Access-Control-Allow-Origin", "*");
    }
};
#endif //CORS_H
