#include "cell.hpp"

#include <string>

using namespace std;

Cell::Cell(int x, int y)
: coords_(x, y)
{}

void Cell::SetTaskId(std::string id) { task_id_ = id; }

void Cell::SetRobotId(std::string id) { robot_id_ = id; }

void Cell::MakeBusy() { is_busy_ = true; }

void Cell::MakeFree() { is_busy_ = false; }

Coord Cell::GetCoords() const { return coords_; }

string Cell::GetTaskId() const { return task_id_; }

string Cell::GetRobotId() const { return robot_id_; }

bool Cell::IsBusy() const { return is_busy_; }

bool Cell::IsActiveCell() const { return !task_id_.empty(); }

