//
// Created by sb on 1/25/26.
//
#include <database.h>
#include <iostream>
#include <utility>
#include <filesystem>
#include <fstream>
#include <random>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <thread>
#include <shared_mutex>
#include <mutex>
#include <ctime>

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
                        continue;
                }
                if (tmp == "$$$$$") {
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
                        continue;
                }
                if (tmp == "$$$$$") {
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
                        continue;
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
        std::unique_lock db_lock(db_mtx);
        std::unique_lock board_lock(boards[post.board_id].board_mtx);
        if (titles.contains(post.title)) {
                boards[post.board_id].bbs.insert(id);
        }
        else {
                titles.insert(post.title);
                boards[post.board_id].bbs.insert(id);
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
        std::shared_lock lock(board.board_mtx);
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
        std::unique_lock lock(db_mtx);
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

DB_with_Comm::DB_with_Comm() {
        comm_thread = std::thread(&DB_with_Comm::server, this);
        comm_thread.detach();
}


void DB_with_Comm::server() {
        const char* socket_path = "/tmp/cpp_python_socket";
        int server_fd, client_fd;
        struct sockaddr_un address;
        // 1. 소켓 생성
        server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_fd < 0) return;
        unlink(socket_path);
        memset(&address, 0, sizeof(address));
        address.sun_family = AF_UNIX;
        strncpy(address.sun_path, socket_path, sizeof(address.sun_path) - 1);
        // 2. 바인드 (소켓 주소/경로 설정)
        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
                perror("bind failed");
                return;
        }
        // 3. 리슨 (접속 대기 상태로 전환)
        if (listen(server_fd, 5) < 0) {
                perror("listen failed");
                return;
        }
        while (true) {
                // 4. 파이썬의 접속을 수락 (접속할 때까지 여기서 멈춤)
                client_fd = accept(server_fd, NULL, NULL);
                if (client_fd < 0) continue;
                std::cout << "CONNECTED" << std::endl;
                char buffer[1024];
                // 5. 연결된 이 소켓(client_fd)을 닫지 않고 계속 통신하는 루프
                while (true) {
                        memset(buffer, 0, 1024);
                        int valread = read(client_fd, buffer, 1024);
                        if (valread <= 0) {
                                // 파이썬이 소켓을 닫았거나 에러가 난 경우
                                std::cout << "CONNECTION LOST" << std::endl;
                                break;
                        }
                        // --- 데이터 처리 로직 ---
                        std::string msg(buffer, valread);
                        std::cout << "받은 메시지: " << msg << std::endl;
                        // 응답 송신 (소켓 유지)
                        std::string ack = "ACK";
                        send(client_fd, ack.c_str(), ack.length(), 0);
                }
                close(client_fd); // 이 세션이 완전히 끝났을 때만 닫음
        }
}

int main() {
        std::string my_data = "경희대 공지사항 크롤링 요청: " + std::to_string(12345);
        DB_with_Comm DB;
        while (true) {

        }
        return 0;
}
/////////////////