#include "field.hpp"
#include "common.hpp"
#include "logging.hpp"
#include "robot.hpp"

#include <vector>
#include <string>
#include <ostream>
#include <unordered_map>
#include <unordered_set>
#include <deque>

class Navigator
{
public:
    using robot_id = std::string;

    Navigator() = default;
    Navigator(Size size, ActiveCellSettings&& sett);

    Navigator(const Navigator& ) = delete;
    Navigator& operator=(const Navigator& ) = delete;

    Navigator(Navigator&& other);
    Navigator& operator=(Navigator&& rhs);

    void InitRobots(std::vector<std::string> ids, std::vector<Coord> coords);

    void PrintState(std::ostream& out) const ;

    robot_id LookForRobot(std::string task, std::unordered_set<std::string> free_rob_ids);

    std::deque<Coord> BuildRoute(Coord from, std::string goal);
    std::deque<Coord> FreeUpActiveCell(std::string cell);

    bool IsRobotInAGoal(std::string a_cell_id, Coord cur) const;
    bool IsRobotInBGoal(std::string b_cell_id, Coord cur) const;

    bool IsRobotMoveCorrect(Coord next_move) const;

    void UpdateRobotLocation(std::string id, Coord new_coord);

    Coord MoveToCenter(Coord rob);

private:
    Field field_;
    std::string mess = "";
};