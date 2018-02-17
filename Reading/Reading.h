#pragma once
#include "PoS_Labeller.h"
#include <queue>
#include <set>
#include <stack>
using namespace std;

//container for punctuation normally present in a text
set<char> punctuation = { '.' , ';' , ',' , '?' , '!', '$', '(', ')', '*', '&', '%', ':', '[',']','{', '}' };

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
		{
			cout << names[i] << endl;
			return true;
		}
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
	//switch to say that the clause is a complete one
	bool complete;
	bool indep;
	bool run_on;
	vector <vector <string> > Saved_PoS_Label;
	int switch_index;

	Clause() {};

	Clause(string s)
	{
		clause_statement = s;
		Read(s);
	};
	//list of all unprocessed words
	vector <Graph_Word*> unprocessed_words;
	//queue for adjective words about to be processe
	stack <Graph_Word*> stored_words_adj;

	//queue for adverb words about to be processed
	queue <Graph_Word*> stored_words_adv;

	//queue for prepositions in the clause
	queue <Graph_Word*> prep_in_clause;

	//Graph of the clause
	Graph_Clause clause;

	//a module to process the string to make it easier to tokenize each word.
	void Process_Text(const string& S)
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
	}

	void Process_Graph();
	void Make_Graph();

	//function to label each element of the text by the correct part of speech and then to fill up the containers for the
	//subject, objects, actions, adjectives, and adverbs in the clause.
	void Read(const string& original_text)
	{
		
		complete = false;
		//learn the words from the database
		Learn();

		//Process the text as the clause statement
		Process_Text(original_text);

		Label_Text_PoS(clause_statement);

		//listing out the unprocessed words and making them nodes in the graph for a word
		for (int index = 0; index < tokens.size(); index++)
		{
			Graph_Word* unproc_word = new Graph_Word(tokens[index]);
			unprocessed_words.push_back(unproc_word);
		}
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
	//switch to see if the objects are finished listing.
	bool obj_full = false;
	//switch to see if a prepositional phrase is there or not
	bool prep_avail = false;
	run_on = false;
	//switches to control whether the sentence is listing a series of words or not.
	bool n_is_listing = false;
	bool n_end_list = false;

	bool v_is_listing = false;
	bool v_end_list = false;
	bool v_ended = false;

	//w_ind is the word index (saving room for clarity)
	for (int w_ind = 0; w_ind < unprocessed_words.size(); w_ind++)
	{
		//here, we look at if the text can be an article first, then preposition, then an adverb, then an adjective, then a noun, then a verb.
		//The reasoning for that is that's the usual way a sentence is made.
		cout << w_ind << " - " << unprocessed_words[w_ind]->name << " : ";

		//if the text is an article
		if (contains(tokens_PoS_Label[w_ind], "article"))
		{
			tokens_PoS_Label[w_ind].clear();
			tokens_PoS_Label[w_ind].push_back("article");
			//simply store the word till the noun arrives.
			stored_words_adj.push(unprocessed_words[w_ind]);
		}

		//if the text could be a preposition
		if (contains(tokens_PoS_Label[w_ind], "prep"))
		{
			//we set the availability of being a preposition true and push it into a preposition queue.

			//for the case that the preposition is misclassified, we save its index and all its possible parts of speech.
			Saved_PoS_Label.push_back(tokens_PoS_Label[w_ind]);
			switch_index = w_ind;
			prep_avail = true;
			prep_in_clause.push(unprocessed_words[w_ind]);

			//if there is a noun previous to it, such as 'the Queen of All', it describes that noun. We can make sure that
			//the program doesn't look at the -1st element by asking if that word even exists.
			if (w_ind != 0 && contains(tokens_PoS_Label[w_ind - 1], "noun"))
			{
				unprocessed_words[w_ind - 1]->descriptors.push_back(unprocessed_words[w_ind]);
			}

			//otherwise, the preposition describes a noun
			else
			{
				//if the preposition could also be named an adverb, that means that the preposition describes the action.
				//For instance, 'in time you will agree', 'in time' describes 'will agree'. And in another case, 'Turn in your badge',
				//'in your badge' also describes the action 'turn' because it tells where to turn. These two cases tell me that there is
				//not location-specific in the text, it's a quality of the type of word that could be a preposition.
				if (contains(tokens_PoS_Label[w_ind], "adv"))
				{
					stored_words_adv.push(unprocessed_words[w_ind]);
				}
				//in the case of something like "Of all the girls, I love her the most."
				else
				{
					stored_words_adj.push(unprocessed_words[w_ind]);
				}
			}
		}

		//if the text is an adverb or if it is a modal verb
		if (contains(tokens_PoS_Label[w_ind], "adv") || contains(tokens_PoS_Label[w_ind], "modal_verb"))
		{
			tokens_PoS_Label[w_ind].clear();
			tokens_PoS_Label[w_ind].push_back("adv");
			//in the case the following word is also an adverb, just append the adverb as a modifier to the next adverb.
			if (contains(tokens_PoS_Label[w_ind + 1], "adv"))
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
			}
			//if the following word is an adjective, then it modifies the adjective.
			else if (contains(tokens_PoS_Label[w_ind + 1], "adj"))
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
			}
			//if the following word is a verb, then it must modify that verb.
			else if (contains(tokens_PoS_Label[w_ind + 1], "verb"))
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
			}
			else
			{
				stored_words_adv.push(unprocessed_words[w_ind]);
			}
		}
		
		//if the text is an adjective
		if (contains(tokens_PoS_Label[w_ind], "adj"))
		{
			tokens_PoS_Label[w_ind].clear();
			tokens_PoS_Label[w_ind].push_back("adj");
			//in the case the following word is also an adverb, just append the adverb as a modifier to the next adverb.
			if (contains(tokens_PoS_Label[w_ind + 1], "noun"))
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
			}
			else
			{
				stored_words_adj.push(unprocessed_words[w_ind]);
			}
		}
		
		//if there is a preposition or adjectives upcoming, we can assume the verb case should not be picked.
		if (!prep_avail && stored_words_adj.empty())
		{
			//if the text is a verb
			if (contains(tokens_PoS_Label[w_ind], "verb"))
			{
				tokens_PoS_Label[w_ind].clear();
				tokens_PoS_Label[w_ind].push_back("verb");
				if (v_end_list)
					v_is_listing = false;
				else
					v_is_listing = true;

				if (!stored_words_adj.empty() && clause.subj.empty())
				{
					if (contains(Saved_PoS_Label.back(), "prep"))
					{
						prep_avail = false;
						while (!stored_words_adj.empty())
						{
							stored_words_adj.pop();
						}
						tokens_PoS_Label[switch_index].clear();
						tokens_PoS_Label[switch_index].push_back("sub_con");
						Make_Graph();
						return;
					}
				}

				if (clause.subj.empty())
				{
					clause.subj.push_back(Graph_Word("you"));
				}

				if (v_is_listing && v_ended)
				{
					cout << "run on!" << endl;
					complete = false;
					run_on = true;
					return;
				}
				else
				{
					//append it into the clause graph
					clause.verbs.push_back(*unprocessed_words[w_ind]);
					v_ended = true;
				}
			}
		}

		//if the text is a noun
		if (contains(tokens_PoS_Label[w_ind], "noun"))
		{
			tokens_PoS_Label[w_ind].clear();
			tokens_PoS_Label[w_ind].push_back("noun");

			if (contains(tokens_PoS_Label[w_ind + 1], "verb"))
			{
				prep_avail = false;
			}

			//append it into the clause graph. This will be ignoring appositives.
			if (clause.verbs.empty() && !prep_avail)
			{
				clause.subj.push_back(*unprocessed_words[w_ind]);
				while (!stored_words_adj.empty())
				{
					unprocessed_words[w_ind]->descriptors.push_back(stored_words_adj.top());
					stored_words_adj.pop();
				}
			}
			//now, to check if it is an indirect or direct object. We can tell the difference by simply checking if the word following
			//the last indirect object is not a conjunction or a comma. 'The cat gave the dog, the mouse, and the person a fright.' The
			//indirect objects here were dog, mouse, and person and you can tell that because the word following 'person' is not continuing
			//the listing of direct objects. 
			else if (!prep_avail)
			{
				clause.noun.push(*unprocessed_words[w_ind]);
				//if it's the last word in the sentence, it has to be a direct object, so just put it into the noun queue list.
				if (w_ind == unprocessed_words.size() - 1)
				{
					break;
				}

				//as long as it's not connected to a conjunction/comma, it is fine.
				else if (!contains(tokens_PoS_Label[w_ind + 1], "conj") && unprocessed_words[w_ind + 1]->name != ",")
				{
					//fill the noun queue into the objects.
					while (!clause.noun.empty() && !obj_full)
					{
						clause.obj1.push_back(clause.noun.top());
						clause.noun.pop();
					}
					obj_full = true;
				}
			}
			//if the noun is currently in a preposition
			else if (prep_avail)
			{
				if (n_end_list)
					n_is_listing = false;
				else
					n_is_listing = true;
				cout << "prep object" << endl;
				//add all queue-stored adjectives and articles as its modifiers
				while (stored_words_adj.size() != 1)
				{
					unprocessed_words[w_ind]->descriptors.push_back(stored_words_adj.top());
					stored_words_adj.pop();
				}
				//fill the nouns as objects for the preposition
				prep_in_clause.front()->descriptors.push_back(unprocessed_words[w_ind]);
				//check if there are multiple objects for this preposition by checking if there is a listing in process
				//if the list is done, we can terminate the preposition. If it isn't, just assume the prepositional phrase continues
				if (!n_is_listing)
				{
					prep_avail = false;
					prep_in_clause.pop();
				}
			}
		}

		//if the text is a conjunction, it is the end of the list, so the next noun/verb in the list can be treated as such.
		if (contains(tokens_PoS_Label[w_ind], "conj"))
		{
			//dealing with and without oxford comma.
			if (contains(tokens_PoS_Label[w_ind - 1], "verb") || (contains(tokens_PoS_Label[w_ind - 2], "noun") && unprocessed_words[w_ind - 1]->name == ","))
			{
				v_ended = true;
				v_end_list = true;
			}
			if (contains(tokens_PoS_Label[w_ind - 1], "noun") || (contains(tokens_PoS_Label[w_ind - 2], "noun") && unprocessed_words[w_ind-1]->name == ","))
			{
				n_end_list = true;
			}
		}
	}
	
	Process_Graph();
}

