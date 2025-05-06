#include "navigator.hpp"

#include <utility>
#include <ostream>
#include <queue>
#include <cmath>
#include <algorithm>

using namespace std;

Navigator::Navigator(Size size, ActiveCellSettings&& sett)
:field_(size)
{
    field_.InitActiveCells(move(sett.coords));
}

Navigator::Navigator(Navigator&& other)
:field_(move(other.field_))
{}

Navigator& Navigator::operator=(Navigator&& rhs)
{
    if(this != &rhs)
    {
        swap(field_, rhs.field_);
    }
    return *this;
}

void Navigator::InitRobots(vector<string> ids, vector<Coord> coords)
{
    for(size_t i = 0; i < ids.size(); ++i){
        field_.InitRobotCell(ids.at(i), coords.at(i));
    }
}

void Navigator::PrintState(std::ostream& out) const { field_.Print(out); }

//BFS
Navigator::robot_id Navigator::LookForRobot(std::string task, unordered_set<string> free_rob_ids)
{
    Coord c = field_.GetActiveCellCoords(task);

    unordered_set<Coord> used_cells {c};
    deque<Coord> q;
    q.push_back(c);


    while(!q.empty())
    {
        auto coord = q.front();
        q.pop_front();

        if(field_.CheckCellIsBusy(coord))
        {
            auto id =  field_.GetRobotId(coord);
            if(free_rob_ids.contains(id))
            {
                return id;
            }
        }

        for(const auto& [dx, dy] : field_.potential_neightbours)
        {
            Coord neightbour(coord.x + dx, coord.y + dy);
            if(!used_cells.contains(neightbour))
            {

                if(field_.CheckCellsConnected(coord, neightbour))
                {
                    q.push_back(neightbour);
                    used_cells.emplace(neightbour);
                }
            }
        }
    }
    return "";
}

// BFS
deque<Coord> Navigator::BuildRoute(Coord from, string goal)
{
    Coord to = field_.GetActiveCellCoords(goal);
    if(from == to) return {};

    queue<Coord> q;
    q.push(from);
    
    int root = -1;
    unordered_map<Coord, Coord> routes;
    routes[from] = Coord{root, root};

    int no_progress = 0;
    while(!q.empty())
    {
        auto coord = q.front();
        q.pop();
        if(coord == to) break;

        for(const auto& [dx, dy] : field_.potential_neightbours)
        {
            Coord neightbour(coord.x + dx, coord.y + dy);
            bool smth_added = true;
            if(field_.CheckCellsConnected(coord, neightbour))
            {
                if(!field_.CheckCellIsBusy(neightbour))
                {
                    if(!routes.contains(neightbour))
                    {
                        smth_added = false;
                        routes[neightbour] = coord;
                        q.push(neightbour);
                    }    
                }
                else
                {
                    if(neightbour == to)
                    {
                        routes[neightbour] = coord;
                        q.push(neightbour);
                        break;
                    }
                }
            }
            no_progress += smth_added;
            auto [x, y] = field_.GetSize();
            if(no_progress > pow(x*y, 1.5))
            {
                throw runtime_error("Can't build any route");
            }
        }
    }

    deque<Coord> res_route;
    for(auto coord = to; coord != from; coord = routes[coord])
    {
        res_route.push_back(coord);
    }
    reverse(res_route.begin(), res_route.end());
    return res_route;
}

bool Navigator::IsRobotInAGoal(std::string a_cell_id, Coord cur) const
{
    return field_.GetActiveCellCoords(a_cell_id) == cur;
}

bool Navigator::IsRobotInBGoal(std::string b_cell_id, Coord cur) const
{
    return field_.GetActiveCellCoords(b_cell_id) == cur;
}

bool Navigator::IsRobotMoveCorrect(Coord next_move) const
{
    return !field_.CheckCellIsBusy(next_move);
}

void Navigator::UpdateRobotLocation(string id, Coord new_coord)
{
    field_.InitRobotCell(id, new_coord);
}

deque<Coord> Navigator::FreeUpActiveCell(std::string cell)
{
    Coord coord = field_.GetActiveCellCoords(cell);

    auto cell_search = [this](Coord c)
    {
        deque<Coord> route;
        for(const auto& [dx, dy] : field_.potential_neightbours)
        {
            auto near = Coord{c.x + dx, c.y + dy};
            if(field_.CheckCellsConnected(c, near)) 
            {
                if(!field_.CheckCellIsBusy(near))
                {
                    route.push_back(near);
                }
            }
        }
        return route;
    };

    auto new_cell = MoveToCenter(coord); 
    deque<Coord> route;
    if(new_cell != coord) route.push_back(new_cell); 
    while(field_.CheckCellCanBeTask(new_cell))
    {
        coord = new_cell;
        Coord rand_cell(-1, -1);
        for(auto near : cell_search(coord))
        {
            rand_cell = near;
            new_cell = MoveToCenter(near);
            if(new_cell != near)
            {
                route.push_back(near);
                route.push_back(new_cell);
                break;
            }
        }
        if(rand_cell == Coord{-1, -1})
        {
            throw runtime_error("Robot is blocked by others");
        }
    }
    return route;
}

Coord Navigator::MoveToCenter(Coord rob)
{
    auto [x, y] = field_.GetSize();
    Coord center(x/2, y/2);
    Coord best_step = rob;
    int cur = abs(rob.x - center.x) + abs(rob.y - center.y);
    for(const auto& [dx, dy] : field_.potential_neightbours)
    {
        auto near = Coord{rob.x + dx, rob.y + dy};
        if(field_.CheckCellsConnected(rob, near))
        {
            if(!field_.CheckCellCanBeTask(near) && !field_.CheckCellIsBusy(near))
            {
                int next_step = abs(rob.x - near.x) + abs(rob.y - near.y);
                if(next_step < cur) best_step = near;
            }
        }
    }
    return best_step;
}