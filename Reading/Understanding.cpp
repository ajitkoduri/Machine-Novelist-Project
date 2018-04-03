// Understanding.cpp : Application to build understanding based on story read
#include <Magick++.h>
//using Reading.h file to read story and generate graph.
#include "Reading.h"

//have to make infinitives, gerunds, participles, and contractions databases as well.
int main()
{
	//sets the ascii values correctly.
	
	std::locale::global(std::locale(""));
		
	string story_text;
	string line;
	ifstream f("ThreeLittlePigs.txt");
	if (f.is_open())
	{
		while (getline(f, line))
		{
			line += "\n";
			story_text += line;
		}
	}
	//building a story out of the text that has been read
	Story S(story_text);
	return 0;
}
