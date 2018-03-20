#pragma once
#include "PoS_Labeller.h"
#include <queue>
#include <set>
#include <stack>
using namespace std;

//container for punctuation normally present in a text
set<char> punctuation = { '.' , ';' , ',' , '?' , '!', '$', '(', ')', '*', '&', '%', ':', '[',']','{', '}' };

//static members of vocabulary structure imported into this header file.
csvreader Vocabulary::reader;
vector<Trie> Vocabulary::verbs_1_s;
vector<string> Vocabulary::verbs_header;
vector<Trie> Vocabulary::verbs_2_s;
vector<Trie> Vocabulary::verbs_3_s;
vector<Trie> Vocabulary::verbs_1_p;
vector<Trie> Vocabulary::verbs_2_p;
vector<Trie> Vocabulary::verbs_3_p;
Trie Vocabulary::nouns_s;
Trie Vocabulary::nouns_p;
Trie Vocabulary::prepositions;
Trie Vocabulary::social;
Trie Vocabulary::adjectives;
Trie Vocabulary::adverbs;
vector<Trie> Vocabulary::pronouns;
Trie Vocabulary::conjunctions;
Trie Vocabulary::modal_verbs;
Trie Vocabulary::articles;
Trie Vocabulary::subjunctive_conjunctions;
map <string, Trie> Vocabulary::Words;

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
	void clear();
};

//method to clear a clause
void Graph_Clause::clear()
{
	while (!noun.empty())
	{
		noun.pop();
	}
	subj.clear();
	verbs.clear();
	obj1.clear();
	obj2.clear();
}

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
	bool complete = false;
	bool indep;
	bool run_on = false;
	vector <vector <string> > Saved_PoS_Label;
	int switch_index;

	/*
	----------------------------------------------------------------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------------------------------------------------------------
	we have multiple switches that activate in order to tell if a list of nouns/verbs is being made or if a comma signifies something else.
	this is a common problem in every day language where the comma can only be accurately identified only after the sequence of events is done!
	In this case, n_is_listing is a switch that describes if the computer should look out for another noun or not. n_end_list is a switch
	that puts a cap on the noun list, saying that the list is not continuing. These are pretty common problems if you have a preposition start the
	clause - it would be hard to decide when the prepositional phrase ended and when the clause began if you didn't use the conjunction 'and' or 'or'.
	That's the same thought process here. The verb switches are the same, only that the nouns (subjects, indirect objects, and direct objects) are
	processed automatically - any noun not in a prepositional phrase before the verb is a subject (even if it is an appositive) and any noun afterwards
	can be determined to be an indirect or direct object based on whether the nouns are still listing. I added an additional switch, v_ended, to signify
	when the verbs list is done so that way any future verbs after the objects have been read can be labelled as not part of the same clause.

	The obj_full switch just determines if the indirect objects are finished or if they are still more indirect objects.
	----------------------------------------------------------------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------------------------------------------------------------
	*/
	//switch to see if the objects are finished listing.
	bool obj_full;
	//switch to see if a prepositional phrase is there or not
	bool prep_avail;
	//switches to control whether the sentence is listing a series of words or not.
	bool n_is_listing = false;
	bool n_end_list = false;

	bool v_is_listing = false;
	bool v_end_list = false;
	bool v_ended = false;

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

	//a method to process the string to make it easier to tokenize each word.
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
	void Read(const string& original_text);
};

