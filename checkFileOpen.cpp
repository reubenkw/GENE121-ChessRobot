#include <fstream>
#include <cstdlib>
#include <iostream>

using namespace std;
void whenFileUpdated()
{
	bool open = false;
	int update = 0;
	do 
	{
		try
		{
			ifstream fin("test.txt");
			if(!fin)
				open = true;
			if(!open)
			{
				fin >> update;
				fin.seekg(0, ios::beg);
				if(update == 1)
				{
					fin.close();
					ofstream fout("test.txt");
					fout << 0;
							
				}
			}
		}
		catch(int e)
		{
			open = true;
		}
	}while(!open && update == 1);
}

int main()
{
	whenFileUpdated();
	 
	system("PAUSE");
	return EXIT_SUCCESS;
	
}

