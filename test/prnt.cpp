
#include <QCoreApplication>
#include <QPrinterInfo>
#include <QPrinter>
#include <QTextStream>

#include <boost/foreach.hpp>

QTextStream sout(stdout);

QTextStream & operator << ( QTextStream & os, QRect const & r )
{
    os << r.left() << ' ';
    os << r.top() << ' ';
    os << r.width() << ' ';
    os << r.height();

    return os;
}

int main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);

    sout << "running\n";
    
    QPrinterInfo lzr;

    BOOST_FOREACH(QPrinterInfo pi, QPrinterInfo::availablePrinters())
    {
        sout << pi.printerName() << '\n';
        if( pi.printerName() == "Noir & Blanc PS" ) lzr = pi;
    }
    
    QPrinter p(lzr, QPrinter::HighResolution);
    QRect paper = p.paperRect();
    QRect page = p.pageRect();
    sout << paper << '\n';
    sout << page << '\n';
    
    return 0;
}
