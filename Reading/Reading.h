#pragma once
#include "PoS_Labeller.h"
#include <queue>
#include <set>
#include <stack>

using namespace std;

//nouns can be subjects or objects. Verbs connect subjects to an object or connect the subject to itself.
//Adjectives modify nouns. Adverbs can modify verbs, adjectives, or other adverbs.

struct Graph_Word
{
	//edge of graph
	string name;

	//vertices of graph
	vector <Graph_Word*> descriptors;

	Graph_Word(string n) : name(n) {};
};

struct Edge
{
	Graph_Word sourc;
	Graph_Word dest;
	Graph_Word action;

	Edge(Graph_Word s, Graph_Word d, Graph_Word a) : sourc(s), dest(d), action(a) {};
};

struct Graph_Clause
{
	//vertices of graph
	stack <Graph_Word> noun;
	//first vertices of the graph
	vector <Graph_Word> subj;
	//edges of the graph
	vector <Graph_Word> verbs;
	//2nd vertices of graph
	vector <Graph_Word> obj1;
	//3rd vertices of graph
	vector <Graph_Word> obj2;

	//creation of edges for graph
	vector <Edge> actions;
};

//function to determine whether a container holds a specific value
bool contains(vector <string> names, string key)
{
	//if it contains the key, return true, otherwise return false.
	for (int i = 0; i < names.size(); i++)
		if (names[i].find(key) != string::npos)
			return true;
	return false;
}

//union function between a set and a new member
template <typename S>
set<S> Union(set<S> s1, S new_s)
{
	set<S> result = s1;
	result.insert(new_s);
	return result;
}

struct Clause : public Vocabulary
{
	//statement to be read
	string clause_statement;

	//list of all unprocessed words
	vector <Graph_Word*> unprocessed_words;
	//queue for words about to be processed
	queue <Graph_Word*> stored_words;
	queue <vector <string> > stored_words_PoS;

	//Graph of the clause
	Graph_Clause clause;

	//container for punctuation
	set<char> punctuation = { '.' , ';' , ',' , '?' , '!', '$', '(', ')', '*', '&', '%', ':', '[',']','{', '}' };

	//a module to process the string to make it easier to tokenize each word.
	string Process_Text(const string& S)
	{
		//put spaces between the words with punctuation to make it easier for the tokens to be processed
		string processed_text;

		//copy the string to be processed
		string copy = S;
		for (int string_index = 0; string_index < copy.size(); string_index++)
		{
			//if there is a punctuation next to a word, append a space
			if (Union<char>(punctuation, copy[string_index]) == punctuation)
			{
				processed_text += " ";
			}

			//convert all the strings to lower case for ease of processing the tokens
			copy[string_index] = tolower(copy[string_index]);
			processed_text += copy[string_index];

			//if there is not a space after the punctuation, append one
			if ((Union<char>(punctuation, copy[string_index]) == punctuation) && (copy[string_index + 1] != ' '))
			{
				processed_text += " ";
			}
		}

		clause_statement = processed_text;
		return processed_text;
	}

	void Process_Graph();
	void Make_Graph();

	//function to label each element of the text by the correct part of speech and then to fill up the containers for the
	//subject, objects, actions, adjectives, and adverbs in the clause.
	void Read(const string& original_text)
	{
		//learn the words from the database
		Learn();

		//Process the text as the clause statement
		clause_statement = Process_Text(original_text);


		Label_Text_PoS(clause_statement);

		Make_Graph();
	}
};

