#!/bin/bash /home/philippe/bin.v2/qc

#pragma qc_import(../../lib/exec)
#pragma qc_import(../../lib/unit_test)

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <QString>
#include <QTemporaryFile>
#include <QTextStream>

using namespace qc;
//using namespace std;


QString secret_text {
    "line 1\n"
    "line 2\n"
    "the secret is here\n"
    "line 4\n"
    "and also a secret here\n"
    "line 6\n"};



QString create_tmp_file(QString contents, QTemporaryFile & f)
{
    f.open();

    QTextStream ts(&f);
    ts << contents;

    f.close();

    return f.fileName();
}

void simple_exec()
{
    exec("./sample_source")();
}

void start_failure()
{
    try
    {
        exec("./notascript")();
    }
    catch(start_error const &)
    {
        return;
    }
    throw std::runtime_error("expected start_error exception in start_failure");
}

void capture_stdout()
{
    QString o;
    QString p;
    p = exec("./sample_source").ostring(&o)();

    QString expect{
        "line 1: sample data to consume in tests\n"
        "line 2\n"
        "line 3\n"
        "qc now supports shebang. Because bash's shebang support will not allow\n"
        "the script interpret to be another script (in our case, that would be\n"
        "qc itself), the syntax is:\n"
        "\n"
        "#!/bin/bash /home/philippe/bin.v2/qc\n"
        "\n"
        "last line\n"};

    utAssertTrue( o == expect );
    utAssertTrue( p == expect );

    o = exec("./sample_source").ostring()();

    utAssertTrue( o == expect );
}

void feed_stdin()
{
    QString o;
    exec("grep secret").istring(secret_text).ostring(&o)();

    QString expect {
        "the secret is here\n"
        "and also a secret here\n"};

    utAssertTrue( o == expect );
}

void pipe()
{
    QString o;
    o = exec("./sample_source").pipe("grep line").pipe("wc").ostring()();

    utAssertTrue( o == "      4      14      64\n" );
}

void file_input()
{
    QTemporaryFile i;

    QString iname = create_tmp_file(secret_text, i);

    QString o;
    o = exec("grep secret").ifile(iname).ostring()();

    QString expect {
        "the secret is here\n"
        "and also a secret here\n"};

    utAssertTrue( o == expect );
}

void file_output()
{
 //   QString fname = ;

   // exec("grep secret").istring(secret_text).ofile(fname)();
}

void cases_without_external_script()
{
    capture_stdout();
    feed_stdin();
    pipe();
    file_input();
    file_output();
}

void use_stdin()
{
    exec("grep secret")();
}

int main(int argc, char ** argv)
{
    if( argc != 2 ) exit(1);

    QString arg = argv[1];

         if( arg == "simple_exec"                   ) simple_exec();
    else if( arg == "start_failure"                 ) start_failure();
    else if( arg == "cases_without_external_script" ) cases_without_external_script();
    else if( arg == "use_stdin"                     ) use_stdin();

    return 0;
}

