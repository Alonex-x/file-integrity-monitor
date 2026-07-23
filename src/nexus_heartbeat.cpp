#include <iostream>
#include <string>
#include <curl/curl.h>
#include <unistd.h>

int main() {
    const char* api_base = std::getenv("NEXUS_API_BASE") ? std::getenv("NEXUS_API_BASE") : "http://localhost:8080";
    const char* api_key = std::getenv("FILE_MONITOR_AGENT_KEY") ? std::getenv("FILE_MONITOR_AGENT_KEY") : "CLAVE_POR_DEFECTO";

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error initializing curl" << std::endl;
        return 1;
    }

    std::string url = std::string(api_base) + "/api/v1/agents/heartbeat";
    struct curl_slist* headers = curl_slist_append(NULL, ("X-Agent-Key: " + std::string(api_key)).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    std::cout << "File-Monitor Agent started. Sending heartbeats..." << std::endl;
    while (true) {
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            std::cout << "Heartbeat sent: " << http_code << std::endl;
        } else {
            std::cerr << "Error sending heartbeat: " << curl_easy_strerror(res) << std::endl;
        }
        sleep(60);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return 0;
}
