#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;


//  I'm unsure of how many words the computer will need to remember, so I'm going to save each node
//  of the trie as a character of a larger string. It's equivalent to saving each letter of a word.
struct Node
{
	char data;								//The character will be saved as the data inside the node

	vector <Node*> children;				//The successors of this particular node will be saved under
											//as a child of the node. In this case, I'll save the kids as
											//array of all possible 

	Node(char c) : data(c) {};				//Initializing a node with a set character gives it
											//data but no children

	Node() : data('\0') {};					//Initializing a node with nothing gives it a * for
											//for the data and no children

	void operator= (Node n)					//an = operator that sets two nodes equal to one another
	{
		data = n.data;						//sets the data of one to the other
		children.clear();												//this section clears the initial childre
		for (int child_num = 0; child_num < n.children.size(); child_num++)		//and then copies the new node's children
			children[child_num] = n.children[child_num];
	}

	void operator= (char c)					//operator that sets the data of a specific node to specified character
	{
		data = c;							//setting the data equal to the specified character
	}

	bool operator== (char c)				//operator that checks the data against a specific character (EQUAL)
	{
		if (data == c)
			return true;
		else
			return false;
	}

	bool operator!= (char c)				//operator that checks the data against a specific character (NOT EQUAL)
	{
		if (data == c)
			return false;
		else
			return true;
	}
};

//The trie data structure
class Trie
{
private:									//parts here should not be changed, else the trie might break 

	Node root;								//the root node of the Trie, all words are branches of this node

	vector <string> stored_words;				//I'm going to save all the words in the trie separately as well
											//since I don't think there is an easy way to list out all the words
											//saved in the trie.

public:										//methods here can be changed and called from the outside

	Trie()									//constructor method
	{
		root = Node('\0');
	}

	vector <string> words()					//method to create a copy vector container for the words stored in the trie
	{
		return stored_words;				//will let the computer call and copy those words and play with them without
											//destroying the stored list of words in the trie
	}

	void add(string str)							//Method to add a string to the trie
	{
		stored_words.push_back(str);			//adding the string to the list of stored words

		str = str + "&";					//setting an end character to the character '&'. This is in the case that
											//a word is saved inside another word but is inaccessible, e.g. the words
											//'world' and 'worldly'. While 'world' and 'worldly' are certainly words, 
											//I don't want the computer to not be able to say whether 'worldl' is not
											//a word.

		Node* node = &root;				//This node will look at the trie starting from the root, and add extra nodes
										//as needed.

		for (int str_index = 0; str_index < str.length(); str_index++)					//iterating through the string
		{
			bool found = false;				//switch to determine whether the node's data matches the string's character

			for (int child_index = 0; child_index < node->children.size(); child_index++)	//iterating through kids of nodes
			{

				if (node->children[child_index]->data == str[str_index])
				{
					node = node->children[child_index];			//if there is a match for the node, then move the node to the
																//matching child node.
					found = true;
				}
			}

			if (!found)
			{
				Node* new_node = new Node(str[str_index]);						//if the character is not found, we'll add a new node
				node->children.push_back(new_node);								//to the list of kids to this node
				node = node->children.back();
			}
		}
	}

	bool find(string str)				//method to find string in trie. Similar to adding, only we ignore that step.
	{
		str = str + "&";					//adding end character '&' to the string.

		Node* node = &root;				//This node will look at the trie starting from the root, and add extra nodes
										//as needed.

		for (int str_index = 0; str_index < str.length(); str_index++)							//iterating through the string
		{
			bool found = false;				//switch to determine whether the node's data matches the string's character

			for (int child_index = 0; child_index < node->children.size(); child_index++)	//iterating through kids of nodes
			{
				if (*node->children[child_index] == str[str_index])
				{
					node = node->children[child_index];			//if there is a match for the node, then move the node to the
																//matching child node.
					found = true;
				}
			}

			if (!found)
				return false;				//if you can't find the node, end the search
		}


		return true;					//if it has found all the characters in the string in the correct order, it will return true
	}

	void read_all()					//method to read all the strings saved inside the trie
	{
		for (int stored_word_index = 0; stored_word_index < stored_words.size(); stored_word_index++)
			cout << stored_words[stored_word_index] << endl;
	}
};