void Clause::Read(const string& original_text)
{

	complete = false;
	//learn the words from the database
	Vocabulary::Learn();

	//Process the text as the clause statement
	Process_Text(original_text);

	Vocabulary::Label_Text_PoS(clause_statement);

	//listing out the unprocessed words and making them nodes in the graph for a word
	for (int index = 0; index < tokens.size(); index++)
	{
		Graph_Word* unproc_word = new Graph_Word(tokens[index]);
		unprocessed_words.push_back(unproc_word);
	}
	Make_Graph();
}

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

		//turn ellipses into periods. Ellipses signify trailing off anyways, so it is not too different.
		if (unprocessed_words[w_ind]->name == "...")
		{
			unprocessed_words[w_ind]->name = ".";
		}

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
			cout << w_ind << endl;
			Saved_PoS_Label.push_back(tokens_PoS_Label[w_ind]);
			for (int i = 0; i < tokens_PoS_Label[w_ind].size(); i++)
				cout << tokens_PoS_Label[w_ind][i] << endl;
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
			if (w_ind + 1 < unprocessed_words.size())
			{
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
			}
			else
			{
				stored_words_adv.push(unprocessed_words[w_ind]);
			}
		}
		
		//if the text is an adjective
		if (contains(tokens_PoS_Label[w_ind], "adj"))
		{
			//if the text is a predicate adjective, I'll store it as a noun and have it treated as such. Otherwise, it'll be an incomplete sentence.
			if (w_ind == unprocessed_words.size() - 1 && contains(tokens_PoS_Label[w_ind], "noun"))
			{
				tokens_PoS_Label[w_ind].clear();
				tokens_PoS_Label[w_ind].push_back("noun");
			}

			tokens_PoS_Label[w_ind].clear();
			tokens_PoS_Label[w_ind].push_back("adj");
			//in the case the following word is also an adverb, just append the adverb as a modifier to the next adverb.
			if (w_ind < unprocessed_words.size() - 1 && contains(tokens_PoS_Label[w_ind + 1], "noun"))
			{
				unprocessed_words[w_ind + 1]->descriptors.push_back(unprocessed_words[w_ind]);
			}
			else
			{
				stored_words_adj.push(unprocessed_words[w_ind]);
			}
		}
		
		//if there are adjectives upcoming, we can assume the verb case is incorrect.
		if (stored_words_adj.empty() || stored_words_adj.size() == 1 && prep_avail)
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

				if (clause.subj.empty())
				{
					if (!Saved_PoS_Label.empty() && contains(Saved_PoS_Label.back(), "prep") )
					{
						indep = true;
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
					else if (indep)
					{
						clause.subj.push_back(Graph_Word("you"));
					}
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
				else if (!contains(tokens_PoS_Label[w_ind + 1], "conj") && unprocessed_words[w_ind + 1]->name != "," && w_ind+1 != unprocessed_words.size())
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
			//ignore the conjunction if it is beginning a clause. This is to help with compound sentences, but also because even though it is not good grammar, no one 
			//really cares or looks for it besides English teachers. So we can just ignore it in most cases since it never really adds meaning to a sentence regardless.
			if (w_ind == 0)
			{
				continue;
			}
			//dealing with and without oxford comma for both verbs and nouns. For the oxford comma, it needs to have at least 3 elements to be grammatically correct.
			//i split the scenarios for each verb and noun for legibility.
			if (contains(tokens_PoS_Label[w_ind - 1], "verb") && unprocessed_words[w_ind - 1]->name != "," && clause.verbs.size() == 1)
			{
				cout << clause.verbs.size() << endl;
				v_ended = true;
				v_end_list = true;
			}
			else if (clause.verbs.size() >= 2 && contains(tokens_PoS_Label[w_ind - 2], "verb") && unprocessed_words[w_ind - 1]->name == "," && v_is_listing)
			{
				cout << clause.verbs.size() << endl;
				v_ended = true;
				v_end_list = true;
			}
			else if (contains(tokens_PoS_Label[w_ind - 1], "noun") && unprocessed_words[w_ind - 1]->name != "," && clause.noun.size() == 1)
			{
				n_end_list = true;
			}
			else if (clause.noun.size() >= 2 && contains(tokens_PoS_Label[w_ind - 2], "noun") && unprocessed_words[w_ind - 1]->name == ",")
			{
				n_end_list = true;
			}
			//in this case, it must be the start of a new independent clause.
			else
			{
				cout << "not complete and is run-on!" << endl;
				complete = false;
				run_on = true;
				return;
			}
		}

		if (unprocessed_words[w_ind]->name == ",")
		{
			if (w_ind + 1 != unprocessed_words.size())
			{
				if (contains(tokens_PoS_Label[w_ind - 1], "verb") && contains(tokens_PoS_Label[w_ind + 1], "noun"))
				{
					run_on = true;
					return;
				}

				if (contains(tokens_PoS_Label[w_ind - 1], "noun") && contains(tokens_PoS_Label[w_ind + 1], "verb"))
				{
					run_on = true;
					return;
				}
			}

			if (contains(tokens_PoS_Label[w_ind - 1], "verb"))
			{
				v_ended = true;
				v_end_list = true;
			}
			else if (contains(tokens_PoS_Label[w_ind - 1], "noun"))
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

	if (clause.subj.empty() || clause.verbs.empty())
	{
		complete = false;
	}

	if (contains(tokens_PoS_Label.back(), "conj") || contains(tokens_PoS_Label.back(), "adj"))
	{
		complete = false;
		cout << "incorrect ending!" << endl;
		return;
	}

	//if the beginning of the clause is a subordinate conjunction, we can append all the subjects of the clause as desciptors of the
	//subordinate conjunction. This gives us the ability to connect the rest of the clause as a descriptor for the subordinate conjunction
	//or relative pronoun.
	if (contains(tokens_PoS_Label[0], "sub_con"))
	{
		for (int subj_ind = 0; subj_ind < clause.subj.size(); subj_ind++)
		{
			unprocessed_words[0]->descriptors.push_back(&clause.subj[subj_ind]);
		}
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
	//text of the sentence
	string text;
	bool question;

	//list of all clauses in sentence
	vector <Clause> Clauses;
	
	//word tokens in the whole sentence
	vector <Graph_Word*> sent_unprocessed_words;
	
	//Constructors for each sentence.
	Sentence() {}
	Sentence(string s)
	{
		text = s;
		split();
	}
	void Question();
	//function to split text into clauses.
	void split();
};
//function to re-configure a question into a sentence structure
void Sentence::Question()
{

	return;
}

void Sentence::split()
{
	stack <Clause> Q_Clauses;
	//tokenize and label by part of speech every word in the sentence
	Vocabulary::Learn();
	Process_Text(text);
	Label_Text_PoS(clause_statement);
	queue <Graph_Word> saved_advs;
	//setting each of the tokens to be processed as a word
	for (int index = 0; index < tokens.size(); index++)
	{
		Graph_Word* unproc_word = new Graph_Word(tokens[index]);
		sent_unprocessed_words.push_back(unproc_word);
	}
	//now to algorithmize the splitting of each clause. For clauses in English, an easy algorithm is to keep
	//attaching words to the latest clause till it is complete and then keep doing so till it is not a run-on.
	//That is what we'll be doing - when the sub-ordinate conjunction arises, a dependent clause is coming.


	/*
	----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Q_Clauses.front().unprocessed_words.push_back(sent_unprocessed_words[index]); <--- line to add a word to the clause

	Q_Clauses.front().tokens_PoS_Label.resize(Q_Clauses.front().unprocessed_words.size()); <-- line that resizes the total number of parts of speech labels 
																							   for each word to fit the clause appropriately.

	Q_Clauses.front().tokens_PoS_Label.back() = tokens_PoS_Label[index]; <--- line to add the part of speech labels for that word into the clause
	----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	*/
	for (int index = 0; index < sent_unprocessed_words.size(); index++)
	{
		//if a period or a exclamation mark is there, it is the end of the sentence.
		if (sent_unprocessed_words[index]->name == "." || sent_unprocessed_words[index]->name == "!") 
		{
			question = false;
			return;
		}
		//question marks are the end of the sentence too, but they need to be re-configured to be understood in the normal subject-action-object context. 
		if (sent_unprocessed_words[index]->name == "?")
		{
			question = true;
			Question();
			return;
		}
		//subordinate conjunctions case, we take a greedy approach of starting a new clause and labelling it a dependent one.
		//In the case of another independent clause, we can just attach a make a new clause to represent that.
		if (contains(tokens_PoS_Label[index], "sub_con") || Q_Clauses.empty())
		{
			Clause* C = new Clause;
			C->indep = false;
			
			if (!Q_Clauses.empty())
			{
				//if the clause is an adverbial one, 
				if (contains(tokens_PoS_Label[index], "adv"))
				{
					//we can append it to the independent clause.
					if (Q_Clauses.top().indep)
					{
						for (int i = 0; i < Q_Clauses.top().clause.verbs.size(); i++)
						{
							Q_Clauses.top().clause.verbs[i].descriptors.push_back(sent_unprocessed_words[index]);
						}
					}
					//or, if the independent clause has not been found yet, we can save it.
					else
					{
						saved_advs.push(*sent_unprocessed_words[index]);
					}
				}
				//if the clause is adjectival, it must be preceded by a noun (if the preceding clause is still incomplete).
				else if (contains(tokens_PoS_Label[index - 1], "noun") && index > 0)
				{
					cout << "adding clause as a adjective" << endl;
					Q_Clauses.top().unprocessed_words[index - 1]->descriptors.push_back(sent_unprocessed_words[index]);
				}
				//if the clause is a noun, it will either be preceded by a verb or an adjective. If it is precede by an adjective
				//it could either be the subject or an object of the clause.
				else if (contains(tokens_PoS_Label[index - 1], "adj") && index > 0)
				{
					cout << "adding clause as a noun" << endl;
					//if there are no verbs that have been passed, it's got to be a subject of the clause.
					if (Q_Clauses.top().clause.verbs.empty())
					{
						Q_Clauses.top().clause.subj.push_back(*sent_unprocessed_words[index]);
					}
					//if there are verbs that have been passed, it's got to be an object.
					else
					{
						Q_Clauses.top().clause.noun.push(*sent_unprocessed_words[index]);
					}
				}
				//in the case that the preceding word is a verb, it must be an object, so we push it through there.
				else if (contains(tokens_PoS_Label[index - 1], "verb") && index > 0)
				{
					cout << "adding clause as an object" << endl;
					Q_Clauses.top().clause.noun.push(*sent_unprocessed_words[index]);
				}
			}
			cout << "making new clause" << endl;

			Q_Clauses.push(*C);
			Q_Clauses.top().unprocessed_words.push_back(sent_unprocessed_words[index]);
			Q_Clauses.top().tokens_PoS_Label.resize(Q_Clauses.top().unprocessed_words.size());
			Q_Clauses.top().tokens_PoS_Label.back() = tokens_PoS_Label[index];

			Q_Clauses.top().Make_Graph();
		}
		
		//the normal case of constantly adding new words to a clause till it is a complete thought
		else if (!Q_Clauses.top().complete || !Q_Clauses.top().run_on)
		{
			Q_Clauses.top().unprocessed_words.push_back(sent_unprocessed_words[index]);
			Q_Clauses.top().tokens_PoS_Label.resize(Q_Clauses.top().unprocessed_words.size());
			Q_Clauses.top().tokens_PoS_Label.back() = tokens_PoS_Label[index];
			Q_Clauses.top().Make_Graph();
			cout << "adding word to clause" << endl;
			cout << Q_Clauses.top().unprocessed_words.back()->name << endl;
		}
		
		//in the case that the clause has wrapped up too many extra words
		if (Q_Clauses.top().run_on || sent_unprocessed_words[index]->name == ";")
		{
			cout << "making new clause" << endl;
			Clauses.push_back(Q_Clauses.top());
			Q_Clauses.pop();
				//if this clause was nested in another clause, we can just go back to looking at the previous clause.
			if (!Q_Clauses.empty())
			{
				Q_Clauses.top().unprocessed_words.push_back(sent_unprocessed_words[index]);
				Q_Clauses.top().tokens_PoS_Label.resize(Q_Clauses.top().unprocessed_words.size());
				Q_Clauses.top().tokens_PoS_Label.back() = tokens_PoS_Label[index];
			}
			//if it was removed from the previous clause, we can append a new clause and begin again.
			else
			{
				Clause* C = new Clause;
				C->indep = true;
				//save all the adverbs
				while (!saved_advs.empty())
				{
					C->stored_words_adv.push(&saved_advs.front());
					saved_advs.pop();
				}
				Q_Clauses.push(*C);
				Q_Clauses.top().unprocessed_words.push_back(sent_unprocessed_words[index]);
				Q_Clauses.top().tokens_PoS_Label.resize(Q_Clauses.top().unprocessed_words.size());
				Q_Clauses.top().tokens_PoS_Label.back() = tokens_PoS_Label[index];
			}
		}
		
		//if it is the last word in the sentence, we can just wrap up the remaining clauses.
		if (index + 1 == sent_unprocessed_words.size())
		{
			cout << "clearing remaining clauses" << endl;
			while (!Q_Clauses.empty())
			{
				Clauses.push_back(Q_Clauses.top());
				Q_Clauses.pop();
			}
		}
		//clear the clause to avoid duplicate entries
		else
			Q_Clauses.top().clause.clear();
	}

	cout << "Number of Clauses in sentence: " << Clauses.size() << endl;
}

//structure to hold all the sentences in the text.
class Story : public Sentence
{
private:
	string text;
	vector <Sentence> Sentences;
	
public:
	//function that will split the text into multiple sentences. The sentences will then be processed 
	//through the sentence data structure.
	void split_into_sentences();
	Story() {}
	Story(string s)
	{
		text = s;
		split_into_sentences();
	}
};

void Story::split_into_sentences()
{
	vector <string> sent_text;
	int sent_start_ind = 0;

	for (int text_ind = 0; text_ind < text.size(); text_ind++)
	{
		if (text[text_ind] == '.' || text[text_ind] == '!' || text[text_ind] == '?' || text[text_ind] == '"')
		{
			sent_text.push_back(text.substr(sent_start_ind, text_ind - sent_start_ind));
			Sentence S(sent_text.back());
			Sentences.push_back(S);
			sent_start_ind = text_ind + 1;
		}
	}
	cout << "Number of sentences in story: " <<  Sentences.size() << endl;
}