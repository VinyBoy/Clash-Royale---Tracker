#include "../includes/Clash.hpp"

namespace {
void appendHeader(std::string &buf, const std::string &key, const std::string &value)
{
    buf += key;
    buf += ": ";
    buf += value;
    buf += "\r\n";
}

struct UploadStatus {
    const char *data;
    size_t len;
};

size_t payloadSource(void *ptr, size_t size, size_t nmemb, void *userp)
{
    UploadStatus *upload = static_cast<UploadStatus *>(userp);
    size_t buffer_size = size * nmemb;
    if (upload->len == 0)
        return 0; // plus rien à envoyer

    size_t copy_size = std::min(buffer_size, upload->len);
    std::memcpy(ptr, upload->data, copy_size);
    upload->data += copy_size;
    upload->len -= copy_size;
    return copy_size;
}
}

static std::string buildEmailPayload(const std::string &from,
                                     const std::string &to,
                                     const std::string &subject,
                                     const std::string &body)
{
    std::string payload;
    appendHeader(payload, "From", from);
    appendHeader(payload, "To", to);
    appendHeader(payload, "Subject", subject);
    payload += "\r\n";
    payload += body;
    payload += "\r\n";
    return payload;
}

bool sendReportEmail(const std::string &publicReportPath,
                     const std::string &privateReportPath)
{
    const char *smtpHost = std::getenv("SMTP_HOST");
    const char *smtpUser = std::getenv("SMTP_USER");
    const char *smtpPass = std::getenv("SMTP_PASS");
    const char *smtpPort = std::getenv("SMTP_PORT");
    const char *fromAddr = std::getenv("SMTP_FROM");
    const char *toAddr   = std::getenv("SMTP_TO");

    if (!smtpHost || !smtpUser || !smtpPass || !fromAddr || !toAddr) {
        std::cerr << "[WARN] SMTP configuration incomplete (skip email)" << std::endl;
        return false;
    }

    std::string pubContent;
    std::string privContent;
    {
        std::ifstream pubFile(publicReportPath);
        std::stringstream ss;
        ss << pubFile.rdbuf();
        pubContent = ss.str();
    }
    {
        std::ifstream privFile(privateReportPath);
        std::stringstream ss;
        ss << privFile.rdbuf();
        privContent = ss.str();
    }

    std::string subject = "Rapports Clash Royale";
    std::string body = "Rapport public:\n\n" + pubContent + "\n\nRapport privé:\n\n" + privContent;
    std::string payload = buildEmailPayload(fromAddr, toAddr, subject, body);

    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[WARN] Impossible d'initialiser libcurl pour SMTP" << std::endl;
        return false;
    }

    std::string smtpUrl = std::string("smtp://") + smtpHost;
    if (smtpPort && *smtpPort) {
        smtpUrl += ":";
        smtpUrl += smtpPort;
    }

    struct curl_slist *recipients = nullptr;
    recipients = curl_slist_append(recipients, toAddr);

    UploadStatus upload{payload.c_str(), payload.size()};

    curl_easy_setopt(curl, CURLOPT_URL, smtpUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_USE_SSL, static_cast<long>(CURLUSESSL_ALL));
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-certificates.crt"); // trust store for STARTTLS
    curl_easy_setopt(curl, CURLOPT_USERNAME, smtpUser);
    curl_easy_setopt(curl, CURLOPT_PASSWORD, smtpPass);
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, fromAddr);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payloadSource);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    CURLcode res = curl_easy_perform(curl);

    if (recipients)
        curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "[WARN] email send failed: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    std::cout << "[INFO] Email envoyé à " << toAddr << std::endl;
    return true;
}
