
#include "exec.hpp"

#include <iostream>

#include <unistd.h>
#include <cstdio>
#include <map>

#include <QString>
#include <QProcess>
#include <QTextStream>

#include <boost/ptr_container/ptr_vector.hpp>


namespace qc
{

namespace
{

class InteractiveProcess : public QProcess
{
    static int stdinClone;
public:
    InteractiveProcess(QObject *parent = 0)
        : QProcess(parent)
    {
        if (stdinClone == -1)
            stdinClone = ::dup(::fileno(stdin));
    }
protected:
    void setupChildProcess()
    {
        ::dup2(stdinClone, ::fileno(stdin));
    }
};

int InteractiveProcess::stdinClone = -1;


void throw_if_error(QProcess & proc)
{
    switch( proc.error() )
    {
        case QProcess::UnknownError: return;

        case QProcess::FailedToStart:
        {
            std::string p = proc.program().toStdString();
            throw start_error("failed to start '" + p + "'");
        }

        default:
        {
            std::map<QProcess::ProcessError, std::string> e;
            e[QProcess::Crashed] = "crashed";
            e[QProcess::Timedout] = "timed out";
            e[QProcess::WriteError] = "write error";
            e[QProcess::ReadError] = "read error";
            throw std::runtime_error("process error: " + e[proc.error()]);
        }
    }
}

#if 0
void dbg(std::string s)
{
    std::cout << s;
    std::cout.flush();
}
#else
void dbg(std::string)
{}
#endif

} // anonymous


ProcessGroup & ProcessGroup::pipe(Command cmd)
{
    cmds_.push_back(cmd);
    return *this;
}

ProcessGroup & ProcessGroup::istring(std::string i)
{
    istring_ = i;
    ifile_ = boost::optional<std::string>();
    return *this;
}

ProcessGroup & ProcessGroup::ifile(std::string i)
{
    ifile_ = i;
    istring_ = boost::optional<std::string>();
    return *this;
}

ProcessGroup & ProcessGroup::ostring(std::string * o)
{
    ostring_ = o;
    return *this;
}

std::string ProcessGroup::operator() ()
{
    if( cmds_.size() == 0 ) return std::string();
   
    
    boost::ptr_vector<QProcess> procs;
    
    auto it = cmds_.begin();

    while( it != cmds_.end() )
    {
        if( it == cmds_.begin() && ! istring_ && ! ifile_ )
        {
            dbg("use an InteractiveProcess\n");
            procs.push_back( new InteractiveProcess );
        }
        else
        {
            dbg("use a QProcess\n");
            procs.push_back( new QProcess );
        }

        ++ it;
    }

    auto pit = procs.begin() + 1;
    auto pprev = procs.begin();
    
    while( pit != procs.end() )
    {
        pprev->setStandardOutputProcess( &(*pit) );
        ++ pit;
        ++ pprev;
    }

    if( ! ostring_ )
    {
        dbg("forward channels to stdout\n");
        pprev->setProcessChannelMode(QProcess::ForwardedChannels);
    }
    
    if( ifile_ )
    {
        dbg("input from file " + *ifile_ + "\n");
        QString fname = QString::fromStdString(*ifile_);
        procs.front().setStandardInputFile( fname );
    }
        
    pit = procs.begin();
    it = cmds_.begin();
    while( pit != procs.end() )
    {
        std::string cmd { it->command() };
        dbg( "command '" + cmd + "'\n" );
        QString qcmd { QString::fromStdString(cmd) };
        pit->start( qcmd );
        throw_if_error(*pit);
        ++ pit;
        ++ it;
    }
    
    if( istring_ )
    {
        dbg("feeding string input\n");
        QTextStream s(&procs.front());
        s << QString::fromStdString( *istring_ );
        s.flush();
        procs.front().closeWriteChannel();
    }

    pit = procs.begin();
    while( pit != procs.end() )
    {
        pit->waitForFinished(-1);
        throw_if_error(*pit);
        ++ pit;
    }
 
    if( ostring_ )
    {
        dbg("capturing output\n");
        QTextStream s(&procs.back());
        std::string r = s.readAll().toStdString();

        if( *ostring_ )
        {
            **ostring_ = r;
        }

        return r;
    }

    return std::string();
}

///////////////////////////////////////////////////////////////

ProcessGroup exec(Command cmd)
{
    ProcessGroup pg;
    pg.pipe(cmd);
    return pg;
}


} // qc

