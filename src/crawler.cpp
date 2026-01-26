
#define MG_ENABLE_BUNDLED_MALLOC 1
#define MG_ENABLE_TLS 1
#include "../include/mongoose.h"
#include <iostream>
#include <set>
#include <string>
#include <database.h>

class Crawler {
private:
    struct mg_mgr mgr{};
    std::set<uint32_t> seen_hashes; //TODO() database기반으로 변경

public:
    Crawler() { mg_mgr_init(&mgr); }
    ~Crawler() { mg_mgr_free(&mgr); }

    static void cb(struct mg_connection *c, const int ev, void *ev_data) {
        if (ev == MG_EV_CONNECT) {
            const auto host = "https://www.ssllabs.com/"; //TODO()

            mg_printf(c,
                "GET / HTTP/1.1\r\n"
                "Host: %s\r\n"
                "User-Agent: Mozilla/5.0\r\n"
                "Accept: */*\r\n"
                "Connection: close\r\n"
                "\r\n", host);

            std::cout << "[DEBUG] 요청 전송: Host=" << host << std::endl;
        }
        else if (ev == MG_EV_TLS_HS) {  // TLS 핸드셰이크 완료 확인
            std::cout << "[DEBUG] TLS 핸드셰이크 완료" << std::endl;
        }
        else if (ev == MG_EV_HTTP_MSG) {
            // [추가] HTML 응답 수신 및 파싱
            const struct mg_http_message *hm = static_cast<struct mg_http_message *>(ev_data);
            auto *self = static_cast<Crawler *>(c->fn_data); // run에서 넘긴 Crawler 객체
            std::string html(hm->body.buf, hm->body.len);
            std::cout << "[DEBUG] 응답 수신 완료 (크기: " << html.length() << "자)" << std::endl;

            // 타이틀 추출 로직
            const size_t s = html.find("<title>");

            if (const size_t e = html.find("</title>"); s != std::string::npos && e != std::string::npos) {
                const std::string title = html.substr(s + 7, e - s - 7);

                if (const uint32_t h = Database::hashing(title); self->seen_hashes.insert(h).second) {
                    std::cout << "\033[1;32m[NEW]\033[0m " << title << " (Hash: " << h << ")" << std::endl;
                } else {
                    std::cout << "\033[1;33m[OLD]\033[0m 중복 콘텐츠" << std::endl;
                }
            }
            c->is_closing = 1; // 작업 완료 후 세션 종료
        }
        else if (ev == MG_EV_ERROR) {
            std::cerr << "[ERR] 연결 실패: " << (char *)ev_data << std::endl;
        }
    }

    [[noreturn]] void run(const char *url) {
        mg_http_connect(&mgr, url, cb, this);
        struct mg_connection *c = mg_http_connect(&mgr, url, cb, this);
        if (c != NULL) {
            if (strncmp(url, "https://", 8) == 0) {
                const struct mg_tls_opts tls_opts = {nullptr};
                // ca 설정을 0으로 두면 인증서 검증을 건너뛰고 연결만 암호화합니다 (테스트용)
                mg_tls_init(c, &tls_opts);
                std::cout << "[SYSTEM] HTTPS(TLS) 모드 활성화" << std::endl;
            }
        }
        std::cout << "[SYSTEM] 크롤링 시작: " << url << std::endl;
        while (true) {
            mg_mgr_poll(&mgr, 1000);
        }


    }
};

int main() {
    Crawler crawler;
    crawler.run("https://www.ssllabs.com/"); //TODO() HTTPS 지원
    return 0;
}