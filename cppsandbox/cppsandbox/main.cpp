#include <iostream>

using namespace std;

void printwchar(const wchar_t** texts)
{
	for (int i = 0; i < 2; i++)
	{
		wcout << texts[i] << endl;
	}
}

int main()
{
	const wchar_t* texts[] = { L"hello", L" lover" };
	printwchar(texts);
}