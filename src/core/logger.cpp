#include "logger.h"
#include "util.h"
#include <fstream>
using namespace std;

// File persistence for savegame.txt and game_summary.txt using primitive variables

bool saveGameStateToFile(const char* filename, int playerFloor, int playerRoom, int energy, int time, int score, const char inventory[MAX_INVENTORY_CAPACITY], int inventoryCount, const bool objectives[NUM_OBJECTIVES], int emergencyTripsLeft, const char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], const int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]) {
    ofstream file(filename);
    if(!file) return false;

    file << "FLOOR " << playerFloor << endl;
    file << "ROOM " << playerRoom << endl;
    file << "ENERGY " << energy << endl;
    file << "TIME " << time << endl;
    file << "SCORE " << score << endl;
    file << "TRIPS " << emergencyTripsLeft << endl;

    file << "INVENTORY " << inventoryCount;
    for (int i=0; i<inventoryCount; i++) file << " " << inventory[i];
    file << endl;

    file << "OBJECTIVES";
    for(int i=0; i<NUM_OBJECTIVES; i++) file << " " << objectives[i];
    file << endl;

    for(int f=0; f<NUM_FLOORS; f++){
        for(int r=0; r<ROOMS_PER_FLOOR; r++){
            file << "F" << f+1 << "-R" << r+1 << " " << entityCounts[f][r];
            for(int i=0; i<entityCounts[f][r]; i++) file << " " << roomEntities[f][r][i];
            file << "\n";
        }
    }
    file.close();
    return true;
}

// loading game state from file
bool loadGameStateFromFile(const char* filename, int& playerFloor, int& playerRoom, int& energy, int& time, int& score, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft, char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
    ifstream file(filename);
    if (!file) return false;

    char label[MAX_NAME_LEN];
    file >> label >> playerFloor;
    file >> label >> playerRoom;
    file >> label >> energy;
    file >> label >> time;
    file >> label >> score;
    file >> label >> emergencyTripsLeft;

    file >> label >> inventoryCount;
    if(inventoryCount<0 || inventoryCount > MAX_INVENTORY_CAPACITY) return false;
    for(int i=0; i<inventoryCount; i++) file >> inventory[i];

    file >> label;
    for(int i=0; i<NUM_OBJECTIVES; i++) file >> objectives[i];

    for(int f=0; f<NUM_FLOORS; f++){
        for(int r=0; r<ROOMS_PER_FLOOR; r++){
            file >> label >> entityCounts[f][r];
            if (entityCounts[f][r] < 0 || entityCounts[f][r] > MAX_ENTITIES_PER_ROOM) return false;
            for(int i=0; i<entityCounts[f][r]; i++) file >> roomEntities[f][r][i];
        }
    }
    
    if (file.fail()) return false;
    file.close();
    return true;
}

// displaying the final results
void writeGameSummaryReport(const char* filename, int score, int remainingEnergy, int remainingTime, const bool objectives[NUM_OBJECTIVES], bool victory){
    ofstream file(filename);
    if(!file) return;

    const char* objectiveNames[NUM_OBJECTIVES-2] = {
        "Master Key collected", "Clue digit 4 found", "Clue digit 2 found", "Clue digit 7 found", "Cabinet unlocked", "Train Key collected", "Fuse collected", "Fuse installed", "Lift Pass collected", "Lift 2 active", "Pasenger rescued", "Cat Carrier collected", "Cat secured", "Train prepared"
    };

    file << "=====  THE LAST TRAIN - GAME SUMMARY  ===== \n";
    if(victory) file << "RESULT: VICTORY\n";
    else file << "RESULT: DEFEAT\n";
    file << "FINAL SCORE: " << score << "/520\n";
    file << "ENERGY LEFT: " << remainingEnergy << "\n";
    file << "TIME LEFT: " << remainingTime << "\n";

    file << "\nOBJECTIVES:\n";
    for (int i=0; i<NUM_OBJECTIVES-2; i++) {
        if(objectives[i]) file << "[X] ";
        else file << "[ ] ";
        file << objectiveNames[i] << "\n";
    }
    file.close();
}