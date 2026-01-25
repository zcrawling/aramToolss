//
// Created by sb on 1/25/26.
//
#include <database.h>
#include <iostream>
#include <utility>
#include <filesystem>
#include <fstream>
#import <random>

Post::Post(const std::string &url, const std::string &title, const std::string &main,
        const uint64_t date, const uint64_t board_id):
url(url), title(title), main(main), date(date), board_id(board_id){};

Database::Database() : boards{} {
        std::ifstream readFile;
        readFile.open("../db/titles.txt");
        if (!readFile.is_open()) {
                std::cerr << "error occurred during initialization of DB" << std::endl;
                exit(100);
        }
        std::string tmp;
        while (std::getline(readFile, tmp)) {
                //TODO() /db/boards에서 bbs 불러오기 ,boards초기화
                if (!tmp.empty()) {
                        titles.insert(tmp);
                } else break;
        }
        readFile.close();
}

void Database::store(Post &post) {
        uint64_t id = hashing(post);
        if (titles.contains(post.title)) {
                boards[post.board_id].bbs.push(id);
        }
        else {
                titles.insert(post.title);
                std::string pwd = "../db/" + std::to_string(id);;
                std::filesystem::path p(pwd);
                std::filesystem::create_directories(p);
                std::string path = pwd + "/data.txt";
                if (std::ofstream file(path); file.is_open()) {
                        file <<(post.url + "\n");
                        file <<(post.title + "\n");
                        file <<(std::to_string(post.date) + "\n");
                        file <<(std::to_string(post.board_id) +"\n");
                        file <<post.main;
                        file.close();
                }
                // TODO() 이미지 저장은 추후 구현

        }


}

void Database::serve(Board board, uint64_t range, uint64_t at) {
        //TODO() 어떻게할지 나중에 판단
}

serve(Board board, uint64_t range, uint64_t at); {

}

void Database::save_before_crash() {
// TODO() boards.txt, titles업데이트
}
std::string Database::convert_time_string(const uint64_t time) {
        std::string ret = "";
        std::random_device rd;
        std::mt19937 gen(rd());
        if (time < 10000000000000) {
                ret+= std::to_string(time);
                std::uniform_int_distribution<> dis(100000, 999999);
                ret +=std::to_string(dis(gen));
        }
        else if (time < 1000000000000000) {
                ret+= std::to_string(time);
                std::uniform_int_distribution<> dis(1000, 9999);
                ret +=std::to_string(dis(gen));
        }
        else {
                ret += std::to_string(time);
        }
        return ret;
}

uint64_t Database::hashing(Post &post) {
//TODO() 제목 최대 상위 19자를 %10하여 파일이름으로.
}
