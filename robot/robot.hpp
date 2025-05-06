#pragma once

#include "common.hpp"

#include <string>
#include <deque>

class Robot
{
public:
    using R = RobotState;

    Robot() = default;
    Robot(std::string id, Coord coords, int a_wait_time, int b_wait_time);

    Robot(const Robot& ) = delete;
    Robot& operator=(const Robot& ) = delete;

    Robot(Robot&& other) noexcept = default;
    Robot& operator=(Robot&& rhs) noexcept = default;
    
    std::string GetId() const;
    Coord GetCoords() const;
    int GetAWaitTime() const;
    int GetBWaitTime() const;
    std::string GetATask() const;
    std::string GetBTask() const;
    RobotState GetState() const;

    bool IsBusy() const;
    bool HasRoute() const;
    bool CompletedA () const;
    bool CompletedB () const;

    static std::string IdMaker(int id);

    void SetTask(Task t);
    void SetRoute(std::deque<Coord>&& route);

    Coord NextStep() const;

    void Move();
    void ChangeState(RobotState new_state);

private:
    Coord coords_;
    std::string id_ = "";
    int a_wait_time_ = 0;
    int b_wait_time_ = 0;
    bool a_complete_ = false;
    bool b_complete_ = false;
    RobotState state_ = R::RELAX;
    Task task_ = {};
    std::deque<Coord> route_ = {};
};