#pragma once
#include "CSVReader.h"
#include "TrieDS.h"
#include <map>

using namespace std;
//list of all unknown words in text
vector <string> unknown_words;

//structure that stores all the words from CSV files created in Python environment
struct Vocabulary
{
	//initialize a csv reader to read documents
	csvreader reader;

	//containers for the different categories of words
	vector<Trie> verbs_1_s;
	vector<string> verbs_header;
	vector<Trie> verbs_2_s;
	vector<Trie> verbs_3_s;
	vector<Trie> verbs_1_p;
	vector<Trie>verbs_2_p;
	vector<Trie> verbs_3_p;
	Trie nouns_s;
	Trie nouns_p;
	Trie prepositions;
	Trie social;
	Trie adjectives;
	Trie adverbs;
	vector<Trie> pronouns;
	Trie conjunctions;
	Trie modal_verbs;
	Trie articles;
	Trie subjunctive_conjunctions;
	
	//list of all words in the text
	vector <string> tokens;
	
	//2-d container of all the Part of Speech labels the algorithm reads for each token
	vector <vector <string> > tokens_PoS_Label;

	//map containing all the words of a Part of Speech type together, they're connected to the name of the trie
	map <string, Trie> Words;

	//function that grabs the words found in the Python environment and places them into the correct Tries
	void Learn()
	{
		//below part is to gather list of words from the CSV files
		//----------------------------------------------------------
		//----------------------------------------------------------

		vector <string> articles_vec = { "the", "a", "an" };	//articles vector. Forgot to make an articles file.

		vector <string> conjunctions_vec = { "for", "and",		//I realized I forgot to make a conjunctions file
			"nor", "but", "or", "yet" };						//so I just wrote it in here. They were taken from
																//the acronym FANBOYS for co-ordinating conjunctions.

		reader.read("C:\\Users\\kodur\\verbs_1_s.csv");			//read 1st Person Singular verbs file
		vector <vector <string>> verbs_1s_2dvec = reader.data;
		vector <string> verbs_header = reader.heading;			//get the header for each file

		reader.read("C:\\Users\\kodur\\verbs_2_s.csv");			//read 2nd Person Singular verbs file
		vector <vector <string>> verbs_2s_2dvec = reader.data;
		reader.read("C:\\Users\\kodur\\verbs_3_s.csv");			//read 3rd Person Singular verbs file
		vector <vector <string>> verbs_3s_2dvec = reader.data;
		reader.read("C:\\Users\\kodur\\verbs_1_p.csv");			//read 1st Person Plural verbs file
		vector <vector <string>> verbs_1p_2dvec = reader.data;
		reader.read("C:\\Users\\kodur\\verbs_2_p.csv");			//read 2nd Person Plural verbs file
		vector <vector <string>> verbs_2p_2dvec = reader.data;
		reader.read("C:\\Users\\kodur\\verbs_3_p.csv");			//read 3rd Person Plural verbs file
		vector <vector <string>> verbs_3p_2dvec = reader.data;

		vector <string> m_verbs = { "can","could","may", "might",						//also I forgot to add a
			"must", "shall","should", "will", "would" };								//databank for modal verbs

		reader.read("C:\\Users\\kodur\\subjunctive_con.csv", false);					//read singular subjunctive conjunctions file
		vector <string> subjunctive_con_vec = reader.data[0];							//data in the 1st column

		reader.read("C:\\Users\\kodur\\nouns_s.csv", false);							//read singular nouns file
		vector <string> nouns_s_vec = reader.data[0];									//nouns are put on the 1st column

		reader.read("C:\\Users\\kodur\\nouns_p.csv", false);							//read plural nouns file
																						//didn't initially create a plural nouns 
		if (!reader.data.empty()) {														//file, so check if it exists yet. Nouns
			vector <string> nouns_p_vec = reader.data[0];								//are put on the 1st column.

																						//if it does exist, gather the plural nouns into their own trie
			for (int noun_p_word = 0; noun_p_word < nouns_p_vec.size(); noun_p_word++)
			{
				nouns_p.add(nouns_p_vec[noun_p_word]);
			}
		}
		reader.read("C:\\Users\\kodur\\prepositions.csv", false);						//read prepositions file
		vector <string> prep_vec = reader.data[0];										//prepositions are put on the 1st column

		reader.read("C:\\Users\\kodur\\social.csv", false);								//read social cues file
		vector <string> soc_vec = reader.data[0];										//social cues only put on the first column

		reader.read("C:\\Users\\kodur\\adj.csv", false);								//read adjectives file
		vector <string> adj_vec = reader.data[0];										//adjectives only put on the first column

		reader.read("C:\\Users\\kodur\\adv.csv", false);								//read adverbs file
		vector <string> adv_vec = reader.data[0];										//adverbs only put on the first column

		reader.read("C:\\Users\\kodur\\Pronouns.csv");											//read pronouns file
		vector <vector <string>> Pronouns_2dvec = reader.data;									//pronouns excluding row index
																								//placed by pandas DataFrame

																								//----------------------------------------------------------
																								//----------------------------------------------------------
																								//Below part is creation of the tries for each of the categories of words
																								//this part is generating the tries for the verbs

																								//getting the vector of tries to the correct size
		verbs_1_s.resize(verbs_1s_2dvec.size());
		verbs_2_s.resize(verbs_2s_2dvec.size());
		verbs_3_s.resize(verbs_3s_2dvec.size());
		verbs_1_p.resize(verbs_1p_2dvec.size());
		verbs_2_p.resize(verbs_2p_2dvec.size());
		verbs_3_p.resize(verbs_3p_2dvec.size());

		pronouns.resize(Pronouns_2dvec.size());

		//the columns of the 2-D vector of verbs are the tenses. Each of the words are stored as a specific verb in the total array.


		for (int tense = 0; tense < verbs_1s_2dvec.size(); tense++)							  //iterate through each of the tenses
		{
			for (int verb_word = 0; verb_word < verbs_1s_2dvec[tense].size(); verb_word++) //iterate through each of the words in list
			{
				verbs_1_s[tense].add(verbs_1s_2dvec[tense][verb_word]);
				verbs_2_s[tense].add(verbs_2s_2dvec[tense][verb_word]);
				verbs_3_s[tense].add(verbs_3s_2dvec[tense][verb_word]);
				verbs_1_p[tense].add(verbs_1p_2dvec[tense][verb_word]);
				verbs_2_p[tense].add(verbs_2p_2dvec[tense][verb_word]);
				verbs_3_p[tense].add(verbs_3p_2dvec[tense][verb_word]);
			}
		}

		//gather the singular nouns
		for (int noun_s_word = 0; noun_s_word < nouns_s_vec.size(); noun_s_word++)
		{
			nouns_s.add(nouns_s_vec[noun_s_word]);
		}
		//gather the prepositions
		for (int prep_word = 0; prep_word < prep_vec.size(); prep_word++)
		{
			prepositions.add(prep_vec[prep_word]);
		}
		//gather the prepositions
		for (int soc_word = 0; soc_word < soc_vec.size(); soc_word++)
		{
			social.add(soc_vec[soc_word]);
		}
		//gather the adjectives
		for (int adj_word = 0; adj_word < adj_vec.size(); adj_word++)
		{
			adjectives.add(adj_vec[adj_word]);
		}
		//gather the adverbs
		for (int adv_word = 0; adv_word < adv_vec.size(); adv_word++)
		{
			adverbs.add(adv_vec[adv_word]);
		}
		//gather the conjunctions
		for (int conj_word = 0; conj_word < conjunctions_vec.size(); conj_word++)
		{
			conjunctions.add(conjunctions_vec[conj_word]);
		}
		//gather the subjunctive conjunctions
		for (int conj_word = 0; conj_word < subjunctive_con_vec.size(); conj_word++)
		{
			subjunctive_conjunctions.add(subjunctive_con_vec[conj_word]);
		}
		//gather the articles
		for (int art_word = 0; art_word < articles_vec.size(); art_word++)
		{
			articles.add(articles_vec[art_word]);
		}
		//gather the modal verbs
		for (int m_verb_word = 0; m_verb_word < m_verbs.size(); m_verb_word++)
		{
			modal_verbs.add(m_verbs[m_verb_word]);
		}

		//columns of the 2d array of Pronouns are the different types of ways of using a pronoun, and also separated by plural and 
		//singular. Each element inside refers to the specific pronoun held inside the column.

		for (int type = 0; type < Pronouns_2dvec.size(); type++)				//iterate through the different types of pronouns
		{
			for (int perspective = 0; perspective < Pronouns_2dvec[type].size(); perspective++)	//iterate through the perspectives
			{
				pronouns[type].add(Pronouns_2dvec[type][perspective]);
			}
		}

		//creating a dictionary for the verbs
		map <string, vector<Trie>> Verbs = {
			{ "verbs1s",verbs_1_s },{ "verbs2s",verbs_2_s },{ "verbs3s",verbs_3_s },{ "verbs1p",verbs_1_p },
			{ "verbs2p",verbs_2_p },{ "verbs3p",verbs_3_p }
		};
		//creating a dictionary for each of the categories of words
		Words =
		{
			//miscellaneous types of words
			{ "noun sing",nouns_s },{ "noun pl", nouns_p },{ "prep", prepositions },{ "social", social },
			{ "adj", adjectives },{ "adv", adverbs },{ "conj", conjunctions }, {"sub_con", subjunctive_conjunctions}, 

			//section for pronouns separated by type of pronoun
			{ "Pronoun Sing", pronouns[0] },{ "Pronoun Pl", pronouns[1] },{ "Pronoun Obj Sing", pronouns[2] },
			{ "Pronoun Obj Pl", pronouns[3] },{ "Pro Poss adj Sing", pronouns[4] },{ "Pro Poss adj Pl", pronouns[5] },
			{ "Pro adv Poss Sing", pronouns[6] },{ "Pro adv Poss Pl", pronouns[7] },
			//section for verbs by their tense whether they are 1st/2nd/3rd person
			//the _p implies it is perfect tense, _c is continous, and _p_c is perfect continuous tense

			//1st person singular verbs
			{ "verbs1s_pres", verbs_1_s[0] },{ "verbs1s_past", verbs_1_s[1] },{ "verbs1s_pres_c", verbs_1_s[2] },
			{ "verbs1s_pres_p", verbs_1_s[3] },{ "verbs1s_fut", verbs_1_s[4] },{ "verbs1s_fut_p", verbs_1_s[5] },
			{ "verbs1s_past_c", verbs_1_s[6] },{ "verbs1s_past_p", verbs_1_s[7] },{ "verbs1s_fut_c", verbs_1_s[8] },
			{ "verbs1s_pres_p_c", verbs_1_s[9] },{ "verbs1s_past_p_c", verbs_1_s[10] },{ "verbs1s_fut_p_c", verbs_1_s[11] },
			//2nd person singular verbs
			{ "verbs2s_pres", verbs_2_s[0] },{ "verbs2s_past", verbs_2_s[1] },{ "verbs2s_pres_c", verbs_2_s[2] },
			{ "verbs2s_pres_p", verbs_2_s[3] },{ "verbs2s_fut", verbs_2_s[4] },{ "verbs2s_fut_p", verbs_2_s[5] },
			{ "verbs2s_past_c", verbs_2_s[6] },{ "verbs2s_past_p", verbs_2_s[7] },{ "verbs2s_fut_c", verbs_2_s[8] },
			{ "verbs2s_pres_p_c", verbs_2_s[9] },{ "verbs2s_past_p_c", verbs_2_s[10] },{ "verbs2s_fut_p_c", verbs_2_s[11] },
			//3rd person singular verbs
			{ "verbs3s_pres", verbs_3_s[0] },{ "verbs3s_past", verbs_3_s[1] },{ "verbs3s_pres_c", verbs_3_s[2] },
			{ "verbs3s_pres_p", verbs_3_s[3] },{ "verbs3s_fut", verbs_3_s[4] },{ "verbs3s_fut_p", verbs_3_s[5] },
			{ "verbs3s_past_c", verbs_3_s[6] },{ "verbs3s_past_p", verbs_3_s[7] },{ "verbs3s_fut_c", verbs_3_s[8] },
			{ "verbs3s_pres_p_c", verbs_3_s[9] },{ "verbs3s_past_p_c", verbs_3_s[10] },{ "verbs3s_fut_p_c", verbs_3_s[11] },
			//1st person plural verbs
			{ "verbs1p_pres", verbs_1_p[0] },{ "verbs1p_past", verbs_1_p[1] },{ "verbs1p_pres_c", verbs_1_p[2] },
			{ "verbs1p_pres_p", verbs_1_p[3] },{ "verbs1p_fut", verbs_1_p[4] },{ "verbs1p_fut_p", verbs_1_p[5] },
			{ "verbs1p_past_c", verbs_1_p[6] },{ "verbs1p_past_p", verbs_1_p[7] },{ "verbs1p_fut_c", verbs_1_p[8] },
			{ "verbs1p_pres_p_c", verbs_1_p[9] },{ "verbs1p_past_p_c", verbs_1_p[10] },{ "verbs1p_fut_p_c", verbs_1_p[11] },
			//2nd person plural verbs
			{ "verbs2p_pres", verbs_2_p[0] },{ "verbs2p_past", verbs_2_p[1] },{ "verbs2p_pres_c", verbs_2_p[2] },
			{ "verbs2p_pres_p", verbs_2_p[3] },{ "verbs2p_fut", verbs_2_p[4] },{ "verbs2p_fut_p", verbs_2_p[5] },
			{ "verbs2p_past_c", verbs_2_p[6] },{ "verbs2p_past_p", verbs_2_p[7] },{ "verbs2p_fut_c", verbs_2_p[8] },
			{ "verbs2p_pres_p_c", verbs_2_p[9] },{ "verbs2p_past_p_c", verbs_2_p[10] },{ "verbs2p_fut_p_c", verbs_2_p[11] },
			//3rd person plural verbs
			{ "verbs3p_pres", verbs_3_p[0] },{ "verbs3p_past", verbs_3_p[1] },{ "verbs3p_pres_c", verbs_3_p[2] },
			{ "verbs3p_pres_p", verbs_3_p[3] },{ "verbs3p_fut", verbs_3_p[4] },{ "verbs3p_fut_p", verbs_3_p[5] },
			{ "verbs3p_past_c", verbs_3_p[6] },{ "verbs3p_past_p", verbs_3_p[7] },{ "verbs3p_fut_c", verbs_3_p[8] },
			{ "verbs3p_pres_p_c", verbs_3_p[9] },{ "verbs3p_past_p_c", verbs_3_p[10] },{ "verbs3p_fut_p_c", verbs_3_p[11] },
			//modal verbs and articles
			{ "modal_verbs", modal_verbs }, {"article", articles}
		};
	}

