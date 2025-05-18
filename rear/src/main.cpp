//
// Created by donghao on 25-4-26.
//
#include <main.h>
web::Front front;

int main() {
    mysqlx::Client client(
        // "mysqlx://root:123456@47.93.221.71:33060/crs",
        "mysqlx://root:123456@127.0.0.1:33060/crs",
        mysqlx::ClientOption::POOL_MAX_SIZE, 10 // 最多10个连接
    );

    // 设置html文件根目录
    front.setHtmlRootPath("front/templates");
    // 设置css文件根目录
    front.setCssRootPath("front/static/css");
    // 设置js文件根目录
    front.setJsRootPath("front/static/js");

    crow::App<CORS> app;

    //添加跟路由
    CROW_ROUTE(app, "/")([]()-> std::string {
        return front.getHtml("index.html");
    });

    // 自动加载与文件同名的路由,没有指定正确文件类型默认返回html
    CROW_ROUTE(app, "/<string>")([](const std::filesystem::path &fileName)-> const std::string & {
        if (fileName.extension().string() == ".html") {
            return front.getHtml(fileName.filename());
        } else if (fileName.extension().string() == ".css") {
            return front.getCss(fileName.filename());
        } else if (fileName.extension().string() == ".js") {
            return front.getJs(fileName.filename());
        } else {
            return front.getHtml(fileName.filename().string() + ".html");
        }
    });

    // 登录状态请求路由
    CROW_ROUTE(app, "/api/isLogin")
    ([&](const crow::request &req) {
        mysqlx::Session sess = client.getSession();
        const std::string raw_cookie = req.get_header_value("Cookie");
        crow::json::wvalue res;
        if (const std::string sess_id = get_cookie(raw_cookie, "sess_id"); !sess_id.empty()) {
            try {
                auto result_cookie_id = sess.sql("select user_id from cookie_id where user_session_id = ?")
                        .bind(sess_id).execute();
                if (auto row_cookie_id = result_cookie_id.fetchOne()) {
                    auto result_users = sess.sql("select studentNumber,name,nowClassroom,onceClassroom from users where id = ?")
                        .bind(row_cookie_id[0].get<int>()).execute();
                    auto row_users = result_users.fetchOne();
                    res["isLogin"] = true;
                    res["user_id"] = row_cookie_id[0].get<int>();
                    res["student_number"] = row_users[0].get<std::string>();
                    res["username"] = row_users[1].get<std::string>();
                    return crow::response(200, res);
                }
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
            }
        }
        res["isLogin"] = false;
        return crow::response(200, res);
    });

    // 登录请求路由
    CROW_ROUTE(app, "/api/login").methods("POST"_method)
    ([&](const crow::request &req) {
        mysqlx::Session sess = client.getSession();
        const auto body = crow::json::load(req.body);

        //发送给前端的 json 内容
        crow::json::wvalue resJson;

        // 检测前端发送json文件的完整性
        if (!body || !body.has("studentNumber") || !body.has("password")) {
            resJson["success"] = false;
            resJson["message"] = "字段不完整";
            return crow::response(400, resJson);
        }

        try {
            crow::response response;
            // 查询数据库中的哈希密码
            auto result = sess.sql("SELECT password_hash,name,id FROM users WHERE studentNumber = ?")
                    .bind(std::string(body["studentNumber"].s())).execute();
            // 获取第一个满足条件的用户
            auto row = result.fetchOne();
            // 判断是否为空来检测是否存在满足条件的用户
            if (!row) {
                resJson["success"] = false;
                resJson["message"] = "用户不存在";
                return crow::response(404, resJson);
            }
            if (const std::string passwordHash = sha256(body["password"].s());
                row[0].get<std::string>() != passwordHash) {
                std::cout << row[0].get<std::string>() << std::endl;
                std::cout << passwordHash << std::endl;
                resJson["success"] = false;
                resJson["message"] = "密码错误";
                return crow::response(401, resJson);
            }

            response.code = 200;
            response.set_header("Content-Type", "application/json");
            std::cout << row[2].get<int>() << std::endl;
            std::string user_session_id = generate_session_id(std::to_string(row[2].get<int>()));
            response.add_header("Set-Cookie",
                                "sess_id=" + user_session_id + "; Path=/; HttpOnly");
            resJson["success"] = true;
            std::cout << user_session_id << std::endl;
            sess.sql("INSERT INTO cookie_id(user_session_id,user_id,user_name) VALUES (?, ?, ?)")
                    .bind(user_session_id, row[2].get<int>(), row[1].get<std::string>())
                    .execute();

            resJson["message"] = "登录成功";
            response.body = resJson.dump();
            return response;
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            resJson["success"] = false;
            resJson["message"] = std::string("数据库问题") + e.what();
            return crow::response(500, resJson);
        }
    });

    // 获取用户信息请求路由
    CROW_ROUTE(app, "/api/get_user_messages").methods("POST"_method)
    ([&](const crow::request &req) {
        mysqlx::Session sess = client.getSession();
        auto user_id = req.get_header_value("user_id");
        try {
            auto result = sess.sql("select studentNumber,name from users where id = ? ")
                .bind(user_id).execute();
            crow::json::wvalue res;
            auto row = result.fetchOne();
            res["user_name"] = row[1].get<std::string>();
            res["user_student_number"] = row[0].get<std::string>();
            return crow::response(200, res);
        }catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
        return crow::response(400, {});
    });

    // 注册请求路由
    CROW_ROUTE(app, "/api/register").methods("POST"_method)
    ([&](const crow::request &req) {
        mysqlx::Session sess = client.getSession();
        const auto body = crow::json::load(req.body);
        crow::json::wvalue res;

        if (!body || !body.has("username") || !body.has("studentNumber")
            || !body.has("password") || !body.has("confirmPassword")) {
            res["success"] = false;
            res["message"] = "字段不完整喵！";
            return crow::response(400, res);
        }

        const std::string password = body["password"].s();
        try {
            // 插入新用户
            sess.sql("INSERT INTO users (name,studentNumber ,password_hash) VALUES (?, ?, ?)")
                    .bind(static_cast<std::string>(body["username"].s()),
                          static_cast<std::string>(body["studentNumber"].s()),
                          sha256(body["password"].s())).execute();

            res["success"] = true;
            res["message"] = "注册成功";
            return crow::response(200, res);
        } catch (const std::exception &e) {
            res["success"] = false;
            res["message"] = std::string("数据库") + e.what();
            return crow::response(500, res);
        }
    });

    // 获取教室信息请求路由
    CROW_ROUTE(app, "/api/getClassrooms")
    ([&] {
        mysqlx::Session sess = client.getSession();
        auto result = sess.sql("select id,buildingNumber,floorNumber,classroomNumber from classrooms").
                execute();
        std::vector<crow::json::wvalue> classrooms;
        for (mysqlx::Row row: result) {
            crow::json::wvalue classroom;
            classroom["id"] = row[0].get<int>();
            classroom["buildingNumber"] = row[1].get<std::string>();
            classroom["floorNumber"] = row[2].get<std::string>();
            classroom["classroomNumber"] = row[3].get<std::string>();
            std::string reservationStatuStr;
            try {
                std::stringstream ss;
                ss << row[3];
                reservationStatuStr = ss.str();
            } catch (...) {
                reservationStatuStr = "{}";
            }
            classroom["reservationStatu"] = crow::json::load(reservationStatuStr);
            classrooms.emplace_back(classroom);
        }
        crow::json::wvalue res;
        res["classrooms"] = std::move(classrooms);
        return res;
    });

    // 添加教室预约信息请求路由
    CROW_ROUTE(app, "/api/reserveClassroom").methods("POST"_method)
    ([&](const crow::request &req) {
        mysqlx::Session sess = client.getSession();
        const auto body = crow::json::load(req.body);
        crow::json::wvalue res;
        try {
            auto result = sess.
                    sql(
                        "SELECT COUNT(*) FROM reservations WHERE classroom_id = ? AND user_id = ? AND year = ? AND month = ? AND day = ? AND time_period = ?")
                    .bind(static_cast<int>(body["classroom_id"].i()),
                          static_cast<int>(body["user_id"].i()),
                          static_cast<int>(body["year"].i()),
                          static_cast<int>(body["month"].i()),
                          static_cast<int>(body["day"].i()),
                          static_cast<std::string>(body["time_period"].s())).execute();

            if (result.count() > 0) {
                std::cout<<static_cast<int>(body["classroom_id"].i())<<
                        static_cast<std::string>(body["username"].s())<<
                       static_cast<int>(body["user_id"].i())<<
                       static_cast<int>(body["year"].i())<<
                       static_cast<int>(body["month"].i())<<
                       static_cast<int>(body["day"].i())<<
                       static_cast<std::string>(body["time_period"].s())<<std::endl;
                sess.sql(
                    "INSERT INTO reservations (classroom_id, user_id, username, year, month, day, time_period) VALUES (?, ?, ?, ?, ?, ?, ?)"
                ).bind(
                    static_cast<int>(body["classroom_id"].i()),
                    static_cast<int>(body["user_id"].i()),
                    static_cast<std::string>(body["username"].s()),
                    static_cast<int>(body["year"].i()),
                    static_cast<int>(body["month"].i()),
                    static_cast<int>(body["day"].i()),
                    static_cast<std::string>(body["time_period"].s())
                ).execute();

                res["success"] = true;
                res["message"] = "预约成功";
                return crow::response(200, res);
            } else {
                res["success"] = false;
                res["message"] = "改教室在这个时间段已被预约";
            }

            std::string timesJson = crow::json::wvalue(body["times"]).dump();
            std::cout << timesJson << std::endl;
            sess.sql("UPDATE classrooms SET reservationStatu = ? WHERE id = ?")
                    .bind(timesJson, static_cast<int>(body["id"].i()))
                    .execute();
        } catch (const std::exception &e) {
            res["success"] = false;
            std::cerr<<e.what()<<std::endl;
            return crow::response(500, res);
        }
        return crow::response(200, res);
    });

    CROW_ROUTE(app, "/api/get_reservations_classroom_id").methods("POST"_method)
    ([&](const crow::request &req) {
        mysqlx::Session sess = client.getSession();  // 拿到连接
        const auto body = crow::json::load(req.body);  // 解析 JSON

        crow::json::wvalue res;

        if (!body || !body.has("year") || !body.has("month") || !body.has("date")) {
            res["error"] = "缺少字段";
            return crow::response(400, res);
        }

        try {
            std::vector<crow::json::wvalue> reservations;
            auto result = sess
                .sql("SELECT classroom_id,time_period,username,user_id FROM reservations WHERE year = ? AND month = ? AND day = ?")
                .bind(
                    static_cast<int>(body["year"].i()),
                    static_cast<int>(body["month"].i()),
                    static_cast<int>(body["date"].i())
                )
                .execute();

            for (const auto& row : result) {
                crow::json::wvalue r;
                r["classroom_id"] = row[0].get<int>();
                r["time_period"] = row[1].get<std::string>();
                r["username"] = row[2].get<std::string>();
                r["user_id"] = row[3].get<int>();
                reservations.emplace_back(r);
            }

            res["reservations"] = std::move(reservations);
            return crow::response(200, res);
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            res["error"] = std::string("数据库") + e.what();
            return crow::response(500, res);
        }
    });



    std::thread clearBuffer([&]() {
        std::string command;
        while (std::cin >> command) {
            if (command == "clear") {
                front.clearBuffer();
            }
        }
    });
    clearBuffer.detach();

    //启动web应用
    app.port(18080).multithreaded().run();

    return 0;
}
