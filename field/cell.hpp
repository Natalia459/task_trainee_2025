#pragma once

#include "common.hpp"

#include <string>

class Cell
{
public:
    Cell() = default;
    Cell(int x, int y);

    bool operator==(const Cell& rhs) const = default;

    void SetTaskId(std::string id);
    void SetRobotId(std::string id);
    void MakeBusy();
    void MakeFree();

    Coord GetCoords() const;
    std::string GetTaskId() const;
    std::string GetRobotId() const;
    bool IsBusy() const;
    bool IsActiveCell() const;

private:
    Coord coords_;
    std::string robot_id_ = "";
    std::string task_id_ = "";
    bool is_busy_ = false;
};

template<>
struct std::hash<Cell>
{
    std::size_t operator()(const Cell& c) const noexcept
    {
        std::size_t h1 = std::hash<Coord>{}(c.GetCoords());
        std::size_t h2 = std::hash<bool>{}(c.IsBusy());
        return h1 ^ (h2 << 1); 
    }
};