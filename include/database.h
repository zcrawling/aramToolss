//
// Created by sb on 1/25/26.
//

#ifndef ARAMTOOL_DATABASE_H
#define ARAMTOOL_DATABASE_H

#define MAX_BOARD 256

#include <cstdint>
#include <queue>
#include <string>
#include <vector>
#include <set>


struct Post;

struct Board {
    std::string url;
    std::string name;
    std::priority_queue<uint64_t> bbs;
};
struct Post {
    std::string url;
    std::string title;
    std::string main;
    uint64_t date;
    uint64_t board_id;
    Post(const std::string &url, const std::string &title, const std::string &main, uint64_t date, uint64_t board_id);
};

class Database {
private:
    std::set<std::string> titles;
    Board boards[MAX_BOARD];
    uint64_t hashing(Post & post);

public:
    Database();
    void store(Post &post);
    void serve(Board board, uint64_t range, uint64_t at);
    void save_before_crash();
    static std::string convert_time_string(uint64_t time);

};


#endif //ARAMTOOL_DATABASE_H