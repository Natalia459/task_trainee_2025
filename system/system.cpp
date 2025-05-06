#include "system.hpp"

#include <fstream>
#include <memory>
#include <utility>

using namespace std;

void System::InitNavigator(Size size, ActiveCellSettings&& setts)
{
    LOG_MESS_STREAM("starting initializing navigator of system", log_.stream());
    nav_ = move(Navigator{size, move(setts)});
 
    LOG_EVENT_STREAM("navigator is initialised", log_.stream());
}

void System::InitRobots(RobotSettings&& setts)
{
    LOG_MESS_STREAM("starting initializing robots of system", log_.stream());
    vector<string> ids;
    ids.reserve(setts.amount);
    free_robots_.reserve(setts.amount);
    busy_robots_.reserve(setts.amount);
    for(int i = 0; i < setts.amount; ++i)
    {
        string id = Robot::IdMaker(i+1);
        free_robots_[id] = Robot{id, setts.coords[i], setts.a_wait_time[i], setts.b_wait_time[i]};
        ids.emplace_back(move(id));
    }
    nav_.InitRobots(ids, setts.coords);
    LOG_EVENT_STREAM("all robots are initialised", log_.stream());
}

void System::InitTasks(vector<std::pair<string, string>>&& setts)
{
    LOG_MESS_STREAM("starting initializing tasks of system, planned: " + to_string(setts.size()), log_.stream());
    for(auto&& [from, to] : setts)
    {
        tasks_.emplace(move(Task{from, to}));
    }
    LOG_EVENT_STREAM(to_string(tasks_.size()) + " tasks are initialised", log_.stream());
}

void System::InitLogSystem(string&& log_file_name)
{
    LOG_MESS("starting initializing log system");
    log_ = LogInterface{log_file_name};
    LOG_EVENT("log system is initialised");
}

void System::Start()
{
    LOG_MESS_STREAM("state of system at the begin is:", log_.stream());
    nav_.PrintState(log_.stream());
    PerformTasks();
}

void System::PerformTasks()
{
    LOG_MESS_STREAM("ready to perform " + to_string(tasks_.size()) + " tasks", log_.stream());
    int tact = 0;
    while(!tasks_.empty() || !busy_robots_.empty())
    {
        ++tact;
        TakeTasks();

        auto became_free_robots = ManageRobots(tact);
        for(auto& id : became_free_robots) 
        {
            free_robots_[id] = move(busy_robots_[id]);
            busy_robots_.erase(id);
        }
        
        LOG_MESS_STREAM("state after " + to_string(tact) + " tact is:\n", log_.stream());
        nav_.PrintState(log_.stream());
    }
}

void System::TakeTasks()
{
    for(int i = min(free_robots_.size(), tasks_.size()); i > 0; --i){
        auto task = tasks_.front();
        tasks_.pop();

        unordered_set<string> free_rob_ids;
        for(auto& f : free_robots_) free_rob_ids.emplace(f.first);
        auto robot_id = nav_.LookForRobot(task.cell_id_from, free_rob_ids);
        LOG_MESS_STREAM("for task \"" + task.cell_id_from + " to " + task.cell_id_to + "\" found " + robot_id,
                 log_.stream());
        
        busy_robots_[robot_id] = move(free_robots_[robot_id]);
        free_robots_.erase(robot_id);
        busy_robots_[robot_id].SetTask(task);
    }
}

unordered_set<string> System::ManageRobots(int tact)
{
    unordered_set<string> became_free;
    for(auto& [id_r, robot] : busy_robots_)
    {
        try
        {
            if(working_robots_.contains(id_r))
            {
                ManageRobotsWork(id_r);
            }
            else if(!robot.HasRoute())
            {
                if(nav_.IsRobotInAGoal(robot.GetATask(), robot.GetCoords()))
                {
                    ManageRobotsAroundA(id_r);
                }
                else if(nav_.IsRobotInBGoal(robot.GetBTask(), robot.GetCoords()))
                {
                    auto free_robs = ManageRobotsAroundB(id_r);
                    for(auto rob : free_robs)
                    {
                        became_free.emplace(rob);
                    }
                }
                else
                {
                    auto free_robs = GiveOrCompleteRoute(id_r);
                    for(auto rob : free_robs)
                    {
                        became_free.emplace(rob);
                    }
                }
            }
            else
            {
                ManageRobotsMove(id_r);
            }
        }
        catch(const std::exception& e)
        {
            LOG_ERROR_STREAM(e.what(), log_.stream());
            Achtung(tact);
        }
        
        RobotTask task = { robot.GetId(), robot.GetATask() + " to " + robot.GetBTask(), robot.GetCoords()};
        LOG_ROBOT_STATE(tact, robot.GetState(), task, log_.stream());
    }
    return became_free;
}

