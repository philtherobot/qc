
#include <iostream>

#include <unistd.h>

#include <QString>
#include <QProcess>
//#include <QTextStream>
//QTextStream sout(stdout);

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


void execute(Process & p)
{
    InteractiveProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(QString::fromStdString(p.cmd()));
    throw_if_error(proc);
    proc.waitForFinished(-1); // wait forever
    throw_if_error(proc);
}


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

