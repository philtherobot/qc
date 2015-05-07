#!/bin/bash /home/philippe/bin.v2/qc

#pragma qc_import(../../lib/exec)
#pragma qc_import(../../lib/unit_test)
#pragma qc_import(../../lib/defaultio)

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include <QString>
#include <QTemporaryFile>
#include <QTextStream>

using namespace qc;


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
    exec("./sample_source").ostring(&o)();

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
}

void capture_stderr()
{
    QString o;
    QString e;
    exec("./error_script").estring(&e).ostring(&o)();

    QString o_expect{
        "This is on stdout\n"
        "Back on stdout\n"};

    utAssertTrue( o == o_expect );

    QString e_expect{
        "This is on stderr\n"
        "On stderr again\n"};

    utAssertTrue( e == e_expect );
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
    exec("./sample_source").pipe("grep line").pipe("wc").ostring(&o)();

    utAssertTrue( o == "      4      14      64\n" );
}

void file_input()
{
    QTemporaryFile i;

    QString iname = create_tmp_file(secret_text, i);

    QString o;
    exec("grep secret").ifile(iname).ostring(&o)();

    QString expect {
        "the secret is here\n"
        "and also a secret here\n"};

    utAssertTrue( o == expect );
}

void file_output()
{
	QString fname;
	{
		QTemporaryFile tf;
		tf.open(); tf.close();

		fname = tf.fileName();
	}

    exec("grep secret").istring(secret_text).ofile(fname)();

    QFile fl(fname);
    fl.open(QIODevice::ReadOnly);
    QString o = fl.readAll();

    QString expect {
        "the secret is here\n"
        "and also a secret here\n"};

    utAssertTrue( o == expect );
}

void file_error()
{
	QString ofname;
	{
		QTemporaryFile tf;
		tf.open(); tf.close();

		ofname = tf.fileName();
	}

	QString efname;
	{
		QTemporaryFile tf;
		tf.open(); tf.close();

		efname = tf.fileName();
	}

    exec("./error_script").efile(efname).ofile(ofname)();

    QFile ofl(ofname);
    ofl.open(QIODevice::ReadOnly);
    QString o = ofl.readAll();

    QString o_expect {
        "This is on stdout\n"
        "Back on stdout\n"};

    utAssertTrue( o == o_expect );

    QFile efl(efname);
    efl.open(QIODevice::ReadOnly);
    QString e = efl.readAll();

    QString e_expect {
        "This is on stderr\n"
        "On stderr again\n"};

    utAssertTrue( e == e_expect );
}

void exit_code()
{
	utAssertTrue( 0 == exec("true")() );
	utAssertTrue( 1 == exec("false")() );
}

void cases_without_external_script()
{
	start_failure();
	capture_stdout();
    capture_stderr();
    feed_stdin();
    pipe();
    file_input();
    file_output();
    file_error();
    exit_code();
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
    else if( arg == "cases_without_external_script" ) cases_without_external_script();
    else if( arg == "use_stdin"                     ) use_stdin();

    return 0;
}

