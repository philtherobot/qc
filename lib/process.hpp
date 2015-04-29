
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


class Command
{
public:
    Command(string cmd);  // implicit!
    Command(string executable, std::vector<string> args);
    
    string 
};

class P
{
public:
    P(Command cmd);
    
    P & pipe(Command cmd);
    P & input(string in);
    P & output(string * ou = 0);
    P & error(string * er);
    P & merge_error();
    
    string operator() ();
    
private:
    string user_input_;  // copy of input
    string * input_;  // optional, if input was called
    
    string local_output_;   // if should capture, but user wants it returned
    string * output_;  // optional, if we should capture
    
    std::vector<Process> pipe_;  // cmd & arguments of each
};

P exec(string cmd);

void demo()
{
    exec("find .")();
    
    string text; // = "line1\nline2\n ... 
    exec("grep line").input(text)();
    
    string r;
    exec("find .").pipe("grep '*.cpp'").output(r)();
    
    /* how to: string r = exec("find .")();
    can be done by making argument to output optional.
    if output() was called, then output will be captured
    and 'operator()' will return a string.  Otherwise,
    'operator()' always returns an empty string.
    */
}

