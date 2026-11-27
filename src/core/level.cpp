#include "level.h"
#include "util.h"
#include "physics.h"
#include <fstream> //read file
using namespace std;

// Modular implementation using primitive arrays for map parsing and verification...

// Load building map and error if file missing or less than 5 floor in file
bool loadBuildingMap(const char* filename, char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR - 1], char roomEntities[NUM_FLOORS][ROOMS_PER_FLOOR][MAX_ENTITIES_PER_ROOM], int entityCounts[NUM_FLOORS][ROOMS_PER_FLOOR]){
    ifstream file(filename);
    if (!file) return false;

    for (int f = 0; f < NUM_FLOORS; f++){
        for (int r = 0; r < ROOMS_PER_FLOOR - 1; r++){
            entityCounts[f][r] = 0;
        }
        for (int c = 0; c < ROOMS_PER_FLOOR - 1; c++){
            horizontalConn[f][c] = CONN_NONE;
        }
    }
    char *topLine = new char[MAX_LINE_LEN];
    char *itemLine = new char[MAX_LINE_LEN];
    int  floorsFound = 0;

    while (file.getline(topLine, MAX_LINE_LEN)){
        int length = 0;
        while (topLine[length] != '\0') length += 1;

        int tagPos[ROOMS_PER_FLOOR];
        int tags = 0;
        for (int i = 0; i+4 < length && tags < ROOMS_PER_FLOOR; i++){
            if (topLine[i] == 'F' && topLine[i+4] >= '1' && topLine[i+4] <= '9' && topLine[i+2] == '-' && topLine[i+3] == 'R' && topLine[i+4] >= '1' && topLine[i+4] <= '9'){
                tagPos[tags] = i;
                tags += 1;
            }   
        }
        if (tags != ROOMS_PER_FLOOR) continue;

        int floorNum = topLine[tagPos[0]+1] - '0';
        if (floorNum < 1 || floorNum > NUM_FLOORS) continue;

        for (int c = 0; c < ROOMS_PER_FLOOR - 1; c++){
            char symbol = CONN_NONE;
            
            for (int i = tagPos[c]+5; i < tagPos[c+1]; i++){
                if (topLine[i] != ' '){
                    symbol = topLine[i];
                    if (symbol == 'L' && topLine[i+1] == '1') symbol = '1';
                break;       }
            }
            horizontalConn[floorNum-1][c] = symbol;
        }

        if (!file.getline(itemLine, MAX_LINE_LEN)) break;

        int itemLength = 0;
        while (itemLine[itemLength] != '\0') itemLength += 1;

        int roomNum = 1;
        for (int i = tagPos[0] - 1; i < itemLength && roomNum <= ROOMS_PER_FLOOR; i++){
            if (itemLine[i] == '|'){
                roomNum += 1;
                }
            else if (itemLine[i] != ' ' && itemLine[i] != '\r'){
                int count = entityCounts[floorNum-1][roomNum-1];
                if (count < MAX_ENTITIES_PER_ROOM){
                    roomEntities[floorNum-1][roomNum-1][count] = itemLine[i];
                    entityCounts[floorNum-1][roomNum-1] += 1;
                }
            }
        }

        floorsFound += 1;
    }
    file.close();

    delete[] topLine;
    delete[] itemLine;
    return floorsFound == NUM_FLOORS;

}

//reads file and updates energy, time and score and reset floor and room to 0 at start of new game
bool loadInitialState(const char* filename, int& playerFloor, int& playerRoom, int& energy, int& time, int& score, char inventory[MAX_INVENTORY_CAPACITY], int& inventoryCount, bool objectives[NUM_OBJECTIVES], int& emergencyTripsLeft){
    ifstream file(filename);
    if (!file) return false;

    char word[MAX_NAME_LEN];
    while(file >> word){
        if (stringEquals(word, "INITIAL_ENERGY:")) file >> energy;
        else if (stringEquals(word, "INITIAL_TIME:")) file >> time;
        else if (stringEquals(word, "INITIAL_SCORE:")) file >> score;
        
    }
    
    file.close();
    playerFloor = 0;
    playerRoom = 0;
    inventoryCount = 0;
    emergencyTripsLeft = 0;
    
    for (int i = 0; i < MAX_INVENTORY_CAPACITY; i++) inventory[i] = ENT_NONE;

    for (int i = 0; i < NUM_OBJECTIVES; i++) objectives[i] = false;
}

// validation of starting room having atleast one exit
bool validateStartingRoom(int startFloor, int startRoom, const char horizontalConn[NUM_FLOORS][ROOMS_PER_FLOOR-1]){
    if(startFloor < 1 || startFloor > NUM_FLOORS || startRoom < 1 || startRoom > ROOMS_PER_FLOOR) return false;
    for(int f=1; f<=NUM_FLOORS; f++){
        for(int r=1; r<=ROOMS_PER_FLOOR; r++){
            char link;
            if(isDirectlyConnected(startFloor, startFloor, f, r, horizontalConn, link) && link != CONN_SOLID_WALL) return true;
        }
    }
    return false;
}

// getting the readable name of an item's symbol
const char* getEntityName(char entitySymbol) {
     switch(entitySymbol){
        case ENT_MASTER_KEY : return "Master Key";
        case ENT_ENERGY_PACK : return "Energy Pack";
        case ENT_EMERGENCY_TOOLKIT : return "Emergency Toolkit";
        case ENT_CLUE_4 : return "Clue 4";
        case ENT_FUSE : return "Fuse";
        case ENT_CAT : return "Cat";
        case ENT_CONTROL_PANEL : return "Control Panel";
        case ENT_LIFT_PASS : return "Lift Pass";
        case ENT_PASSENGER : return "Injured Passenger";
        case ENT_MEDICAL_KIT : return "Medical Kit";
        case ENT_EMERGENCY_CARD : return "Emergency Access Card";
        case ENT_CLUE_2 : return "Clue Digit 2";
        case ENT_CLUE_7 : return "Clue Digit 7";
        case ENT_CABINET_TRAIN_KEY : return "Cabinet/Train Key";
        case ENT_CAT_CARRIER : return "Cat Carrier";
        case ENT_OBSERVATION_TOKEN : return "Observation Token";
        default : return "Unknown";
     }
}

