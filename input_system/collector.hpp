#pragma once

#include "system.hpp"
#include "parser_json.hpp"

#include <fstream>
#include <string>
#include <vector>
#include <utility>

class Collector
{
public:
	Collector(System& system);

	void ConfigureSystem(const std::string& file_name = "");

private:
	System& system_;

	void ReadConfigFromCin();
	void ReadConfigFromFile(std::ifstream& fin);
	void InitSystem(Parser&& data);
	std::vector<std::pair<std::string, std::string>> ReadTasksFromCin();
	std::vector<std::pair<std::string, std::string>> ReadTasksFromFile(std::string file_name);
	std::vector<std::pair<std::string, std::string>> ParseTask(std::istream& in);
};