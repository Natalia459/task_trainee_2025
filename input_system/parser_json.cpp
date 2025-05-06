#include "parser_json.hpp"

using namespace std;

Parser::Parser(Document&& doc)
{
    for (const auto& [config_type, data] : doc.GetRoot().AsDict())
    {
        if(config_type == "field_size")
        {
            RenderFieldSize(data.AsArray());
        }
        else if(config_type == "robot_settings")
        {
            RenderRobotSettings(data.AsDict());
        }
        else if (config_type == "active_cells_settings")
        {
            RenderActiveCellSettings(data.AsDict());
        }
        else if (config_type == "task_file_name")
        {
            if(!data.IsNull())
            {
                task_file_name_ = data.AsString();
            }
        }
        else if (config_type == "log_file_name")
        {
            if(!data.IsNull())
            {
                log_file_name_ = data.AsString();
            }
        }
    }
}

Size Parser::GetFieldSize() const
{
    return size_;
}

RobotSettings Parser::GetRobotSettings() const
{
    return robot_sets_;
}

ActiveCellSettings Parser::GetActiveCellSettings() const
{
    return act_cell_sets_;
}

std::string Parser::GetTaskFileName() const
{
    return task_file_name_;
}

std::string Parser::GetLogFileName() const
{
    return log_file_name_;
}


Size&& Parser::GetFieldSize()
{
    return move(size_);
}

RobotSettings&& Parser::GetRobotSettings()
{
    return move(robot_sets_);
}

ActiveCellSettings&& Parser::GetActiveCellSettings()
{
    return move(act_cell_sets_);
}

std::string&& Parser::GetTaskFileName()
{
    return move(task_file_name_);
}

std::string&& Parser::GetLogFileName()
{
    return move(log_file_name_);
}


void Parser::RenderFieldSize(const Array& data)
{
    if(data.size() == 2)
    {
        size_.length = data[0].AsInt();
        size_.width = data[1].AsInt();
    }
    else
    {
        throw ParsingError("Size of field is incorrect");
    }
}

void Parser::RenderRobotSettings(const Dict& setts)
{
    for(const auto& [config_type, data] : setts){
        if(config_type == "amount")
        {
            robot_sets_.amount = data.AsInt();
        }
        else if(config_type == "coordinates")
        {
            robot_sets_.coords.reserve(robot_sets_.amount);
            for(const auto& coords : data.AsArray())
            {
                auto cell = coords.AsArray();
                if(cell.size() == 2)
                {
                    robot_sets_.coords.push_back(Coord{.x = cell[0].AsInt(), .y = cell[1].AsInt()});
                }
                else
                {
                    throw ParsingError("Settings of robots' coordinates are incorrect");
                }
            }
        }
        else if(config_type == "a_wait_time")
        {
            robot_sets_.a_wait_time.reserve(robot_sets_.amount);
            for(const auto& a : data.AsArray())
            {
                robot_sets_.a_wait_time.push_back(a.AsInt());
            }
        }
        else if(config_type == "b_wait_time")
        {
            robot_sets_.b_wait_time.reserve(robot_sets_.amount);
            for(const auto& b : data.AsArray())
            {
                robot_sets_.b_wait_time.push_back(b.AsInt());
            }
        }
        else 
        {
            throw ParsingError("Settings of robots are incorrect in general");
        }
    }
}

void Parser::RenderActiveCellSettings(const Dict& setts)
{
    for(const auto& [id, coords] : setts){
        if(coords.AsArray().size() == 2)
        {
            auto vals = coords.AsArray();
            act_cell_sets_.coords.emplace(id, Coord{.x = vals[0].AsInt(), .y = vals[1].AsInt()});
        }
        else
        {
            throw ParsingError("Settings of active cells are incorrect");
        }    
    }
}