/*---------------------------------------------------------------------------------------------------------------
the purpose of this function is to connect words to one another. In this case, I'll be running a greedy algorithm
since that would probably be the best and most organic way of reading a list of terms in the English language.
If you think about how you're reading these sentences, it's probably not based off the parts of speeches entirely,
but also based on how the words themselves are placed. For instance, take the sentence "Make a turn at the turn signal."
Logically speaking, the word 'turn' arises twice, but you instantly know the difference between the two locations.

But, how did you know? 'Turn' can be read as a verb, noun, or adjective depending on the connotation of the sentence.
But in terms of a human reading it, it's easy to presume what type of word it is, as long as we know some critical portion
of the sentence to make some guesses about what 'turn' was in this sense. 'The turn turns at the turn signal' might be a
tongue twister, but it is palatable as an English sentence. Your inferences on that sentence can easily be amounted to you
basing your opinion of the words around it. Later on, I'll add a re-read function to help make more sense out of grammatical
errors, but for now, we need to teach it proper English so it can know what to be loose around later on.

So, back to this function. This function will connect adverbs to adverbs, adjectives, and verbs, adjectives to nouns, and
conjunctions will be used to create extra appendages to the word. 'red and blue flower' should ignore the 'and', but connect
'red' and 'blue' to 'flower'. This is pretty good for our purposes because adjectives, adverbs, nouns, and verbs have places
they need to be for them to be sensible. If the adverb is at the beginning or end of a clause, it modifies the verb, but if it
is behind an adverb, the verb itself, or an adjective, it will modify them. The situations where it doesn't do not exist - the
only sensible way for us to read is to assume the modifiers follow the word they modify.

Choosing subject-verb clarity can be done as the last step, as usually it is meaningless in the English language since verb syntax
often overlaps between all cases. Process the tokens by their part of speech as they come. The goal here is to excise extra labels
for words in the clause,removing words that have been classified more than once.
----------------------------------------------------------------------------------------------------------------*/

void Clause::Make_Graph()
{
	//queue for adverb words about to be processed
	queue <Graph_Word*> stored_words_adv;
	queue <vector <string> > stored_words_PoS_adv;
	bool is_listing = false;

	//listing out the unprocessed words and making them nodes in the graph for a word
	for (int index = 0; index < tokens.size(); index++)
	{
		Graph_Word* unproc_word = new Graph_Word(tokens[index]);
		unprocessed_words.push_back(unproc_word);
		cout << unproc_word->name << endl;
	}

	//w_ind is the word index (saving room for clarity)
	for (int w_ind = 0; w_ind < unprocessed_words.size() - 1; w_ind++)
	{
		//here, we look at if the text can be an article first, then an adverb, then an adjective, then a noun, then a verb.
		//The reasoning for that is that's the usual way a sentence is made.

		//if the text is an article
		if (contains(tokens_PoS_Label[w_ind], "article"))
		{
			//if the penultimate word is an article, the last word is a noun for sure.
			if (w_ind == unprocessed_words.size() - 2)
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
				cout << "connecting " << unprocessed_words[w_ind]->name << " to " << unprocessed_words[w_ind + 1]->name << endl;
				//append the noun into the clause graph
				clause.noun.push(*unprocessed_words[w_ind + 1]);
				cout << "new noun added to clause: " << unprocessed_words[w_ind + 1]->name << endl;
			}

			else
			{
				//simply store the word till the noun arrives.
				stored_words.push(unprocessed_words[w_ind]);
				stored_words_PoS.push(tokens_PoS_Label[w_ind]);
			}
		}
		//if the text is an adverb
		else if (contains(tokens_PoS_Label[w_ind], "adv"))
		{
			//in the case the following word is also an adverb, just append the adverb as a modifier to the next adverb.
			if (contains(tokens_PoS_Label[w_ind + 1], "adv"))
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
				cout << "connecting " << unprocessed_words[w_ind]->name << " to " << unprocessed_words[w_ind + 1]->name << endl;
			}
			//if the following word is an adjective, then it modifies the adjective.
			else if (contains(tokens_PoS_Label[w_ind + 1], "adj"))
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
				cout << "connecting " << unprocessed_words[w_ind]->name << " to " << unprocessed_words[w_ind + 1]->name << endl;
			}
			//if the following word is a verb, then it must modify that verb.
			else if (contains(tokens_PoS_Label[w_ind + 1], "verb"))
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
				cout << "connecting " << unprocessed_words[w_ind]->name << " to " << unprocessed_words[w_ind + 1]->name << endl;
			}
			else
			{
				stored_words_adv.push(unprocessed_words[w_ind]);
				stored_words_PoS_adv.push(tokens_PoS_Label[w_ind]);
			}
		}
		//if the text is an adjective
		else if (contains(tokens_PoS_Label[w_ind], "adj"))
		{
			//in the case the following word is also an adverb, just append the adverb as a modifier to the next adverb.
			if (contains(tokens_PoS_Label[w_ind + 1], "noun"))
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
				cout << "connecting " << unprocessed_words[w_ind]->name << " to " << unprocessed_words[w_ind + 1]->name << endl;
			}
			else
			{
				stored_words.push(unprocessed_words[w_ind]);
				stored_words_PoS.push(tokens_PoS_Label[w_ind]);
			}
		}
		//if the text is a noun
		else if (contains(tokens_PoS_Label[w_ind], "noun"))
		{
			//add all queue-stored adjectives and articles as its modifiers
			while (!stored_words.empty())
			{
				cout << "connecting " << stored_words.front()->name << " to " << unprocessed_words[w_ind]->name << endl;
				unprocessed_words[w_ind]->descriptors.push_back(stored_words.front());
				stored_words.pop();
				stored_words_PoS.pop();
			}

			//append it into the clause graph
			if (clause.verbs.empty())
			{
				clause.subj.push_back(*unprocessed_words[w_ind]);
				cout << "new subject added to clause: " << unprocessed_words[w_ind]->name << endl;
			}
			else
			{
				clause.noun.push(*unprocessed_words[w_ind]);
				cout << "new noun added to clause: " << unprocessed_words[w_ind]->name << endl;
			}
		}
		//if the text is a verb
		else if (contains(tokens_PoS_Label[w_ind], "verb"))
		{
			//add all queue-stored adverbs as modifiers
			while (!stored_words_adv.empty())
			{
				cout << "connecting " << stored_words_adv.front()->name << " to " << unprocessed_words[w_ind]->name << endl;
				unprocessed_words[w_ind]->descriptors.push_back(stored_words_adv.front());
				stored_words_adv.pop();
				stored_words_PoS_adv.pop();
			}
			//append it into the clause graph
			clause.verbs.push_back(*unprocessed_words[w_ind]);
			clause.noun.push(*unprocessed_words[w_ind]);
			cout << "new verb added to clause: " << unprocessed_words[w_ind]->name << endl;
		}
	}

	Process_Graph();
}

