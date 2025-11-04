#include "../includes/Clash.hpp"

std::vector<RankedEntry> top3ByDonations(const std::vector<PlayerStats>& v)
{
    std::vector<RankedEntry> out;
    for (const auto &p : v) { RankedEntry r; r.name = p.name; r.value = p.donations; out.push_back(r); }
    std::sort(out.begin(), out.end(), [](const RankedEntry &a, const RankedEntry &b){ return a.value > b.value; });
    if (out.size() > 3) out.resize(3);
    return out;
}

RankGain bestRankGain(const std::vector<PlayerStats> &v)
{
    RankGain best{"", 0, 0, 0};
    for (const auto &p : v) {
        int gain = p.previousClanRank - p.clanRank;
        if (gain > best.gain) {
            best.gain = gain; best.name = p.name; best.prevRank = p.previousClanRank; best.curRank = p.clanRank;
        }
    }
    return best;
}

RatInfo findRat(const std::vector<PlayerStats> &v)
{
    RatInfo r; r.found = false; r.ratScore = -9999;
    for (const auto &p : v) {
        int score = p.donations - p.donationsReceived;
        if (score < r.ratScore || !r.found) { r.found = true; r.name = p.name; r.donations = p.donations; r.received = p.donationsReceived; r.ratScore = score; }
    }
    return r;
}
