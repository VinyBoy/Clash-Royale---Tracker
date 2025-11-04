#include "../includes/Clash.hpp"

std::vector<PlayerStats> buildPlayerStats(const std::string &jsonStr)
{
    std::vector<PlayerStats> out;
    if (jsonStr.empty()) {
        // Aucun JSON -> renvoyer vecteur vide
        return out;
    }

    try {
        json j = json::parse(jsonStr);
        // L'API peut renvoyer soit { "members": [...] } soit { "items": [...] }
        json arrayRef = json::array();
        if (j.contains("members") && j["members"].is_array()) arrayRef = j["members"];
        else if (j.contains("items") && j["items"].is_array()) arrayRef = j["items"];
        else return out;

        for (auto &m : arrayRef) {
            PlayerStats p;
            p.tag = m.value("tag", "#UNK");
            p.name = m.value("name", "Unknown");
            p.role = m.value("role", "member");
            p.trophies = m.value("trophies", 0);
            p.donations = m.value("donations", 0);
            // Some responses use donationsReceived, some might not
            p.donationsReceived = m.value("donationsReceived", m.value("cardsReceived", 0));
            p.clanRank = m.value("clanRank", 0);
            p.previousClanRank = m.value("previousClanRank", 0);
            p.lastSeenRaw = m.value("lastSeen", m.value("lastSeenRaw", std::string("")));
            // arena may be an object {"id":..., "name":...}
            if (m.contains("arena") && m["arena"].is_object()) p.arenaName = m["arena"].value("name", std::string());
            else p.arenaName = m.value("arenaName", std::string());
            p.clanChestPoints = m.value("clanChestPoints", 0);
            p.inactiveDays = computeInactiveDays(p.lastSeenRaw);
            p.warPoints = 0;
            p.didWar = false;
            out.push_back(p);
        }
    }
    catch (...) {
        // si parsing échoue, retourner vide
    }
    return out;
}

int computeInactiveDays(const std::string &isoUtc);
std::string getCurrentDate();
