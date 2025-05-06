#include "system.hpp"
#include "common.hpp"
#include "collector.hpp"
#include "logging.hpp"
#include "duration.hpp"

#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char* argv[])
{
    {
        LOG_DURATION("checking time of executing...\n");
        LOG_MESS("starting to initialize system");
  
        using namespace std;
        System system;
        Collector col(system);
    
        if(argc!= 0 && static_cast<string>(argv[0]) == "cmd")
        {
            LOG_MESS("configuring system with command line)");
            
            col.ConfigureSystem();
        }
        else
        {
            LOG_MESS("configuring system with the default file (config.json)");
            std::filesystem::path p(std::filesystem::current_path());
            const string file_name = p.parent_path().string() + "/config.json";
            col.ConfigureSystem(file_name);
        }

        LOG_MESS("starting to work...");
        system.Start();

        LOG_MESS("shutting system down...");
    }
}