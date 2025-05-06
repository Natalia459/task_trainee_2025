#include "field.hpp"
#include "logging.hpp"

#include <string>
#include <unordered_map>
#include <ostream>

using namespace std;

Field::Field(Size size)
:size_(size)
{
    LOG_MESS("start init field");
    InitField(size);
}

Field::Field(Field&& other)
: list_(move(other.list_)),
    active_cells_(move(other.active_cells_)),
    busy_cells_(move(other.busy_cells_)),
    size_(other.size_)
{}

Field& Field::operator=(Field&& rhs)
{
    if(this != &rhs){
        swap(list_, rhs.list_);
        swap(active_cells_, rhs.active_cells_);
        swap(busy_cells_, rhs.busy_cells_);
        swap(size_, rhs.size_);
    }
    return *this;
}

void Field::InitActiveCells(unordered_map<string, Coord>&& cells)
{
    for(auto [id, coords] : cells)
    {
        list_[coords].SetTaskId(id);
        active_cells_[id] = coords;
    }
}

void Field::InitField(Size size)
{
    for(int x = 0; x < size.length; ++x)
    {
        for(int y = 0; y < size.width; ++y)
        {
            list_[{x,y}] = Cell{x, y};
        }
    }
}

void Field::InitRobotCell(string id, Coord coords)
{
    if(busy_cells_.contains(id)){
        auto old_coords = busy_cells_[id];
        // LOG_MESS("====== " + id + " was " + to_string(old_coords.x) + ", " + to_string(old_coords.y));
        list_[old_coords].MakeFree();
        list_[old_coords].SetRobotId("");
    }
    
    // LOG_MESS("====== " + id + " become " + to_string(coords.x) + ", " + to_string(coords.y));
    busy_cells_[id] = coords;
    list_[coords].MakeBusy();
    list_[coords].SetRobotId(id);
}

bool Field::CheckCellsConnected(Coord a, Coord b) const
{
    if (a.x < 0 || a.x >= size_.length || a.y < 0 || a.y >= size_.width) return false;
    if (b.x < 0 || b.x >= size_.length || b.y < 0 || b.y >= size_.width) return false;

    int dx = abs(a.x - b.x);
    int dy = abs(a.y - b.y);
    return (dx == 1 && dy == 0) || (dx == 0 && dy == 1);
}

bool Field::CheckCellIsBusy(Coord c) const
{
    return list_.at(c).IsBusy();
}

bool Field::CheckCellCanBeTask(Coord c) const
{
    return list_.at(c).IsActiveCell();
}

void Field::Print(std::ostream& out) const
{
    out << "\n";
    for (int x = 0; x < size_.length; ++x) {
        for (int y = 0; y < size_.width; ++y) {
            Coord c(x, y);
            if(list_.at(c).IsBusy())
            {
                out << " " + list_.at(c).GetRobotId() + " ";
                // out << "   " + list_.at(c).GetRobotId() + "   ";
            }
            else if(list_.at(c).IsActiveCell())
            {
                string task = " " + list_.at(c).GetTaskId() + "  ";
                if(task.size() == 5)
                {
                    task.erase(4);
                }
                out << task;
                // out << "   T" + list_.at(c).GetTaskId() + "   ";
            }
            else{
                // out << " (" + to_string(x) + ", " + to_string(y) + ") ";
                out << " .. ";
            }
        }
        out << "\n"s;
    }
}

Coord Field::GetActiveCellCoords(std::string task_id) const 
{ 
    return active_cells_.at(task_id); 
}

Size Field::GetSize() const { return size_; }

std::string Field::GetRobotId(Coord c) const 
{
     return list_.at(c).GetRobotId(); 
}