void System::ManageRobotsWork(std::string id)
{
    int rest_time = working_robots_[id];
    if(rest_time - 1 > 0)
    {
        working_robots_[id] -= 1;
    }
    else
    {
        working_robots_.erase(id);    
    }
}

void System::ManageRobotsAroundA(std::string id)
{
    auto& robot = busy_robots_[id];
    if(robot.CompletedA())
    {
        try
        {
            auto route = nav_.BuildRoute(robot.GetCoords(), robot.GetBTask());
          
            if(!route.empty() && nav_.IsRobotMoveCorrect(route.front()))
            {
                robot.SetRoute(move(route));
                robot.Move();
                nav_.UpdateRobotLocation(id, robot.GetCoords());
                robot.ChangeState(RobotState::B_MOVE);
            }
            else
            {
                robot.ChangeState(RobotState::WAIT);
            }    
        }
        catch(const exception& e)
        {
            throw runtime_error(e.what());
        }
    }
    else
    {
        if(robot.GetAWaitTime() - 1 > 0)
        {
            working_robots_[id] = robot.GetAWaitTime() - 1;
        }

        robot.ChangeState(RobotState::WORK);
    }
}

unordered_set<string> System::ManageRobotsAroundB(string id)
{
    auto& robot = busy_robots_[id];
    unordered_set<string> became_free;
    if(robot.CompletedB())
    {
        try
        {
            robot.SetRoute(nav_.FreeUpActiveCell(robot.GetBTask()));
            robot.Move();
            nav_.UpdateRobotLocation(id, robot.GetCoords());
            robot.ChangeState(RobotState::AWAY_MOVE);
            }
        catch(const std::exception& e)
        {
            throw runtime_error(e.what());
        }
        
    }
    else
    {
        if(robot.GetBWaitTime() - 1 > 0)
        {
            working_robots_[id] = robot.GetBWaitTime() - 1;
        }
        robot.ChangeState(RobotState::WORK);
    }

    return became_free;
}

std::unordered_set<std::string> System::GiveOrCompleteRoute(string id)
{
    auto& robot = busy_robots_[id];
    unordered_set<string> became_free;
    if(robot.GetState() == RobotState::AWAY_MOVE)
    {
        became_free.emplace(id);
        robot.ChangeState(RobotState::RELAX);
    }
    else
    {
        try
        {
            auto route = nav_.BuildRoute(robot.GetCoords(), robot.GetATask());
            if(!route.empty() && nav_.IsRobotMoveCorrect(route.front()))
            {
                robot.SetRoute(move(route));
                robot.Move();
                nav_.UpdateRobotLocation(id, robot.GetCoords());
                robot.ChangeState(RobotState::A_MOVE);
            }
            else
            {
                robot.ChangeState(RobotState::WAIT);
            }
        }
        catch(const exception& e)
        {
            throw runtime_error(e.what());
        } 
    }

    return became_free;
}

void System::ManageRobotsMove(string id)
{
    auto& robot = busy_robots_[id];
    if(nav_.IsRobotMoveCorrect(robot.NextStep()))
    {
        robot.Move();
        nav_.UpdateRobotLocation(id, robot.GetCoords());
        if(robot.GetState() == RobotState::WAIT)
        {
            robot.ChangeState((robot.CompletedA()) ? 
                RobotState::B_MOVE : RobotState::A_MOVE);
        }
    }
    else
    {
        try
        {
            string task_id = (robot.CompletedA()) ? robot.GetBTask() : robot.GetATask();
            auto route = nav_.BuildRoute(robot.GetCoords(), task_id);
            if(!route.empty() && nav_.IsRobotMoveCorrect(route.front()))
            {
                robot.SetRoute(move(route));
                robot.Move();
                nav_.UpdateRobotLocation(id, robot.GetCoords());
                if(robot.GetState() == RobotState::WAIT)
                {
                    robot.ChangeState((robot.CompletedA()) ? 
                        RobotState::B_MOVE : RobotState::A_MOVE);
                }
            }
            else
            {
                robot.ChangeState(RobotState::WAIT);
            }
        }
        catch(const std::exception& e)
        {
            throw runtime_error(e.what());
        }        
    }
}

void System::Achtung(int tact)
{
    for(auto& [id_r, robot] : free_robots_)
    {
        robot.SetRoute({nav_.MoveToCenter(robot.GetCoords())});
        robot.Move();
        nav_.UpdateRobotLocation(id_r, robot.GetCoords());
        RobotTask task = { robot.GetId(), "center", robot.GetCoords()};
        LOG_ROBOT_STATE(tact, RobotState::CENTER_MOVE, task, log_.stream());
    }
}