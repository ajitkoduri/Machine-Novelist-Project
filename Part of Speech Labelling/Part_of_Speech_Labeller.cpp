// Part_of_Speech_Labeller.cpp : Defines the entry point for the console application.
//

#include "PoS_Labeller.h"

//driver function
int main()
{
	Vocabulary Toddler;
	Toddler.Learn();
	string sentence = "the car will have been blue";
	
	Toddler.Label_Text(sentence);
    return 0;
}

