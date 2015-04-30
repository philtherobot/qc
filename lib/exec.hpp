
#include <string>
#include <vector>

#include <boost/optional.hpp>

namespace qc
{

struct start_error : public std::runtime_error
{
    start_error(std::string const & w) : std::runtime_error(w) {}
};

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
    ProcessGroup & ifile(std::string i);

    ProcessGroup & ostring(std::string * o = 0);
//    P & error(string * er);
//    P & merge_error();
    
    std::string operator() ();

private:
    std::vector<Command> cmds_;
    boost::optional<std::string> istring_;
    boost::optional<std::string> ifile_;
    boost::optional<std::string*> ostring_;
};


ProcessGroup exec(Command cmd);

} //qc

