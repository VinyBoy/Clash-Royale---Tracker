
#include "../includes/Clash.hpp"

// ==========================
// Parsing du lastSeen en jours d'inactivité
// ==========================
//
// lastSeen format: "20251025T144030.000Z"
// On renvoie combien de jours depuis cette date jusqu'à maintenant.
//
int computeInactiveDays(const std::string &isoUtc)
{
    if (isoUtc.size() < 16)
	    return 9999; // cas bizarre -> on considère ultra inactif

    // Extraire champs
    int year   = std::stoi(isoUtc.substr(0,4));
    int month  = std::stoi(isoUtc.substr(4,2));
    int day    = std::stoi(isoUtc.substr(6,2));
    int hour   = std::stoi(isoUtc.substr(9,2));
    int minute = std::stoi(isoUtc.substr(11,2));
    int second = 0;
    if (isoUtc.size() >= 16)
        second = std::stoi(isoUtc.substr(13,2)); // "144030" -> "30" à partir index 13

    // On construit un tm en UTC
    std::tm tmSeen = {};
    tmSeen.tm_year = year - 1900;
    tmSeen.tm_mon  = month - 1;
    tmSeen.tm_mday = day;
    tmSeen.tm_hour = hour;
    tmSeen.tm_min  = minute;
    tmSeen.tm_sec  = second;

    // time_t pour le moment "vu pour la dernière fois" (en supposant UTC)
    // mktime() suppose localtime, pas UTC. On va approx : on considère que
    // l'écart en jours ne sera pas gravement impacté par le fuseau.
    time_t tSeen = timegm(&tmSeen); // GNU/glibc: convertit tm (UTC) -> time_t
    if (tSeen == -1) {
        // fallback au cas où timegm n'est pas dispo sur ta plate-forme
        // (école 42 Debian -> normalement glibc donc ok)
        return 9999;
    }

    time_t now = time(NULL);

    double diffSeconds = difftime(now, tSeen); // now - seen
    int days = (int)(diffSeconds / (60.0 * 60.0 * 24.0));
    if (days < 0) days = 0;
    return days;
}

std::string getCurrentDate()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
	#ifdef _WIN32
    localtime_s(&tm, &t);
	#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}