void Clause::Process_Graph()
{
	if (!clause.subj.empty() && !clause.verbs.empty() && Union<char>(punctuation,unprocessed_words.back()->name[0]) == punctuation)
	{
		complete = true;
	}

	if (contains(tokens_PoS_Label.back(), "conj") || contains(tokens_PoS_Label.back(), "adj"))
	{
		complete = false;
		return;
	}
	//if there are prepositions still not completed, we should interpret them as adverbs describing all the verbs.
	while (!prep_in_clause.empty())
	{
		stored_words_adv.push(prep_in_clause.front());
		prep_in_clause.pop();
	}
	//all the remaining nouns in the text to be processed are the 2nd layer of objects.
	while (!clause.noun.empty())
	{
		clause.obj2.push_back(clause.noun.top());
		clause.noun.pop();
	}
	//in the case that the only object was the direct object, all we have to do is move the initially processed as indirect objects 
	//to the direct ones.
	if (clause.obj2.empty())
	{
		clause.obj2 = clause.obj1;
		clause.obj1.clear();
	}

	//if the sentence only contains a subject and verb scenario
	if (clause.obj2.empty())
	{
		//if there are no objects, the subjects refer back to themselves (they receives their own action)
		for (int subjects = 0; subjects < clause.subj.size(); subjects++)
			for (int verb = 0; verb < clause.verbs.size(); verb++)
				clause.actions.push_back(Edge(clause.subj[subjects], clause.subj[subjects], clause.verbs[verb]));
	}
	cout << clause.subj.size() << "," << clause.verbs.size() << "," << clause.obj1.size() << "," << clause.obj2.size() << endl;

	//if the sentence only contains a subject and verb scenario
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
			//if there are indirect objects, the sentence can be rewritten as subjects...verb...direct objects... "to"-indirect objects.
			//in that case, 'to' is an adverb, so we should treat the indirect object as an adverbial prepositional phrase.
			for (int i_obj = 0; i_obj < clause.obj1.size(); i_obj++)
			{
				//we change the phrase of the indirect object to a 'to X' analog and treat that as an adverb for the actions. 
				Graph_Word* connect = new Graph_Word("to");
				connect->descriptors.push_back(&clause.obj1[i_obj]);
				//append it to the list of adverbs
				stored_words_adv.push(connect);
			}
		}
	}

	//add all queue-stored adverbs as modifiers for every verb
	while (!stored_words_adv.empty())
	{
		for (int verb = 0; verb < clause.verbs.size(); verb++)
		{
			clause.verbs[verb].descriptors.push_back(stored_words_adv.front());
		}
		stored_words_adv.pop();
	}

}

/*
Sentence structure, will contain multiple clauses.
*/
struct Sentence : public Clause
{
	string text;
	vector <Clause> Clauses;
	queue <Clause> Q_Clauses;
	Sentence() {}
	Sentence(string s)
	{
		text = s;
	}

	//function to split text into clauses.
	void split()
	{
		Learn();
		Process_Text(text);
		Label_Text_PoS(clause_statement);

		for (int index = 0; index < tokens.size(); index++)
		{
			Graph_Word* unproc_word = new Graph_Word(tokens[index]);
			unprocessed_words.push_back(unproc_word);
		}
		for (int index = 0; index < unprocessed_words.size(); index++)
		{

		}
	}
};