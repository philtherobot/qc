#!/bin/bash /home/philippe/bin.v2/qc

#include <iostream>

using namespace std;

#include <unistd.h>
#include <cstdio>

#include <QString>
#include <QProcess>
#include <QTextStream>

#include <boost/ptr_container/ptr_vector.hpp>

QTextStream sout(stdout);

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


/*

[ifile] | [istring]  process +  [ofile] | [ostring]

class process;
class process_pipe : public process


execute( process, output = 0 )
execute( input, process, output = 0 )
*/


class Process
{
public:
    Process(std::string cmd) : cmd_(cmd) {}
    
    std::string cmd() const { return cmd_; }
    
private:
    std::string cmd_;
};

void throw_if_error(QProcess & proc)
{
    if( proc.error() != QProcess::UnknownError )
        throw std::runtime_error("process error");
}

void execute(Process & p)
{
    InteractiveProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(QString::fromStdString(p.cmd()));
    throw_if_error(proc);
    proc.waitForFinished(-1); // wait forever
    throw_if_error(proc);
}

typedef std::vector<Process> ProcessList;

void execute(ProcessList & pipe)
{
    if( pipe.size() == 0 ) return;
    if( pipe.size() == 1 ) { execute(pipe[0]); }
    
    
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

    pprev->setProcessChannelMode(QProcess::ForwardedChannels);
    
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
}

int user_main()
{
    sout << "running sample_source\n";
    sout.flush();
    Process f("./sample_source");
    execute(f);
    
    sout << "running grep (blocks on stdin)\n";
    sout.flush();
    Process g("grep ba");
    execute(g);
    
    sout << "running a pipe\n";
    sout.flush();
    ProcessList pl;
    pl.push_back(f);
    pl.push_back(g);
    execute(pl);
    
    return 0;
}

int main()
{
    try
    {
        return user_main();
    }
    catch(std::exception const & ex)
    {
        sout << "exception: " << ex.what() << '\n';
        return 1;
    }
    sout << "unknown exception\n";
    return 1;
}
