
# coding: utf-8

# In[31]:

from urllib.request import urlopen #Web browser tool
from bs4 import BeautifulSoup #html reader tool
import re #string parsing tool
import numpy as np
import pandas as pd
import csv

#Very similar template to the Vocabulary Builder, notice that the Pronouns section is gone since that is complete.
#This is the section where I will be adding words to the CSV files from, so it will be easier to manage overall.
#The data for this can come from many sources, including texts I've read, documents on how many words a specific
#age group should know, and even from texts that the computer doesn't know.

verbs_to_find = ['seek']
verbs_learned = list()
#Add each conjugation of each verb in the list to the database, seperated by 1st person, 2nd person, and 3rd person
#updated as they updated their pages for each verb conjugation
for element in verbs_to_find:
    verb_conjugations = urlopen('http://conjugator.reverso.net/conjugation-english-verb-'+element+'.html')
    soup = BeautifulSoup(verb_conjugations,"html5lib")
    Verbs = list()
    tenses = list()
    pov = list()

    for perspectives in soup.find_all('div', attrs={'class': 'blue-box-wrap'}):
        Verbs.append(perspectives.get_text(" ",strip=True))
        tense_perspective = re.split('(?:^|(?<= ))(I|you|he/she/it|we|they)(?:(?= )|$)',Verbs[-1])
        for verbs in tense_perspective:
            verbs = verbs.strip()
        tenses.append(tense_perspective[0])
        pov.append(tense_perspective[2::2])
        tenses = tenses[:12]
        pov = pov[:12]

    pov = np.array(pov).transpose()
    pov = list(pov)
    verbs_learned.append(pov)
    
Verbs_1_S = list()
Verbs_2_S = list()
Verbs_3_S = list()
Verbs_1_P = list()
Verbs_2_P = list()
Verbs_3_P = list()
for verbs in verbs_learned:
    Verbs_1_S.append(verbs[0])
    Verbs_2_S.append(verbs[1])
    Verbs_3_S.append(verbs[2])
    Verbs_1_P.append(verbs[3])
    Verbs_2_P.append(verbs[4])
    Verbs_3_P.append(verbs[5])
    
#Converting Verb lists into dataframes indexed by present tense
Verbs_1_S = pd.DataFrame(Verbs_1_S, columns=tenses)
Verbs_1_S = Verbs_1_S.set_index(['Present '])

Verbs_2_S = pd.DataFrame(Verbs_2_S, columns=tenses)
Verbs_2_S = Verbs_2_S.set_index(['Present '])

Verbs_3_S = pd.DataFrame(Verbs_3_S, columns=tenses)
Verbs_3_S = Verbs_3_S.set_index(['Present '])

Verbs_1_P = pd.DataFrame(Verbs_1_P, columns=tenses)
Verbs_1_P = Verbs_1_P.set_index(['Present '])

Verbs_2_P = pd.DataFrame(Verbs_2_P, columns=tenses)
Verbs_2_P = Verbs_2_P.set_index(['Present '])

Verbs_3_P = pd.DataFrame(Verbs_3_P, columns=tenses)
Verbs_3_P = Verbs_3_P.set_index(['Present '])
    
#Prepositions list
Prepositions = []

#Social Function list
Social_Function = []

#Adjectives list
Adjectives = []
#Nouns Singular list (plural will be created and updated as needed)
Nouns_S = ['fortune']

#Adverbs list
Adverbs = []
                
#creating verbs files for each perspective
Verbs_1_S.to_csv('verbs_1_s.csv', mode = 'a', header=False)
Verbs_2_S.to_csv('verbs_2_s.csv', mode = 'a',  header=False)
Verbs_3_S.to_csv('verbs_3_s.csv', mode = 'a',  header=False)
Verbs_1_P.to_csv('verbs_1_p.csv', mode = 'a',  header=False)
Verbs_2_P.to_csv('verbs_2_p.csv', mode = 'a',  header=False)
Verbs_3_P.to_csv('verbs_3_p.csv', mode = 'a',  header=False)

#update noun file
with open('nouns.csv', 'a+', newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=',')
    for element in Nouns_S:
        writer.writerow([element,])

#update prepositions file
with open('prepositions.csv', 'a+', newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=',')
    for element in Prepositions:
        writer.writerow([element,])
    
#update Social Functions file
with open('social.csv','a+',newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=',')
    for element in Social_Function:
        writer.writerow([element,])
    

#update Adjectives file
with open('adj.csv','a+',newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=',')
    for element in Adjectives:
        writer.writerow([element,])
    
#update Adverbs file
with open('adv.csv','a+',newline='') as csvfile:
    writer = csv.writer(csvfile, delimiter=',')
    for element in Adverbs:
        writer.writerow([element,])


# In[ ]:



