#include <iostream>
#include <fstream>

int main()
{
	std::ofstream fout("input.txt");
	int N = 9 * 16;
	fout << N << std::endl;
	for(int i(0); i < N; ++i)
	{
		fout << 9;
	}
	fout << std::endl;
	for(int i(0); i < N; ++i)
	{
		fout << 2;
	}
	fout.close();
}
