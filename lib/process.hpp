
#include <string>
#include <vector>

class Process
{
public:
    Process(std::string cmd) : cmd_(cmd) {}
    
    std::string cmd() const { return cmd_; }
    
private:
    std::string cmd_;
};

typedef std::vector<Process> ProcessList;

void execute(Process & p, std::string * output =  0);
void execute(ProcessList & pipe, std::string * output =  0);
void execute(std::string input, Process & p, std::string * output =  0);
void execute(std::string input, ProcessList & pipe, std::string * output =  0);

std::string execapture(Process & p);
std::string execapture(ProcessList & pipe);
std::string execapture(std::string input, Process & p);
std::string execapture(std::string input, ProcessList & pipe);
