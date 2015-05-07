
#include <QString>
#include <QStringList>
#include <QVector>

#include <stdexcept>

#include <boost/optional.hpp>

namespace qc
{

struct start_error : public std::runtime_error
{
    start_error(QString w) : std::runtime_error(w.toStdString()) {}
};

class Command
{
public:
	Command() {}
    Command(char const * cmd)  // implicit!
        : cmd_{cmd} {}
    Command(QString cmd)  // implicit!
        : cmd_{cmd} {}
    Command(QString executable, QStringList args);
    
    QString command() const { return cmd_; }

private:
    QString cmd_;
};


class ProcessGroup
{
public:
    
    ProcessGroup & pipe(Command cmd);

    ProcessGroup & istring(QString i);
    ProcessGroup & ifile(QString i);

    ProcessGroup & ostring(QString * o = 0);
    ProcessGroup & ofile(QString o);

    ProcessGroup & estring(QString * e = 0);
    ProcessGroup & efile(QString e);

    //    P & merge_error();
    // exit_code(int *);
    
    QString operator() ();

private:
    QVector<Command> cmds_;
    boost::optional<QString> istring_;
    boost::optional<QString> ifile_;
    boost::optional<QString*> ostring_;
    boost::optional<QString> ofile_;
    boost::optional<QString*> estring_;
    boost::optional<QString> efile_;
};


ProcessGroup exec(Command cmd);

} //qc

