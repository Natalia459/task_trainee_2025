#pragma once

#include "common.hpp"
#include "navigator.hpp"
#include "logging.hpp"
#include "robot.hpp"

#include <string>
#include <vector>
#include <queue>

class System
{
public:

System() = default;

void InitNavigator(Size size, ActiveCellSettings&& setts);
void InitRobots(RobotSettings&& setts);
void InitTasks(std::vector<std::pair<std::string, std::string>>&& setts);
void InitLogSystem(std::string&& log_file_name);

void Start();

private:
    Navigator nav_;
    std::unordered_map<std::string, Robot> free_robots_ = {};
    std::unordered_map<std::string, Robot> busy_robots_ = {};
    std::unordered_map<std::string, size_t> working_robots_ = {};
    
    LogInterface log_;
    std::queue<Task> tasks_ = {};

    void PerformTasks();
    void TakeTasks();
    std::unordered_set<std::string> ManageRobots(int tact);
    void ManageRobotsWork(std::string id);
    void ManageRobotsAroundA(std::string id);
    std::unordered_set<std::string> ManageRobotsAroundB(std::string id);
    std::unordered_set<std::string> GiveOrCompleteRoute(std::string id);
    void ManageRobotsMove(std::string id);

    void Achtung(int tact);
};