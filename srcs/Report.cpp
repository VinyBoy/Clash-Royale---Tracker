#include "../includes/Clash.hpp"

// Generate reports using std::string concatenation. The helpers in Data_Members
// and War are used to fill the sections (top lists, rat, rank gain, etc.).

std::string MakePublicReport(const std::vector<PlayerStats> &stats)
{
    try {
        std::string out;
        out.reserve(4096);
        out += "Semaine - Rapport de Clan\n\n";

        // Top du Clan (performance globale)
        out += "Top du Clan 🏆 (performance globale)\n";
        auto topPerf = top3ClanStrongestWithWar(stats);
        for (size_t i = 0; i < topPerf.size(); ++i) {
            const auto &p = topPerf[i];
            out += std::to_string(i+1) + ". " + p.name + " — score " + std::to_string(p.score) + " | " + std::to_string(p.trophies) + " trophées | " + std::to_string(p.donations) + " dons | " + std::to_string(p.warPoints) + " pts guerre | actif il y a " + std::to_string(p.inactiveDays) + "j\n";
        }
        out += "\n";

        // Top Guerre
        out += "Top Guerre de Clan ⚔\n";
        auto topWar = top3ByWar(stats);
        for (size_t i = 0; i < topWar.size(); ++i) {
            out += std::to_string(i+1) + ". " + topWar[i].name + " — " + std::to_string(topWar[i].value) + " pts guerre\n";
        }
        out += "\n";

        // Top Donateurs
        out += "Top Donateurs 🤝\n";
        auto topDon = top3ByDonations(stats);
        for (size_t i = 0; i < topDon.size(); ++i) {
            out += std::to_string(i+1) + ". " + topDon[i].name + " — " + std::to_string(topDon[i].value) + " cartes données\n";
        }
        out += "\n";

        // Montée de la semaine
        auto gain = bestRankGain(stats);
        if (!gain.name.empty() && gain.gain > 0) {
            out += "Montée de la semaine 📈\n";
            out += gain.name + " a gagné " + std::to_string(gain.gain) + " places (rang " + std::to_string(gain.prevRank) + " → " + std::to_string(gain.curRank) + ")\n\n";
        }

        // Rat de la semaine
        auto rat = findRat(stats);
        if (rat.found) {
            out += "Rat de la semaine 🐀\n";
            out += rat.name + " a donné " + std::to_string(rat.donations) + " cartes mais a reçu " + std::to_string(rat.received) + " cartes 😭\n\n";
        }

        out += "Rappel ⚠\nFaites vos guerres, restez actifs, et go top ladder ❤️\n";
        return out;
    } catch (...) {
        return std::string("Semaine - Rapport de Clan\n\n(Erreur de génération du rapport public)\n");
    }
}

std::string MakePrivateReport(const std::vector<PlayerStats> & stats)
{
    try {
        std::string oss;
        oss.reserve(8192);
        oss += "Rapport interne (chefs / co-leaders seulement)\n\n";

        // 1) Inactivité (>=3 jours)
        oss += "Inactivité (>=3 jours sans se connecter) :\n";
        std::vector<PlayerStats> inactive;
        for (const auto &p : stats) if (p.inactiveDays >= 3) inactive.push_back(p);
        std::sort(inactive.begin(), inactive.end(), [](const PlayerStats &a, const PlayerStats &b){ return a.inactiveDays > b.inactiveDays; });
        for (const auto &p : inactive) {
            oss += "- " + p.name + " — inactif depuis " + std::to_string(p.inactiveDays) + " jours — rôle " + p.role + " — " + std::to_string(p.trophies) + " trophées\n";
        }
        oss += "\n";

        // 2) A PAS fait la guerre (0 participation)
        oss += "A PAS fait la guerre ⚔ (0 participation) :\n";
        std::vector<PlayerStats> noWar;
        for (const auto &p : stats) {
            if (!p.didWar || p.warPoints == 0) noWar.push_back(p);
        }
        std::sort(noWar.begin(), noWar.end(), [](const PlayerStats &a, const PlayerStats &b){ if (a.warPoints != b.warPoints) return a.warPoints > b.warPoints; return a.inactiveDays > b.inactiveDays; });
        for (const auto &p : noWar) {
            oss += "- " + p.name + " — " + std::to_string(p.warPoints) + " pts guerre — inactif " + std::to_string(p.inactiveDays) + "j — rôle " + p.role + "\n";
        }
        oss += "\n";

        // 3) Candidats kick (member uniquement) : membres qui n'ont PAS participé à la guerre
        oss += "Candidats kick 🚫 (member uniquement) :\n";
        std::vector<PlayerStats> candidates;
        for (const auto &p : stats) {
            if (p.role == "member" && (!p.didWar || p.warPoints == 0)) candidates.push_back(p);
        }
        // Trier par inactiveDays desc puis par trophées desc
        std::sort(candidates.begin(), candidates.end(), [](const PlayerStats &a, const PlayerStats &b){ if (a.inactiveDays != b.inactiveDays) return a.inactiveDays > b.inactiveDays; return a.trophies > b.trophies; });
        for (const auto &p : candidates) {
            oss += "- " + p.name + " — " + std::to_string(p.trophies) + " trophées — " + std::to_string(p.inactiveDays) + "j inactif — PAS de guerre\n";
        }

        oss += "\n";

        // Ajouter un résumé public rapide en bas
        oss += "Semaine - Rapport de Clan\n\n";
        auto v = top3ClanStrongestWithWar(stats);
        for (size_t i = 0; i < v.size(); ++i) {
            const auto &p = v[i];
            oss += std::to_string(i+1) + ". " + p.name + " — score " + std::to_string(p.score) + " | " + std::to_string(p.trophies) + " trophées | " + std::to_string(p.donations) + " dons | " + std::to_string(p.warPoints) + " pts guerre | actif il y a " + std::to_string(p.inactiveDays) + "j\n";
        }
        oss += "\n";

        // Top Guerre
        oss += "Top Guerre de Clan ⚔\n";
        auto byWar = top3ByWar(stats);
        for (size_t k = 0; k < byWar.size(); ++k) {
            oss += std::to_string(k+1) + ". " + byWar[k].name + " — " + std::to_string(byWar[k].value) + " pts guerre\n";
        }
        oss += "\n";

        // Top Donateurs
        oss += "Top Donateurs 🤝\n";
        auto byDon = top3ByDonations(stats);
        for (size_t k = 0; k < byDon.size(); ++k) {
            oss += std::to_string(k+1) + ". " + byDon[k].name + " — " + std::to_string(byDon[k].value) + " cartes données\n";
        }
        oss += "\n";

        oss += "Rappel ⚠\nFaites vos guerres, restez actifs, et go top ladder ❤️\n";

        return oss;
    } catch (...) {
        return std::string("Rapport interne (chefs)\n\n(Erreur de génération du rapport privé)\n");
    }
}
