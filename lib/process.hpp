
#include <string>
#include <vector>

#include <boost/optional.hpp>

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


class Command
{
public:
    Command(char const * cmd)  // implicit!
        : cmd_{cmd} {}
    Command(std::string cmd)  // implicit!
        : cmd_{cmd} {}
    Command(std::string executable, std::vector<std::string> args);
    
    std::string command() const { return cmd_; }

private:
    std::string cmd_;
};


class ProcessGroup
{
public:
    
    ProcessGroup & pipe(Command cmd);
    ProcessGroup & istring(std::string i);
    ProcessGroup & ostring(std::string * o = 0);
//    P & error(string * er);
//    P & merge_error();
    
    std::string operator() ();

private:
    std::vector<Command> cmds_;
    boost::optional<std::string> istring_;
    boost::optional<std::string*> ostring_;
};


ProcessGroup exec(Command cmd);