	//How I am planning on parsing through the sentence:
	//as many of you know, there are sometimes multiple words that are connected. Obvious examples of this 
	//include proper nouns, such as names, and also different tenses of each verb. While the words themselves
	//are separate, the idea they represent doesn't make sense unless they are grouped. To combat this, I'm
	//going to run an algorithm that gets the longest chain of words in the trie and group those words together.

	//For instance, in the sentence 'the car is blue.' In this case, we would look for the longest chain of words
	//that we can find on the initial segment of the string - in this case that is 'the'. After we process that word,
	//we move onto 'car is blue' segment. We find that 'car' is the longest segment, and continue to 'is', then 'blue'.
	//For a simple sentence like that, it is not any different than splitting the whole string into a word tokens. However,
	//in the case of 'Harry Potter is a wizard', it would find 'Harry Potter' [as opposed to just 'Harry' and 'Potter'] 
	//and then move onto the rest of the sentence. This can extrapolate further, as in 'the car is on the Golden Gate Bridge'
	//and such like examples.

	//In essence, what I'm trying to do is get a string that processes the longest chain on the trie as it can go on the 
	//string,then the string will enter the trie again and again till it is completely processed.

	//function will output the word and then the label it comes with.
	void Label_Text_PoS(const string& S)
	{
		//container for each of the words
		string word;
		string remember;

		//a switch to determine if the previous word was found and another to find if the current word is known
		bool prev_found = false;
		bool found_current = false;

		stringstream reader(S);

		while (getline(reader, word, ' '))
		{
			//if the previous word can be found, check to see if when it connected to the next word if it can be found
			if (prev_found)
			{
				prev_found = false;
				string old = remember;
				//change the remember string to have all of the new word
				remember += " " + word;
				//iterate through each of the word categories
				for (map <string, Trie>::iterator category = Words.begin(); category != Words.end(); category++)
				{
					//if the concatenated word is a singular phrase, we can reassess the previously found singular word as the whole phrase
					if (category->second.find(remember))
					{
						if (tokens.back() == old)
						{
							//remove the previously singular word of the phrase and replase it with the whole phrase
							tokens.pop_back();
							tokens_PoS_Label.pop_back();
							tokens.push_back(remember);
						}
						//append the newly found part of speech for a token into its list of available parts of speeches
						tokens_PoS_Label.resize(tokens.size());
						tokens_PoS_Label.back().push_back(category->first);
						prev_found = true;
					}
				}
			}

			//if the previous word cannot be found, check the singular token to see if it can be found
			if (!prev_found)
			{
				prev_found = false;
				//iterate through each of the word categories
				found_current = false;
				for (map <string, Trie>::iterator category = Words.begin(); category != Words.end(); category++)
				{
					if (category->second.find(word))
					{
						//if the number of tokens exceeds 1, there is a possibility of duplicates arising. In those cases, remove them as they come.
						if (tokens.size() >= 1)
						{
							if (tokens.back() == word)
								tokens.pop_back();
						}
						//append the word to the tokens if it is found and append the appropraite part of speech to its labels as well.
						tokens.push_back(word);
						tokens_PoS_Label.resize(tokens.size());
						tokens_PoS_Label.back().push_back(category->first);
						prev_found = true;
						found_current = true;
					}
				}

				if (found_current == false)
				{
					tokens.push_back(word);
					tokens_PoS_Label.resize(tokens.size());
					cout << "unknown word: " << word << endl;
					unknown_words.push_back(word);
					tokens_PoS_Label.back().push_back("unknown");
				}
			}

			if (prev_found)
			{
				//save the previous word if it can be found
				remember = word;
			}
		}
	}

	void Disp_Tokens()
	{
		for (int i = 0; i < tokens.size(); i++)
		{
			cout << i << " " << tokens[i] << ":" << endl;
			for (int j = 0; j < tokens_PoS_Label[i].size(); j++)
			{
				cout << "--- " << tokens_PoS_Label[i][j] << endl;
			}
		}
	}
};