void Clause::Process_Graph()
{
	//process the nouns.
	clause.obj2.push_back(clause.noun.top());
	cout << "the direct object is: " << clause.noun.top().name << endl;

	clause.noun.pop();

	if (!clause.noun.empty())
	{
		clause.obj1.push_back(clause.noun.top());
		cout << "the indirect object is: " << clause.noun.top().name << endl;
	}

	if (clause.obj2.empty())
	{
		//if there are no objects, the subjects refer back to themselves (they receives their own action)
		for (int subjects = 0; subjects < clause.subj.size(); subjects++)
			for (int verb = 0; verb < clause.verbs.size(); verb++)
				clause.actions.push_back(Edge(clause.subj[subjects], clause.subj[subjects], clause.verbs[verb]));
	}

	//in the case there are nouns that could be direct or indirect, we need to be a bit more clever.
	else
	{
		//the indirect object always precedes the direct object, so it really comes down to whether or not the noun was added in a list
		//or if the nouns are separate, but the direct object always comes at the end. In these cases, we need to rely on the punctuation 
		//and context.

		//I'm going to ignore lists (and therefore punctuation) for right now, but we will get back to it. For now, it'll just be a shortcut
		//of whether or not two nouns can be objects or not.

		for (int subjects = 0; subjects < clause.subj.size(); subjects++)
			for (int verb = 0; verb < clause.verbs.size(); verb++)
				for (int d_obj = 0; d_obj < clause.obj2.size(); d_obj++)
					clause.actions.push_back(Edge(clause.subj[subjects], clause.obj2[d_obj], clause.verbs[verb]));

		if (!clause.obj1.empty())
		{
			//all the direct objects will be modifiers of the indirect objects (not sure how well this is going to handle it)
			for (int d_obj = 0; d_obj < clause.obj2.size(); d_obj++)
				for (int i_obj = 0; i_obj < clause.obj1.size(); i_obj++)
					clause.obj1[i_obj].descriptors.push_back(&clause.obj2[d_obj]);
		}
	}

}
