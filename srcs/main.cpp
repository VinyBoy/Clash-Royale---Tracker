#include "../includes/Clash.hpp"

int main(int argc, char **argv)
{
    std::string clanTag = "LQY80L9P";
    if (argc > 1) clanTag = argv[1];
    else {
        // lecture conseillée dans README.md ; on laisse vide -> générer des données factices
    }

    try {
        std::string raw = fetchClanMembers(clanTag);
        std::vector<PlayerStats> players;
        try {
            players = buildPlayerStats(raw);
        } catch (const std::exception &e) {
            std::cerr << "[ERROR] buildPlayerStats threw: " << e.what() << "\n";
            players.clear();
        } catch (...) {
            std::cerr << "[ERROR] buildPlayerStats threw unknown exception\n";
            players.clear();
        }

        // merger les stats de guerre si disponibles (stub)
        try {
            std::string rr = fetchCurrentRiverRace(clanTag);
            auto warMap = parseRiverRace(rr);
            mergeWarStats(players, warMap);
        } catch (const std::exception &e) {
            std::cerr << "[WARN] river race processing error: " << e.what() << "\n";
        } catch (...) {
            std::cerr << "[WARN] unknown error processing river race\n";
        }

        ensureOutputFolder();
        const std::string timestampFolder = getCurrentDateTimeStamp();
        const std::string baseFolder = "files/" + timestampFolder;
        std::filesystem::create_directories(baseFolder);

    	std::string pub;
    	std::string priv;
    try {
        pub = MakePublicReport(players);
    } catch (const std::bad_alloc &e) {
        std::cerr << "[ERROR] MakePublicReport bad_alloc: " << e.what() << "\n";
        pub = "Semaine - Rapport de Clan\n\n(Erreur de génération du rapport public)\n";
    } catch (const std::exception &e) {
        std::cerr << "[ERROR] MakePublicReport exception: " << e.what() << "\n";
        pub = "Semaine - Rapport de Clan\n\n(Erreur de génération du rapport public)\n";
    }
    try {
        priv = MakePrivateReport(players);
    } catch (const std::bad_alloc &e) {
        std::cerr << "[ERROR] MakePrivateReport bad_alloc: " << e.what() << "\n";
        priv = "Rapport interne (chefs)\n\n(Erreur de génération du rapport privé)\n";
    } catch (const std::exception &e) {
        std::cerr << "[ERROR] MakePrivateReport exception: " << e.what() << "\n";
        priv = "Rapport interne (chefs)\n\n(Erreur de génération du rapport privé)\n";
    }

        std::string date = getCurrentDate();
        std::string pubPath = baseFolder + "/rapport_public_" + date + ".txt";
        std::string privPath = baseFolder + "/rapport_prive_" + date + ".txt";

    
    std::ofstream ofs(pubPath);
    ofs << pub;
    ofs.close();

    std::ofstream ofs2(privPath);
    ofs2 << priv;
    ofs2.close();
    

        std::cout << "Rapports générés :\n  - " << pubPath << "\n  - " << privPath << std::endl;
        sendReportEmail(pubPath, privPath);
        // Afficher un résumé console des stats (top 3 trophées, top guerre, top donateurs)
        std::cout << "\nRésumé rapide :\n";
        // Top trophées - sélection O(n) sans copier tout le vecteur
        std::array<int,3> topIdx = {-1,-1,-1};
        for (size_t idx = 0; idx < players.size(); ++idx) {
            for (int pos = 0; pos < 3; ++pos) {
                if (topIdx[pos] == -1 || players[idx].trophies > players[topIdx[pos]].trophies) {
                    // shift down
                    for (int k = 2; k > pos; --k) topIdx[k] = topIdx[k-1];
                    topIdx[pos] = (int)idx;
                    break;
                }
            }
        }
        std::cout << "Top trophées :\n";
        for (int i = 0; i < 3; ++i) if (topIdx[i] != -1) std::cout << (i+1) << ". " << players[topIdx[i]].name << " — " << players[topIdx[i]].trophies << " trophées" << std::endl;

        // Top guerre
        std::array<int,3> topWar = {-1,-1,-1};
        for (size_t idx = 0; idx < players.size(); ++idx) {
            for (int pos = 0; pos < 3; ++pos) {
                if (topWar[pos] == -1 || players[idx].warPoints > players[topWar[pos]].warPoints) {
                    for (int k = 2; k > pos; --k) topWar[k] = topWar[k-1];
                    topWar[pos] = (int)idx;
                    break;
                }
            }
        }
        std::cout << "Top guerre :\n";
        for (int i = 0; i < 3; ++i) if (topWar[i] != -1) std::cout << (i+1) << ". " << players[topWar[i]].name << " — " << players[topWar[i]].warPoints << " pts guerre" << std::endl;

        // Top donateurs
        std::array<int,3> topDon = {-1,-1,-1};
        for (size_t idx = 0; idx < players.size(); ++idx) {
            for (int pos = 0; pos < 3; ++pos) {
                if (topDon[pos] == -1 || players[idx].donations > players[topDon[pos]].donations) {
                    for (int k = 2; k > pos; --k) topDon[k] = topDon[k-1];
                    topDon[pos] = (int)idx;
                    break;
                }
            }
        }
        std::cout << "Top donateurs :\n";
        for (int i = 0; i < 3; ++i) if (topDon[i] != -1) std::cout << (i+1) << ". " << players[topDon[i]].name << " — " << players[topDon[i]].donations << " dons" << std::endl;
        return 0;
    }
    catch (const std::exception &e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
        return 1;
    }
}
