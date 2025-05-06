#include "robot.hpp"

#include <string>

using namespace std;

Robot::Robot(string id, Coord coords, int a, int b)
: coords_(coords), id_(id), a_wait_time_(a), b_wait_time_(b)
{}

std::string Robot::GetId() const { return id_; }

Coord Robot::GetCoords() const { return coords_; }

int Robot::GetAWaitTime() const { return a_wait_time_; }

int Robot::GetBWaitTime() const { return b_wait_time_; }

std::string Robot::GetATask() const { return task_.cell_id_from; }
std::string Robot::GetBTask() const { return task_.cell_id_to; }

bool Robot::IsBusy() const { return state_ != R::RELAX; }

bool Robot::HasRoute() const {return !route_.empty(); }

bool Robot::CompletedA () const { return a_complete_; }

bool Robot::CompletedB () const { return b_complete_; }

RobotState Robot::GetState() const { return state_; }

string Robot::IdMaker(int id) { return "R" + to_string(id); }

void Robot::SetTask(Task task) { task_ = task; }

void Robot::SetRoute(std::deque<Coord>&& route) {route_ = move(route); }

Coord Robot::NextStep() const { return route_.front(); }

void Robot::Move() 
{
    coords_ = route_.front();
    route_.pop_front(); 
}

void Robot::ChangeState(RobotState new_state) 
{ 
    if(new_state == RobotState::WORK && state_ == RobotState::A_MOVE)
    {
        a_complete_ = true;
    }
    if(new_state == RobotState::WORK && state_ == RobotState::RELAX)
    {
        a_complete_ = true;
    }
    else if(new_state == RobotState::WORK && state_ == RobotState::B_MOVE)
    {
        b_complete_ = true;
    }
    else if(new_state == RobotState::RELAX && state_ == RobotState::AWAY_MOVE)
    {
        a_complete_ = false;
        b_complete_ = false;
    }
    state_ = new_state; 
}