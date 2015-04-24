#!/bin/bash /home/philippe/bin.v2/qc

#include <iostream>

using namespace std;

#include <unistd.h>

#include <QString>
#include <QProcess>
#include <QTextStream>


class A {};

std::string read_from_subprocess(std::string cmd)
{
    QProcess ps;
    ps.start(QString::fromStdString(cmd));
    ps.waitForFinished();
    QByteArray ba = ps.readAllStandardOutput();
    return std::string( ba.data(), ba.size() );
}

void write_to_subprocess(std::string cmd, std::string text)
{
    QProcess ps;
    ps.start(QString::fromStdString(cmd));
    ps.write(text.c_str(), text.size());
    ps.closeWriteChannel();
    ps.waitForFinished();
}


class InteractiveProcess : public QProcess
{
    static int stdinClone;
public:
    InteractiveProcess(QObject *parent = 0)
        : QProcess(parent)
    {
        if (stdinClone == -1)
            stdinClone = ::dup(fileno(stdin));
    }
protected:
    void setupChildProcess()
    {
        ::dup2(stdinClone, fileno(stdin));
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

class Process : public InteractiveProcess, Startable
{
public:
    Process(std::string cmd) : InteractiveProcess(), cmd_(cmd) {}
    
    virtual void start()
    {
        QProcess::start(QString::fromStdString(cmd_));
    }
    
private:
    std::string cmd_;
};

void execute(Process & p)
{
    p.setProcessChannelMode(QProcess::ForwardedChannels);
    p.start();
    p.waitForFinished(-1); // wait forever
}

typedef std::vector<Process> ProcessList;

void execute(ProcessList & pipe)
{
    if( pipe.size() == 0 ) return;
    if( pipe.size() == 1 ) { execute(pipe[0]); }
    
    ProcessList::iterator it;
    ProcessList::iterator prev;
    
    it = pipe.begin() + 1;
    prev = pipe.begin();
    
    while( it != pipe.end() )
    {
        prev->setProcessOutput( & (*it) );
        prev = it;
        ++ it;
    }
    
    it = pipe.begin();
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
