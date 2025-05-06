#pragma once

#include "common.hpp"
#include "json.hpp"

#include <string>

class Parser{
public:
    Parser(Document&& doc);

    Size GetFieldSize() const;
    RobotSettings GetRobotSettings() const;
    ActiveCellSettings GetActiveCellSettings() const;
    std::string GetTaskFileName() const;
    std::string GetLogFileName() const;

    Size&& GetFieldSize();
    RobotSettings&& GetRobotSettings();
    ActiveCellSettings&& GetActiveCellSettings();
    std::string&& GetTaskFileName();
    std::string&& GetLogFileName();

private:
    Size size_;
    RobotSettings robot_sets_;
    ActiveCellSettings act_cell_sets_;
    std::string task_file_name_;
    std::string log_file_name_;

    void RenderFieldSize(const Array& data);
    void RenderRobotSettings(const Dict& data);
    void RenderActiveCellSettings(const Dict& data);
};