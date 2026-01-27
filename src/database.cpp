//
// Created by sb on 1/25/26.
//
#include <database.h>
#include <iostream>
#include <utility>
#include <filesystem>
#include <fstream>
#include <random>


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
                if (!tmp.empty()) {
                        titles.insert(tmp);
                } else break;
        }
        readFile.close();
        readFile.open("../db/board/boards.txt");
        if (!readFile.is_open()) {
                std::cerr << "error occurred during initialization of DB" << std::endl;
                exit(101);
        }
        uint64_t now = 0;
        while (std::getline(readFile, tmp)) {
                if (tmp.empty()) {
                        break;
                }
                if (tmp == "#####") {
                        now++;
                }
                else if (tmp == "$$$$$") {
                        break;
                }
                boards[now].bbs.insert(std::stoull(tmp));
        }
        now = 0;
        while (std::getline(readFile, tmp)) {
                if (tmp.empty()) {
                        break;
                }
                if (tmp == "#####") {
                        now++;
                }
                else if (tmp == "$$$$$") {
                        break;
                }
                boards[now].name = tmp;
        }
        now = 0;
        while (std::getline(readFile, tmp)) {
                if (tmp.empty()) {
                        break;
                }
                if (tmp == "#####") {
                        now++;
                }
                else if (tmp == "$$$$$") {
                        break;
                }
                boards[now].url = tmp;
        }
        readFile.close();
}
Database::~Database() {
        save_before_crash();
}
void Database::store(Post &post) {
        uint64_t id = hashing(post);
        if (titles.contains(post.title)) {
                boards[post.board_id].bbs.insert(id);
        }
        else {
                titles.insert(post.title);
                std::string pwd = "../db/" + std::to_string(id);;
                std::filesystem::path p(pwd);
                std::filesystem::create_directories(p);
                std::string path = pwd + "/data.txt";
                if (std::ofstream readFile(path); readFile.is_open()) {
                        readFile <<(post.url + "\n");
                        readFile <<(post.title + "\n");
                        readFile <<(std::to_string(post.date) + "\n");
                        readFile <<(std::to_string(post.board_id) +"\n");
                        readFile <<post.main;
                        readFile.close();
                }
                // TODO() 이미지 저장은 추후 구현
        }


}

bool Database::serve(Board board, std::multiset<uint64_t,std::greater<>> &ret,
        const uint64_t at, const uint64_t range) {
        //at: 접근을 시작할 위치, range: serve할 게시글의 개수
        //1. 유효성 확인 -> board.bbs의 원소개수 확인하여 at이 존재하는지 확인
        //2. 최대 n개 serve
        if (board.bbs.size() <= at) {
                return false;
        }
        auto pt = board.bbs.begin();
        for (int i = 0; i < at; ++i) ++pt;
        if (board.bbs.size() < at + range ) {
                while (pt != board.bbs.end()) {
                        ret.insert(*pt);
                }
        }
        else {
                for (int i = 0; i < range; ++i) {
                        ret.insert(*pt);
                        ++pt;
                }
        }
        return true;
}

void Database::save_before_crash() {
        std::ofstream writeFile;
        writeFile.open("../db/titles.txt");
        if (!writeFile.is_open()) {
                std::cerr << "error occurred during saving titles..." << std::endl;
                exit(102);
        }
        for (auto pt = titles.begin(); pt != titles.end(); ++pt) {
                writeFile <<*pt <<"\n";
        }
        writeFile.close();
        writeFile.open("../db/board/boards.txt");
        if (!writeFile.is_open()) {
                std::cerr << "error occurred during saving boards..." << std::endl;
                exit(103);
        }
        for (auto pt = titles.begin(); pt != titles.end(); ++pt) {
                writeFile << *pt <<"\n";
        }
        uint64_t now = 0;
        for (; now <MAX_BOARD; ++now) {
                if (!boards[now].bbs.empty()) {
                        for (auto pt = boards[now].bbs.begin(); pt != boards[now].bbs.end(); ++pt) {
                                writeFile<< *pt <<"\n";
                        }
                }
                writeFile <<"#####\n";
        }
        writeFile <<"$$$$$\n";
        for (now = 0; now <MAX_BOARD; ++now) {
                if (boards[now].name.empty()) {
                        writeFile <<"#####\n";
                }
                else writeFile<< boards[now].name <<"\n";
        }
        writeFile <<"$$$$$\n";
        for (now = 0; now <MAX_BOARD; ++now) {
                if (boards[now].url.empty()) {
                        writeFile <<"#####\n";
                }
                else writeFile<< boards[now].url <<"\n";
        }
        writeFile <<"$$$$$\n";
        writeFile.close();
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

uint64_t Database::hashing(const std::string & str) {
        // FNV-1a 64bit로 hashing
        uint64_t hash = 14695981039346656037ULL;
        for (const char c : str) {
                hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
                hash *= 1099511628211ULL;
        }
        return hash;
}

uint64_t Database::hashing(const Post & post) {
        // FNV-1a 64bit로 hashing
        return hashing(post.title);
}