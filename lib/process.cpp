
#include "process.hpp"

#include <iostream>

#include <unistd.h>
#include <cstdio>

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
        throw std::runtime_error("process error");
}

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
