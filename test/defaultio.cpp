
#pragma qc_import(../lib/defaultio)

using namespace qc;

int main()
{
	int number;
	din >> number;
	dout << "You typed " << number << ".\n";
}
