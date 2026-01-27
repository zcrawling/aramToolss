#include "../include/crow.h"
#include <unistd.h>
#include <limits.h>

int main() {
    // char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)) != NULL) {
    //     std::cout << "[SYSTEM] Current Working Directory: " << cwd << std::endl;
    // }
    crow::SimpleApp app;
    // app.loglevel(crow::LogLevel::Debug);
    // crow::mustache::set_base("../templates");
    CROW_ROUTE(app, "/")([]() {
        auto page = crow::mustache::load("index.html");
        return page.render();
    });
    app.port(8080).multithreaded().run();
}