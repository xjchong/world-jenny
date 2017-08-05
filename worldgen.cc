
#include <iostream>
#include <cmath>
#include <algorithm>
#include "worldgen.h"
using namespace std;


int WorldGen::roll(int lo, int hi){
    uniform_int_distribution<int> dice(lo, hi);
    return dice(mt);
}


int WorldGen::getOdd(int num){
    return (num/2)*2+1;
}


Cell &WorldGen::getCell(int row, int col){
    return grid[row*cols + col];
}


bool WorldGen::cellExists(int row, int col){
    if (row < 0 || row > rows-1) return false;
    if (col < 0 || col > cols-1) return false;
    return true;
}


vector<Cell> WorldGen::adjCells(Cell &c){
    vector<Cell> adj;
    if (c.row+1 < rows) adj.emplace_back(getCell(c.row+1, c.col));
    if (c.col+1 < cols) adj.emplace_back(getCell(c.row, c.col+1));
    if (c.row-1 > 0) adj.emplace_back(getCell(c.row-1, c.col));
    if (c.col-1 > 0) adj.emplace_back(getCell(c.row, c.col-1));
    return adj;
}


bool WorldGen::roomSafe(Cell &c, int w, int h){
    if (!cellExists(c.row+h, c.col+w)) return false;
    for (int y=c.row; y<c.row+h; ++y){
        for (int x=c.col; x<c.col+w; ++x){
            if (getCell(y, x).rep != wall) return false;
        }
    }
    return true;
}


void WorldGen::buildRoom(Cell &c, int w, int h){
    for (int y=c.row; y<c.row+h; ++y){
        for (int x=c.col; x<c.col+w; ++x){
            getCell(y, x).rep = floor;
            getCell(y, x).id = ids;
        }
    }
}


void WorldGen::placeRooms(int tries, int w_mean, int h_mean, int stdev){
    normal_distribution<> room_dist(0, stdev);
    for (int i=0; i<tries; ++i){
        int row = getOdd(roll(1, rows-2));
        int col = getOdd(roll(1, cols-2));
        int w = getOdd(w_mean+round(room_dist(mt)));
        int h = getOdd(h_mean+round(room_dist(mt)));
        if (roomSafe(getCell(row, col), w, h)){
            buildRoom(getCell(row, col), w, h);
            ids++;
        }
    }
}

void WorldGen::breakWalls(Cell &c){
    if (cellExists(c.row+2, c.col) && getCell(c.row+2, c.col).id == c.id){
        getCell(c.row+1, c.col).rep = floor;
        getCell(c.row+1, c.col).id = c.id;
    }
    if (cellExists(c.row-2, c.col) && getCell(c.row-2, c.col).id == c.id){
        Cell &target = getCell(c.row-1, c.col);
        target.rep = floor;
        target.id = c.id;
    }
    if (cellExists(c.row, c.col+2) && getCell(c.row, c.col+2).id == c.id){
        Cell &target = getCell(c.row, c.col+1);
        target.rep = floor;
        target.id = c.id;
    }
    if (cellExists(c.row, c.col-2) && getCell(c.row, c.col-2).id == c.id){
        Cell &target = getCell(c.row, c.col-1);
        target.rep = floor;
        target.id = c.id;
    }
}


bool WorldGen::corridorSafe(Cell &c, int dir){
    int row = c.row;
    int col = c.col;
    switch(dir){
        case 1: row+=2; break;
        case 2: col-=2; break;
        case 3: col+=2; break;
        case 4: row-=2; break;
        default: break;
    }
    return (cellExists(row, col) && getCell(row, col).rep == wall);
}


void WorldGen::buildCorridors(Cell &c, int spaciness){
    vector<int> dirs = {1, 2, 3, 4};
    random_shuffle(dirs.begin(), dirs.end());
    getCell(c.row, c.col).rep = floor;
    for (auto dir : dirs){
        if (corridorSafe(getCell(c.row, c.col), dir)){
            int i = 0;
            int row = c.row;
            int col = c.col;
            while(i++<2){
                switch(dir){
                    case 1: row+=1; break; 
                    case 2: col-=1; break;
                    case 3: col+=1; break;
                    case 4: row-=1; break;
                    default: break;
                }
                getCell(row, col).rep = floor;
                getCell(row, col).id = ids;
                if (spaciness>roll()) breakWalls(getCell(row, col));
            }
            buildCorridors(getCell(row, col), spaciness);
        }
    }
}


void WorldGen::placeCorridors(int spaciness){
    for (int row=1; row<rows; row+=2){
        for (int col=1; col<cols; col+=2){
            if (getCell(row, col).rep == wall){
                buildCorridors(getCell(row, col), spaciness);
                getCell(row, col).id = ids++;
            }
        }
    }
}


