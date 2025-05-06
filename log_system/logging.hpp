#pragma once

#include "common.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <memory>

#define LOG_ERROR(mess) LogError (mess)
#define LOG_EVENT(mess) LogEvent(mess)
#define LOG_MESS(mess) LogMessage(mess)
#define LOG_ERROR_STREAM(mess, stream) LogError (mess, stream)
#define LOG_EVENT_STREAM(mess, stream) LogEvent (mess, stream)
#define LOG_MESS_STREAM(mess, stream) LogMessage (mess, stream)

#define LOG_ROBOT_STATE(tact, state, task, stream) LogRobotState(tact, state, task, stream)


inline std::unordered_map<RobotState, std::string> state_converter = 
{
    {RobotState::RELAX, " robot is free"},
    {RobotState::A_MOVE, " robot is moving to (A)"},
    {RobotState::WAIT, " robot can't move, it's waiting"},
    {RobotState::WORK, " robot is working"},
    {RobotState::B_MOVE, " robot is moving to (B)"},
    {RobotState::AWAY_MOVE, " robot is moving to free up task's cell"},
    {RobotState::CENTER_MOVE, " robot is moving to free up road for other"}
};

inline void LogMessage(std::string_view mess, std::ostream& os = std::cout)
{
    os << "\nMESSAGE: " << mess;
}

inline void LogEvent(std::string_view mess, std::ostream& os = std::cout)
{
    os << "\nEVENT: " << mess;
}

inline void LogError(std::string_view mess, std::ostream& os = std::cerr)
{
    os << "\nERROR: " << mess;
}

inline void LogRobotState(int tact, RobotState state, RobotTask task, std::ostream& os = std::cout)
{
    std::string mess = "\ntact " + std::to_string(tact) + ": " + task.robot + state_converter[state];
    if(!task.id.empty())
    {
        mess += "\ncurrent task is " + task.id ;
    }
    mess += "\ncoordinates: x= " + std::to_string(task.coords.x) + ", y= " + std::to_string(task.coords.y) + ".\n";
    LogEvent(mess, os);
}

class LogInterface
{
public:
    LogInterface() = default;
    LogInterface(const std::string& file_name);

    LogInterface(const LogInterface& other) = delete;
    LogInterface& operator=(const LogInterface& rhs) = delete;

    LogInterface(LogInterface&& other) noexcept = default;
    LogInterface& operator=(LogInterface&& rhs) noexcept = default;

    std::string GetFileName() const;

    std::ostream& stream();

    ~LogInterface();

private:
    std::string file_name_ = "";
    std::ostringstream sout_;

    void WriteInFile();
};
