
#include "process.hpp"

#include <iostream>

#include <unistd.h>
#include <cstdio>
#include <map>

#include <QString>
#include <QProcess>
#include <QTextStream>

#include <boost/ptr_container/ptr_vector.hpp>


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
    if( proc.error() != QProcess::UnknownError )
    {
        std::map<QProcess::ProcessError, std::string> e;
        e[QProcess::FailedToStart] = "failed to start";
        e[QProcess::Crashed] = "crashed";
        e[QProcess::Timedout] = "timed out";
        e[QProcess::WriteError] = "write error";
        e[QProcess::ReadError] = "read error";
        throw std::runtime_error("process error: " + e[proc.error()]);
    }
}

#if 0
void dbg(std::string s)
{
    std::cout << s;
    std::cout.flush();
#else
void dbg(std::string)
{}
#endif

} // anonymous


void execute(Process & p, std::string * output)
{
    InteractiveProcess proc;

    if( !output )
    {
        proc.setProcessChannelMode(QProcess::ForwardedChannels);
    }

    proc.start(QString::fromStdString(p.cmd()));
    throw_if_error(proc);
    proc.waitForFinished(-1); // wait forever
    throw_if_error(proc);

    if( output )
    {
        QTextStream s(&proc);
        *output = s.readAll().toStdString();
    }
}


void execute(ProcessList & pipe, std::string * output)
{
    if( pipe.size() == 0 ) return;
    if( pipe.size() == 1 ) 
    { 
        execute(pipe[0], output); 
        return;
    }
    
    
    boost::ptr_vector<QProcess> procs;
    
    procs.push_back(new InteractiveProcess);

    auto it = pipe.begin() + 1;

    while( it != pipe.end() )
    {
        procs.push_back( new QProcess );
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

    if( !output )
    {
        pprev->setProcessChannelMode(QProcess::ForwardedChannels);
    }
    
    pit = procs.begin();
    it = pipe.begin();
    while( pit != procs.end() )
    {
        QString cmd = QString::fromStdString(it->cmd());
        pit->start( cmd );
        throw_if_error(*pit);
        ++ pit;
        ++ it;
    }
    
    pit = procs.begin();
    while( pit != procs.end() )
    {
        pit->waitForFinished(-1);
        throw_if_error(*pit);
        ++ pit;
    }

    if( output )
    {
        QTextStream s(&procs.back());
        *output = s.readAll().toStdString();
    }
}

void execute(std::string input, Process & p, std::string * output)
{
    QProcess proc;
    
    if( !output )
    {
        proc.setProcessChannelMode(QProcess::ForwardedChannels);
    }
    
    proc.start(QString::fromStdString(p.cmd()));
    throw_if_error(proc);
    
    QTextStream s(&proc);
    s << QString::fromStdString(input);
    s.flush();
    proc.closeWriteChannel();
    
    proc.waitForFinished(-1); // wait forever
    throw_if_error(proc);
    
    if( output )
    {
        QTextStream s(&proc);
        *output = s.readAll().toStdString();
    }
}

void execute(std::string input, ProcessList & pipe, std::string * output)
{
    if( pipe.size() == 0 ) return;
    if( pipe.size() == 1 ) 
    { 
        execute(input, pipe[0], output); 
        return; 
    }
    
    
    boost::ptr_vector<QProcess> procs;
    
    auto it = pipe.begin();

    while( it != pipe.end() )
    {
        procs.push_back( new QProcess );
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

    if( !output )
    {
        pprev->setProcessChannelMode(QProcess::ForwardedChannels);
    }
    
    pit = procs.begin();
    it = pipe.begin();
    while( pit != procs.end() )
    {
        QString cmd = QString::fromStdString(it->cmd());
        pit->start( cmd );
        throw_if_error(*pit);
        ++ pit;
        ++ it;
    }
    
    QTextStream s(&procs.front());
    s << QString::fromStdString(input);
    s.flush();
    procs.front().closeWriteChannel();

    pit = procs.begin();
    while( pit != procs.end() )
    {
        pit->waitForFinished(-1);
        throw_if_error(*pit);
        ++ pit;
    }

    if( output )
    {
        QTextStream s(&procs.back());
        *output = s.readAll().toStdString();
    }
}


std::string execapture(Process & p)
{
    std::string ou;
    execute(p, &ou);
    return ou;
}
    
std::string execapture(ProcessList & pipe)
{
    std::string ou;
    execute(pipe, &ou);
    return ou;
}
    
std::string execapture(std::string input, Process & p)
{
    std::string ou;
    execute(input, p, &ou);
    return ou;
}

std::string execapture(std::string input, ProcessList & pipe)
{
    std::string ou;
    execute(input, pipe, &ou);
    return ou;
}

///////////////////////////////////////////////////////////////

ProcessGroup & ProcessGroup::pipe(Command cmd)
{
    cmds_.push_back(cmd);
    return *this;
}

ProcessGroup & ProcessGroup::istring(std::string i)
{
    istring_ = i;
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
        if( it == cmds_.begin() && ! istring_ )
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
        pprev->setProcessChannelMode(QProcess::ForwardedChannels);
    }
    
    pit = procs.begin();
    it = cmds_.begin();
    while( pit != procs.end() )
    {
        QString cmd = QString::fromStdString(it->command());
        pit->start( cmd );
        throw_if_error(*pit);
        ++ pit;
        ++ it;
    }
    
    if( istring_ )
    {
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