bool WorldGen::isDeadEnd(Cell &c){
    int walls = 0;
    if (getCell(c.row+1, c.col).rep == wall || c.row+1==rows-1) ++walls;
    if (getCell(c.row-1, c.col).rep == wall || c.row-1==0) ++walls;
    if (getCell(c.row, c.col+1).rep == wall || c.col+1==cols-1) ++walls;
    if (getCell(c.row, c.col-1).rep == wall || c.col-1==0) ++walls;
    return (walls>=3);
}


void WorldGen::removeDeadEnd(Cell &c){
    if (isDeadEnd(c)){
        c.rep = wall;
        vector<Cell> adj = adjCells(c);
        for (auto cell : adj){
            if (cell.rep != wall) removeDeadEnd(getCell(cell.row, cell.col));   
        }
    }
}


void WorldGen::removeDeadEnds(){
    for (int row=1; row<rows-1; ++row){
        for (int col=1; col<cols-1; ++col){
            if (getCell(row, col).rep != floor) continue;
            if (isDeadEnd(getCell(row, col))){
                removeDeadEnd(getCell(row, col));
            }
        }
    }
}


bool WorldGen::isConnector(Cell &c){
    vector<Cell> adj = adjCells(c);
    vector<int> adj_id;
    for (auto cell : adj){
        if (cell.id != 0) adj_id.emplace_back(cell.id);
    }
    if (adj_id.size() != 2) return false;
    else if (adj_id[0]-adj_id[1] == 0) return false;
    else return true;
}


int WorldGen::countMergedIDs(Connector &ktor){
    int sum = 0;
    if (merged.find(ktor.id[0]) != merged.end()) ++sum;
    if (merged.find(ktor.id[1]) != merged.end()) ++sum;
    return sum;
}


void WorldGen::buildConnectors(vector<Connector> &ktors, int chance, int max_extra){
    for (auto ktor : ktors){
        if (countMergedIDs(ktor)==1){
            for (int i=0; i<=1; ++i) merged.insert(ktor.id[i]);
            getCell(ktor.row, ktor.col).rep = door;
            break;
        }
    }
    ktors.erase(remove_if(ktors.begin(), ktors.end(), [&](Connector ktor){
        int m = countMergedIDs(ktor);
        if (chance>roll() && m==2 && max_extra>0){
            --max_extra;
            getCell(ktor.row, ktor.col).rep = door;
        }
        return m==2;
    }), ktors.end());
}


void WorldGen::placeConnectors(int chance, int max_extra){
    vector<Connector> ktors;
    for (int row=1; row<rows-1; ++row){
        for (int col=1; col<cols-1; ++col){
            if (getCell(row, col).rep == wall && isConnector(getCell(row, col))){
                ktors.emplace_back(Connector{*this, row, col});
            }
        }
    }
    random_shuffle(ktors.begin(), ktors.end());
    merged.insert(ktors[0].id[0]);
    while(!ktors.empty()) buildConnectors(ktors, chance, max_extra);
}


bool WorldGen::isPillar(Cell &c){
    vector<Cell> adj = adjCells(c);
    if (adj.size() < 4) return false;
    int walls = 0;
    for (auto cell : adj) if (cell.rep != floor) ++walls;
    return (walls == 0);
}


void WorldGen::removePillars(int chance){
    for (int row=1; row<rows-1; ++row){
        for (int col=1; col<cols-1; ++col){
            if (isPillar(getCell(row, col)) && chance>roll()){
                getCell(row, col).rep = floor;
            }
        }
    }
}

WorldGen::WorldGen(const int rows, const int cols)
    : rows{rows}, cols{cols}, ids{1}
    , wall{'#'}, floor{'.'}, door{'+'}
    , mt{mt19937{random_device{}()}}
    {}


std::string WorldGen::generate(
    int room_density, int room_w, int room_h, 
    int room_stdev, int spacy, int doorsy,
    int max_doors, int pillars){
    merged.clear();
    grid.resize(rows*cols);
    for (int row=0; row<rows; ++row){
        for (int col=0; col<cols; ++col){
            grid[row*cols + col] = Cell{row, col};
        }
    }
    placeRooms(room_density, room_w, room_h, room_stdev);
    placeCorridors(spacy);
    placeConnectors(doorsy, max_doors);
    removeDeadEnds();
    removePillars(100-pillars);
    string world = "";
    for (int row=0; row<rows; ++row){
        for (int col=0; col<cols; ++col){
            world += getCell(row, col).rep;
        }
        world+='\n';
    }
    cout << world << endl;
    return world;
}
