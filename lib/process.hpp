
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



/*

[ifile] | [istring]  process +  [ofile] | [ostring]

class process;
class process_pipe : public process


execute( process, output = 0 )
execute( input, process, output = 0 )
*/



void execute(Process & p);
void execute(ProcessList & pipe);

