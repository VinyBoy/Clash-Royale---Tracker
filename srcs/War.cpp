#include "../includes/Clash.hpp"

std::vector<TopClanEntry> top3ClanStrongestWithWar(const std::vector<PlayerStats> &players)
{
    std::vector<TopClanEntry> out;
    for (const auto &p : players) {
        TopClanEntry e;
        e.name = p.name;
        e.trophies = p.trophies;
        e.donations = p.donations;
        e.warPoints = p.warPoints;
        e.inactiveDays = p.inactiveDays;
        e.score = p.trophies + p.warPoints/2 - p.inactiveDays*10;
        out.push_back(e);
    }
    std::sort(out.begin(), out.end(), [](const TopClanEntry &a, const TopClanEntry &b){ return a.score > b.score; });
    if (out.size() > 3) out.resize(3);
    return out;
}

std::unordered_map<std::string, WarParticipant> parseRiverRace(const std::string &jsonStr)
{
    std::unordered_map<std::string, WarParticipant> map;
    // stub : aucun participant
    if (jsonStr.empty()) return map;
    try {
        json j = json::parse(jsonStr);
        // Plusieurs formes possibles selon l'endpoint :
        // - { "clan": { "members": [...] } }
        // - { "team": { "participants": [...] } }
        // - { "participants": [...] }
        std::vector<json> candidates;
        if (j.contains("participants") && j["participants"].is_array()) candidates.push_back(j["participants"]);
        if (j.contains("clan") && j["clan"].contains("participants") && j["clan"]["participants"].is_array()) candidates.push_back(j["clan"]["participants"]);
        if (j.contains("team") && j["team"].contains("participants") && j["team"]["participants"].is_array()) candidates.push_back(j["team"]["participants"]);

        for (auto &arr : candidates) {
            for (auto &p : arr) {
                WarParticipant wp;
                wp.tag = p.value("tag", std::string());
                wp.name = p.value("name", std::string());
                wp.warPoints = p.value("warPoints", p.value("points", 0));
                wp.attacks = p.value("attacks", 0);
                if (!wp.tag.empty()) map[wp.tag] = wp;
            }
        }
    } catch (...) {
        // parsing failed -> retourner map vide
    }
    return map;
}

void mergeWarStats(std::vector<PlayerStats> &players, const std::unordered_map<std::string, WarParticipant> &warMap)
{
    for (auto &p : players) {
        auto it = warMap.find(p.tag);
        if (it != warMap.end()) {
            p.didWar = true;
            p.warPoints = it->second.warPoints;
        } else {
            p.didWar = false;
            p.warPoints = 0;
        }
    }
}

std::vector<RankedEntry> top3ByWar(const std::vector<PlayerStats>& v)
{
    std::vector<RankedEntry> out;
    for (const auto &p : v) {
        RankedEntry r; r.name = p.name; r.value = p.warPoints; out.push_back(r);
    }
    std::sort(out.begin(), out.end(), [](const RankedEntry &a, const RankedEntry &b){ return a.value > b.value; });
    if (out.size() > 3) out.resize(3);
    return out;
}

std::vector<KickMember> findKickList(const std::vector<PlayerStats> &v)
{
    std::vector<KickMember> out;
    for (const auto &p : v) {
        if (p.inactiveDays >= 10 && p.role == "member") {
            KickMember k; k.name = p.name; k.inactiveDays = p.inactiveDays; k.role = p.role; k.trophies = p.trophies; k.didWar = p.didWar; k.warPoints = p.warPoints; out.push_back(k);
        }
    }
    return out;
}

void ensureOutputFolder()
{
    std::filesystem::create_directories("files");
}

std::string getCurrentDate();
