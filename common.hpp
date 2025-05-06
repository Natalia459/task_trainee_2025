#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

struct Size 
{
	int length = 0;
	int width = 0;
};

struct Coord
{
    int x = 0;
    int y = 0;
	bool operator==(const Coord& rhs) const = default;
};

template<>
struct std::hash<Coord>
{
    std::size_t operator()(const Coord& c) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(c.x);
        std::size_t h2 = std::hash<int>{}(c.y);
        return h1 ^ (h2 << 1);
    }
};

struct ActiveCellSettings
{
	std::unordered_map<std::string, Coord> coords = {};
};

struct Task
{
	std::string cell_id_from = "";
    std::string cell_id_to = "";
};

struct RobotSettings
{
	int amount = 0;
	std::vector<Coord> coords = {};
	std::vector<int> a_wait_time = {};
	std::vector<int> b_wait_time = {};	
};

enum class RobotState
{
	RELAX,
	A_MOVE,
	WAIT,
	WORK,
	B_MOVE,
	AWAY_MOVE,
	CENTER_MOVE
};

struct RobotTask
{
	std::string robot = "";
	std::string id = "";
	Coord coords;
};
