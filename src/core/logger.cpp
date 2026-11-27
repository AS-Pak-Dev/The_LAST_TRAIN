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