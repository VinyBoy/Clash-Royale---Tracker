#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <chrono> 
#include <sstream> 

#include <curl/curl.h>
#include <nlohmann/json.hpp>
using nlohmann::json;

// ==========================
// Struct regroupée pour stats joueur
// ==========================
struct Member
{
	std::string tag;
	std::string name;
	std::string role;
	std::string lastSeenRaw;
	int         expLevel;
	int         trophies;
	int         clanRank;
	int         previousClanRank;
	int         donations;
	int         donationsReceived;
	std::string arenaName;
};

struct PlayerStats
{
    std::string tag;
    std::string name;
    std::string role;
    int trophies;
    int donations;
    int donationsReceived;
    int clanRank;
    int previousClanRank;
    std::string lastSeenRaw;
    int inactiveDays;
    int warPoints;    // ⚠ doit être rempli après appel /currentriverrace
    bool didWar;
    int apiOrder;  
    std::string arenaName;
    int clanChestPoints;
};

struct RankedEntry
{
    std::string name;
    int value;
};

struct RankGain
{
    std::string name;
    int gain;
    int prevRank;
    int curRank;
};

struct RatInfo
{
    std::string name;
    int donations;
    int received;
    int ratScore;
    bool found;
};

struct KickMember
{
    std::string name;
    int inactiveDays;
    std::string role;
    int trophies;
    bool didWar;
    int warPoints;
};

struct ClanRankEntry
{
    std::string name;
    int clanRank;
    int trophies;
};

struct TopClanEntry
{
    std::string name;
    int trophies;
    int donations;
    int warPoints;
    int inactiveDays;
    int score;
};

struct WarParticipant
{
    std::string tag;
    std::string name;
    int warPoints;
    int attacks;
};



size_t 				writeCallBack(void *contents, size_t size, size_t nmemb, void *userp);
std::string 			fetchClanMembers(const std::string & clanTag);
std::vector<PlayerStats> 	buildPlayerStats(const std::string &jsonStr);
int 				computeInactiveDays(const std::string &isoUtc);
std::vector<RankedEntry> 	top3ByDonations(const std::vector<PlayerStats>& v);
RankGain 					bestRankGain(const std::vector<PlayerStats> &v);
RatInfo 					findRat(const std::vector<PlayerStats> &v);
std::string 				MakePublicReport(const std::vector<PlayerStats> &stats);
std::string 				MakePrivateReport(const std::vector<PlayerStats> & stats);
/*	War.cpp*/
std::vector<TopClanEntry> 	top3ClanStrongestWithWar(const std::vector<PlayerStats> &players);
std::string 				fetchCurrentRiverRace(const std::string &clanTag);
std::unordered_map<std::string, WarParticipant>	parseRiverRace(const std::string &jsonStr);
void						mergeWarStats(std::vector<PlayerStats> &players, const std::unordered_map<std::string, WarParticipant> &warMap);
std::vector<RankedEntry>	top3ByWar(const std::vector<PlayerStats>& v);
std::vector<KickMember> 	findKickList(const std::vector<PlayerStats> &v);
void 						ensureOutputFolder();
std::string 				getCurrentDate();
