#include "../includes/Clash.hpp"

size_t writeCallBack(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t total = size * nmemb;
    std::string *s = static_cast<std::string *>(userp);
    s->append(static_cast<char *>(contents), total);
    return total;
}

std::string fetchClanMembers(const std::string & clanTag)
{
    // Si aucun token, on renvoie JSON factice pour développement
    const char *rawToken = std::getenv("CLASH_TOKEN");
    if (!rawToken) {
        json j;
        j["members"] = json::array();
        for (int i = 1; i <= 10; ++i) {
            json m;
            m["tag"] = "#TAG" + std::to_string(1000 + i);
            m["name"] = "Player" + std::to_string(i);
            m["role"] = (i%5==0) ? "leader" : "member";
            m["lastSeen"] = "20251025T144030.000Z";
            m["expLevel"] = 10 + i;
            m["trophies"] = 4000 + i*50;
            m["clanRank"] = i;
            m["previousClanRank"] = i+1;
            m["donations"] = i*10;
            m["donationsReceived"] = i*5;
            m["arenaName"] = "Arena" + std::to_string((i%12)+1);
            j["members"].push_back(m);
        }
        return j.dump();
    }

    // Préparer le token (accepte soit "Bearer ..." soit juste le token)
    std::string token(rawToken);
    if (token.rfind("Bearer ", 0) != 0) {
        token = std::string("Bearer ") + token;
    }

    // Encode minimalement le clanTag : s'assure que le tag commence par %23
    std::string tagEsc = clanTag;
    if (tagEsc.empty()) tagEsc = std::string();
    else if (tagEsc[0] == '#') tagEsc.replace(0,1, "%23");
    else tagEsc = std::string("%23") + tagEsc;

    std::string url = "https://api.clashroyale.com/v1/clans/" + tagEsc + "/members";

    CURL *curl = curl_easy_init();
    if (!curl) throw std::runtime_error("Impossible d'initialiser libcurl");

    std::string readBuffer;
    struct curl_slist *headers = NULL;
    std::string authHeader = std::string("Authorization: ") + token;
    headers = curl_slist_append(headers, authHeader.c_str());
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "[WARN] curl error: " << curl_easy_strerror(res) << ", fallback to fake data" << std::endl;
        // fallback to fake JSON
        json j;
        j["members"] = json::array();
        for (int i = 1; i <= 10; ++i) {
            json m;
            m["tag"] = "#TAG" + std::to_string(1000 + i);
            m["name"] = "Player" + std::to_string(i);
            m["role"] = (i%5==0) ? "leader" : "member";
            m["lastSeen"] = "20251025T144030.000Z";
            m["expLevel"] = 10 + i;
            m["trophies"] = 4000 + i*50;
            m["clanRank"] = i;
            m["previousClanRank"] = i+1;
            m["donations"] = i*10;
            m["donationsReceived"] = i*5;
            m["arenaName"] = "Arena" + std::to_string((i%12)+1);
            j["members"].push_back(m);
        }
        return j.dump();
    }
    if (http_code < 200 || http_code >= 300) {
        std::cerr << "[WARN] API returned HTTP " << http_code << ", body: " << readBuffer << ", fallback to fake data" << std::endl;
        // fallback to fake JSON
        json j;
        j["members"] = json::array();
        for (int i = 1; i <= 10; ++i) {
            json m;
            m["tag"] = "#TAG" + std::to_string(1000 + i);
            m["name"] = "Player" + std::to_string(i);
            m["role"] = (i%5==0) ? "leader" : "member";
            m["lastSeen"] = "20251025T144030.000Z";
            m["expLevel"] = 10 + i;
            m["trophies"] = 4000 + i*50;
            m["clanRank"] = i;
            m["previousClanRank"] = i+1;
            m["donations"] = i*10;
            m["donationsReceived"] = i*5;
            m["arenaName"] = "Arena" + std::to_string((i%12)+1);
            j["members"].push_back(m);
        }
        return j.dump();
    }

    return readBuffer;
}

// Expose également la récupération de la current river race
std::string fetchCurrentRiverRace(const std::string &clanTag)
{
    const char *rawToken = std::getenv("CLASH_TOKEN");
    if (!rawToken) return std::string();
    std::string token(rawToken);
    if (token.rfind("Bearer ", 0) != 0) token = std::string("Bearer ") + token;

    std::string tagEsc = clanTag;
    if (tagEsc.empty()) tagEsc = std::string();
    else if (tagEsc[0] == '#') tagEsc.replace(0,1, "%23");
    else tagEsc = std::string("%23") + tagEsc;
    std::string url = "https://api.clashroyale.com/v1/clans/" + tagEsc + "/currentriverrace";

    CURL *curl = curl_easy_init();
    if (!curl) return std::string();
    std::string readBuffer;
    struct curl_slist *headers = NULL;
    std::string authHeader = std::string("Authorization: ") + token;
    headers = curl_slist_append(headers, authHeader.c_str());
    headers = curl_slist_append(headers, "Accept: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "[WARN] curl error (river race): " << curl_easy_strerror(res) << ", skipping river race" << std::endl;
        return std::string();
    }
    if (http_code < 200 || http_code >= 300) {
        std::cerr << "[WARN] API returned HTTP " << http_code << " for river race, body: " << readBuffer << ", skipping river race" << std::endl;
        return std::string();
    }
    return readBuffer;
}
