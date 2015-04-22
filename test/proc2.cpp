#!/bin/bash /home/philippe/bin.v2/qc

#include <iostream>

using namespace std;

#include <unistd.h>
#include <cstdio>

#include <QString>
#include <QProcess>
#include <QTextStream>

#include <boost/ptr_container/ptr_vector.hpp>


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

class Startable
{
public:
    virtual ~Startable() {}
    virtual void start() = 0;
};

class Process
{
public:
    Process(std::string cmd) : cmd_(cmd) {}
    
    std::string cmd() const { return cmd_; }
    
private:
    std::string cmd_;
};

void execute(Process & p)
{
    InteractiveProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(QString::fromStdString(p.cmd()));
    proc.waitForFinished(-1); // wait forever
}

typedef std::vector<Process> ProcessList;

void execute(ProcessList & pipe)
{
    if( pipe.size() == 0 ) return;
    if( pipe.size() == 1 ) { execute(pipe[0]); }
    
    
    boost::ptr_vector<QProcess> procs;
    
    procs.push_back(new InteractiveProcess);

    ProcessList::iterator it;
    
    it = pipe.begin() + 1;

    while( it != pipe.end() )
    {
        procs.push_back( new QProcess );
        ++ it;
    }

    auto pit = procs.begin();
    auto pprev = 
    
    while( pit != procs.end() - 1 )
    {
        pit->setProcessOutput( & (*it) );
        procs.push_back( new QProcess );
        ++ it;
    }

    InteractiveProcess head;
    QProcess * antecedant = &head;

    ProcessList::iterator prev;
    prev = it;
    
    antecedant->setProcessOutput( & (*it) );
    antecedant->start( QString::fromStdString(prev->cmd()) );
    
    ++ it;

    while( it != pipe.end() )
    {
        antecedant->setProcessOutput( & (*it) );
        antecedant->start( QString::fromStdString(prev->cmd()) );

        antecedant = it;
        prev = it;
        ++ it;
    }
    
    antecedant->start( QString::fromStdString(prev->cmd()) );

    head.waitForFinished(-1);
    
    it = pipe.begin() + 1;
    while( it != pipe.end() )
    {
        it->waitForFinished(-1); // wait forever
        ++it;
    }
}

int main()
{
    //string text = read_from_subprocess("find .");
    //cout << text;

    //write_to_subprocess("grep debug",  text);
    
    Process f("find /home/philt_000/dev");
    execute( f );
    
    Process g("grep objects");
    execute(g);
    
    ProcessList pl;
    pl.push_back(f);
    pl.push_back(g);
    execute(pl);
    
    return 0;
}
