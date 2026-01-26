//
// Created by sb on 1/25/26.
//

#ifndef ARAMTOOL_DATABASE_H
#define ARAMTOOL_DATABASE_H

#define MAX_BOARD 256

#include <cstdint>
#include <queue>
#include <string>
#include <set>


struct Post;

struct Board {
    std::string url;
    std::string name;
    std::multiset<uint64_t,std::greater<>> bbs;
};
struct Post {
    std::string url;
    std::string title;
    std::string main;
    std::vector<std::string> images;
    uint64_t date;
    uint64_t board_id;
    Post(const std::string &url, const std::string &title, const std::string &main, uint64_t date, uint64_t board_id);
};

class Database {
private:
    std::set<std::string> titles;
    Board boards[MAX_BOARD];

public:
    Database();

    ~Database();

    void store(Post &post);
    static bool serve(Board board, std::multiset<uint64_t,std::greater<>> &ret, uint64_t at, uint64_t range);
    void save_before_crash();
    static std::string convert_time_string(uint64_t time);
    static uint64_t hashing(const Post & post);
    static uint64_t hashing(const std::string & str);

};


#endif //ARAMTOOL_DATABASE_H