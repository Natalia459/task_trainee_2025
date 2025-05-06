#pragma once

#include "common.hpp"
#include "cell.hpp"

#include <unordered_map>
#include <string>

class Field
{
public:
    std::vector<Coord> potential_neightbours{{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    Field() = default;
    Field(Size size);

    Field(const Field&) = delete;
    Field& operator=(const Field&) = delete;

    Field(Field&& other);
    Field& operator=(Field&& rhs);

    void InitActiveCells(std::unordered_map<std::string, Coord>&& cells);
    void InitField(Size size);
    void InitRobotCell(std::string id, Coord coords);

    bool CheckCellsConnected(Coord a, Coord b) const;
    bool CheckCellIsBusy(Coord c) const;
    bool CheckCellCanBeTask(Coord c) const;

    void Print(std::ostream& out) const;

    Coord GetActiveCellCoords(std::string task_id) const;
    Size GetSize() const;
    std::string GetRobotId(Coord c) const;

private:
    std::unordered_map<Coord, Cell> list_ = {};
    std::unordered_map<std::string, Coord> active_cells_ = {};
    std::unordered_map<std::string, Coord> busy_cells_ = {};
    Size size_;

};