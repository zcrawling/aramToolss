#include "../include/crow.h"

int main() {
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")([](){ return "Hello C++ Web!"; });
    app.port(8080).multithreaded().run();
}