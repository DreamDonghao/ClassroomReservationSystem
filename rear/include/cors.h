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
        auto origin = req.get_header_value("Origin");
        if (req.method == "OPTIONS"_method)
        {
            if (!origin.empty())
            {
                res.set_header("Access-Control-Allow-Origin", origin);
                res.set_header("Access-Control-Allow-Credentials", "true");
            }
            res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            res.code = 204;
            res.end();
            return;
        }
    }

    void after_handle(crow::request& req, crow::response& res, context&)
    {
        const auto origin = req.get_header_value("Origin");
        if (!origin.empty())
        {
            res.set_header("Access-Control-Allow-Origin", origin);
            res.set_header("Access-Control-Allow-Credentials", "true");
        }
    }

};
#endif //CORS_H
