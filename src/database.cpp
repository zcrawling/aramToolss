//
// Created by sb on 1/25/26.
//
#include <database.h>
#include <iostream>
#include <utility>
#import <random>

Post::Post(std::string url, std::string title, uint64_t date, uint64_t serial_num,
        Board board,  std::vector<std::string> tags, std::vector<std::string> img_list):
url(std::move(url)), title(std::move(title)), serial_num(serial_num), date(std::move(date)),
board(std::move(board)), tags(std::move(tags)), img_list(std::move(img_list)){};

Database::Database() {
        std::ifstream readFile;
        readFile.open("../db/titles");
        if (!readFile.is_open()) {
                std::cerr << "error occured during initalization of DB" <<std::endl;
                exit(100);

        }
        std::string tmp;
        while (std::getline(readFile, tmp)) {
                if (!tmp.empty()) {
                        titles.insert(tmp);
                }
                else break;
        }
        readFile.close();
}

bool Database::store(Post &post) {
        Database::titles.insert(post.title);

}

bool Database::find_copy(Post post) {

}

bool Database::edit_post(Post post) {

}

bool Database::serve_all(Board board) {

}

void Database::save_before_crash() {

}

std::string convert_time_string(uint64_t time) {
        std::string ret = "";
        std::random_device rd;
        std::mt19937 gen(rd());
        if (uint64_t < 10000000000000) {
                ret+= std::to_string(time);
                std::uniform_int_distribution<> dis(100000, 999999);
                ret +=std::to_string(dis(gen));
        }
        else if (uint64_t < 1000000000000000) {
                ret+= std::to_string(time);
                std::uniform_int_distribution<> dis(1000, 9999);
                ret +=std::to_string(dis(gen));
        }
        else {
                ret += std::to_string(time);
        }
        return ret;
}