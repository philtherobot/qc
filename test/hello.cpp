#!/bin/bash /home/philippe/bin.v2/qc

#include <iostream>

using namespace std;

#include <boost/shared_ptr.hpp>
#include "boost/date_time/gregorian/gregorian.hpp"

#include <QString>
#include <QProcess>
#include <QTextStream>

QTextStream sout(stdout);

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


int main()
{
    boost::shared_ptr<A> a( new A );
    
    std::cout << "Hello Pierre-Paul.\n";
    
    using namespace boost::gregorian;
    std::cout << "Today is " << day_clock::local_day() << '\n';
    
    QString test("test");
    
    string text = read_from_subprocess("find .");
    cout << text;

    write_to_subprocess("grep debug",  text);
    
    return 0;
}
