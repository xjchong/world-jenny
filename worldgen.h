#include <vector>
#include <set>
#include <string>
#include <random>

struct Cell{
        char rep = '#';
        int row, col, id;
        Cell(int row=0, int col=0, int id=0)
            : row{row}, col{col}, id{id}
        {}
};


class WorldGen{


    int roll(int lo=0, int hi=99);
    int getOdd(int num);
    Cell &getCell(int row, int col);
    bool cellExists(int row, int col);
    std::vector<Cell> adjCells(Cell &c);
    
    struct Connector{
        int row, col;
        int id[2];
        Connector(WorldGen &gen, int row, int col)
            : row{row}, col{col}
        {
            int i=0;
            std::vector<Cell> adj = gen.adjCells(gen.getCell(row, col));
            for (auto cell : adj) if (cell.id) id[i++] = cell.id;
        }
    };

    const int rows, cols; 
    int ids;
    const char wall, floor, door;
    std::mt19937 mt;
    std::vector<Cell> grid;
    std::set<int> merged;

    bool roomSafe(Cell &c, int w, int h);
    void buildRoom(Cell &c, int w, int h);
    void placeRooms(int tries=750, int w_mean=11, int h_mean=6, int stdev=2);

    void breakWalls(Cell &c);
    bool corridorSafe(Cell &c, int dir);
    void buildCorridors(Cell &c, int spaciness=10);
    void placeCorridors(int spaciness=10);

    bool isDeadEnd(Cell &c);
    void removeDeadEnd(Cell &c);
    void removeDeadEnds();

    bool isConnector(Cell &c);
    int countMergedIDs(Connector &ktor);
    void buildConnectors(std::vector<Connector> &ktors, int chance=4, int max_extra=2);
    void placeConnectors(int doorsy=4, int max_extra=2);

    bool isPillar(Cell &c);
    void removePillars(int chance=100);

public:

    WorldGen(const int rows=21, const int cols=79);

    std::string generate(
        int room_density=250, int room_w=11, int room_h=6, 
        int room_stdev=2, int spacy=10, int doorsy=4,
        int max_extra=2, int pillars=0);

    friend struct Connector;
};




