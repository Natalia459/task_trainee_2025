#include "logging.hpp"

#include <memory>

using namespace std;

LogInterface::LogInterface(const string& file_name)
:file_name_(file_name), sout_("")
{
    ofstream in(file_name_, ios::out | ios::trunc);
    in << "START";
}

string LogInterface::GetFileName() const { return file_name_; }

ostream& LogInterface::stream()
{
    if(file_name_.empty())
    {
        return cout;
    }
    if(sout_.str().size() >= 10)
    {
        WriteInFile();
        sout_.str("");
    }
    return sout_;
}

LogInterface::~LogInterface()
{
    if(!sout_.view().empty())
    {
        WriteInFile();
    }
}

void LogInterface::WriteInFile()
{
    ofstream out(file_name_, ios::out | ios::app);
    if(!out.is_open())
    {
        LOG_ERROR("can't open log file " + file_name_ + "\n");
        throw runtime_error("File name for logging is wrong");
    }
    out << sout_.str();
}