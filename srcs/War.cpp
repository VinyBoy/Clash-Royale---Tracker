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
    if (jsonStr.empty()) {
        return map;
    }

    try {
        const json root = json::parse(jsonStr);
        std::vector<json> arrays;

        auto addIfArray = [&arrays](const json &node) {
            if (node.is_array()) {
                arrays.push_back(node);
            }
        };

        if (root.contains("participants")) addIfArray(root["participants"]);
        if (root.contains("clan") && root["clan"].contains("participants")) addIfArray(root["clan"]["participants"]);
        if (root.contains("team") && root["team"].contains("participants")) addIfArray(root["team"]["participants"]);
        if (root.contains("items")) {
            for (const auto &item : root["items"]) {
                if (item.contains("participants")) addIfArray(item["participants"]);
                if (item.contains("clan") && item["clan"].contains("participants")) addIfArray(item["clan"]["participants"]);
            }
        }

        for (const auto &arr : arrays) {
            for (const auto &p : arr) {
                WarParticipant wp;
                wp.tag = p.value("tag", std::string());
                wp.name = p.value("name", std::string());

                if (p.contains("fame")) {
                    wp.warPoints = p.value("fame", 0);
                } else if (p.contains("pointsEarned")) {
                    wp.warPoints = p.value("pointsEarned", 0);
                } else if (p.contains("points")) {
                    wp.warPoints = p.value("points", 0);
                } else {
                    wp.warPoints = p.value("warPoints", 0);
                }

                if (p.contains("decksUsed")) {
                    wp.attacks = p.value("decksUsed", 0);
                } else if (p.contains("decksUsedToday")) {
                    wp.attacks = p.value("decksUsedToday", 0);
                } else if (p.contains("boatAttacks")) {
                    wp.attacks = p.value("boatAttacks", 0);
                } else {
                    wp.attacks = p.value("attacks", 0);
                }

                if (!wp.tag.empty()) {
                    map[wp.tag] = wp;
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "[WARN] parseRiverRace JSON error: " << e.what() << '\n';
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
