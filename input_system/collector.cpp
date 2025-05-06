#include "collector.hpp"
#include "field.hpp"
#include "parser_json.hpp"
#include "logging.hpp"

#include <iostream>
#include <filesystem>

using namespace std;

Collector::Collector(System& system)
:system_(system)
{}

void Collector::ConfigureSystem(const string& file_name)
{
    if(file_name.empty())
    {
        ReadConfigFromCin();
    }
    else
    {
        ifstream fin(file_name); 
        ReadConfigFromFile(fin);
    }
}

void Collector::ReadConfigFromCin()
{
    InitSystem(Load(cin));
}

void Collector::ReadConfigFromFile(ifstream& fin)
{
    if(!fin.is_open())
    {
        throw runtime_error("File doesn't exist!");
    }

    InitSystem(Load(fin));
}

void Collector::InitSystem(Parser&& data)
{
    LOG_MESS("configure data parsed");

    std::filesystem::path p(std::filesystem::current_path());
    const string file_path = p.parent_path().string();
    if(!data.GetLogFileName().empty())
    {    
        string log_file = file_path + "/" + data.GetLogFileName();
        LOG_MESS("log_file in collector is " + log_file);
        system_.InitLogSystem(move(log_file));
    }
    system_.InitNavigator(data.GetFieldSize(), data.GetActiveCellSettings());
    system_.InitRobots(data.GetRobotSettings());
    
    if(!data.GetTaskFileName().empty())
    {
        string task_file = file_path + "/" + data.GetTaskFileName();
        system_.InitTasks(ReadTasksFromFile(task_file));
    }
    else
    {
        system_.InitTasks(ReadTasksFromCin());
    }
    LOG_EVENT("system is initialized");
}

vector<pair<string, string>> Collector::ReadTasksFromCin()
{
    return ParseTask(std::cin);
}

vector<pair<string, string>> Collector::ReadTasksFromFile(string file_name)
{
    ifstream in(file_name);
    if(!in.is_open())
    {
        LOG_ERROR("tasks file can't be opened, name is: " + file_name + "\n");
        throw runtime_error("File name is wrong");
    }
    return ParseTask(in);
}

vector<pair<string, string>> Collector::ParseTask(std::istream& in)
{
    string task_id;
    pair<string, string> task;
    vector<pair<string, string>> res;
    bool a = true;
    bool& pair_incomplete = a;
    
    while(in >> task_id)
    {
        auto& t = (a) ? task.first : task.second;
        t = task_id;
        if(!pair_incomplete)
        {
            res.emplace_back(task);
        }
        a = !a;
    }
    LOG_EVENT("task data parsed");
    return res;
}