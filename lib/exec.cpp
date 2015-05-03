
#include "exec.hpp"

#pragma qc_import(defaultio)

#include <cstdio>
#include <map>
#include <string>
#include <unistd.h>

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
            QString p = proc.program();
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
void dbg(QString s)
{
    dout << s;
    dout.flush();
}
#else
void dbg(QString)
{}
#endif

} // anonymous


ProcessGroup & ProcessGroup::pipe(Command cmd)
{
    cmds_.push_back(cmd);
    return *this;
}

ProcessGroup & ProcessGroup::istring(QString i)
{
    istring_ = i;
    ifile_ = boost::optional<QString>();
    return *this;
}

ProcessGroup & ProcessGroup::ifile(QString i)
{
    ifile_ = i;
    istring_ = boost::optional<QString>();
    return *this;
}

ProcessGroup & ProcessGroup::ostring(QString * o)
{
    ostring_ = o;
    ofile_ = boost::optional<QString>();
    return *this;
}

ProcessGroup & ProcessGroup::ofile(QString o)
{
    ofile_ = o;
    ostring_ = boost::optional<QString*>();
    return *this;
}

QString ProcessGroup::operator() ()
{
    if( cmds_.size() == 0 ) return QString();
   
    
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

    if( ostring_ )
    {}
    else if( ofile_ )
    {
    	pprev->setStandardOutputFile(*ofile_);
    }
    else
    {
        dbg("forward channels to stdout\n");
        pprev->setProcessChannelMode(QProcess::ForwardedChannels);
    }
    
    if( ifile_ )
    {
        dbg("input from file " + *ifile_ + "\n");
        procs.front().setStandardInputFile( *ifile_ );
    }
        
    pit = procs.begin();
    it = cmds_.begin();
    while( pit != procs.end() )
    {
        QString cmd { it->command() };
        dbg( "command '" + cmd + "'\n" );
        pit->start( cmd );
        throw_if_error(*pit);
        ++ pit;
        ++ it;
    }
    
    if( istring_ )
    {
        dbg("feeding string input\n");
        QTextStream s(&procs.front());
        s << *istring_;
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
        QString r = s.readAll();

        if( *ostring_ )
        {
            **ostring_ = r;
        }

        return r;
    }

    return QString();
}

///////////////////////////////////////////////////////////////

ProcessGroup exec(Command cmd)
{
    ProcessGroup pg;
    pg.pipe(cmd);
    return pg;
}


} // qc

