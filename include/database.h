//
// Created by sb on 1/25/26.
//

#ifndef ARAMTOOL_DATABASE_H
#define ARAMTOOL_DATABASE_H


#include <cstdint>
#include <queue>
#include <string>
#include <vector>
#include <set>

struct Post;

struct Board {
    std::string url;
    std::string name;
    std::queue<Post> bbs;
    bool is_shared;
};
struct Post {
    std::string url;
    std::string title;
    uint64_t date;
    uint64_t serial_num;
    Board board;
    std::vector<std::string> tags;
    std::vector<std::string> img_list;
    Post(std::string url, std::string title, uint64_t date, uint64_t serial_num,
        Board board,  std::vector<std::string> tags, std::vector<std::string> img_list);
};

class Database {
private:
    std::set<std::string> titles;
public:
    Database();
    bool store(Post &post);
    bool find_copy(Post post);
    bool edit_post(Post post);
    bool serve_all(Board board);
    void save_before_crash();
    std::string convert_time_string(uint64_t);
};


#endif //ARAMTOOL_DATABASE_H