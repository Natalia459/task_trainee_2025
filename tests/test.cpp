#include "system.hpp"
#include "common.hpp"
#include "collector.hpp"
#include "logging.hpp"

#include <iostream>
#include <string>
#include <filesystem>

int test(int argc, char* argv[])
{
    using namespace std;
    for(int i = 1; i <= 5; ++i)
    {
        LOG_MESS(to_string(i) + "starting to initialize system");
  
        using namespace std;
        System system;
        Collector col(system);
       
        if(static_cast<string>(argv[0]) == "cmd")
        {
            LOG_MESS(to_string(i) + "configuring system with command line)");
            
            col.ConfigureSystem();
        }
        else
        {
            LOG_MESS(to_string(i) + "configuring system with the default file (config.json)");
            std::filesystem::path p(std::filesystem::current_path());
            const string file_name = p.string() + "/configs/config" + to_string(i) + ".json";
            col.ConfigureSystem(file_name);
        }

        LOG_MESS(to_string(i) + " starting to work...");
        system.Start();
    
        LOG_MESS(to_string(i) + "shutting system down...");
    }
    return 1;
}