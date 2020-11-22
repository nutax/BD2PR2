import os
from nltk.stem import SnowballStemmer
from nltk.corpus import stopwords 
from nltk.tokenize import word_tokenize

stop_words = set(stopwords.words('english'))
stemmer = SnowballStemmer('english')

alphabet = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}
accents = ( ("á", "a"),("é", "e"),("í", "i"),("ó", "o"),("ú", "u"),("ä", "a"),("ë", "e"),("ï", "i"),("ö", "o"),("ü", "u"),("â", "a"),("ê", "e"),("î", "i"),("ô", "o"),("û", "u"))

def alphabet_filter(txt, replacement):
        #lower case
        txt = txt.lower()

        #no accents
        for a, b in accents:
                txt = txt.replace(a, b)
        
        #no enie
        txt = txt.replace("ñ", "n")

        #no non alphas
        i = 0
        sz = len(txt)
        alphas = ""
        while i < sz:
                alphas += txt[i] if txt[i] in alphabet else replacement
                i += 1
        return alphas


lines = open("INPUT/InvertedIndex/index.txt", "r").read().split("\n")
index = [line.split(",") for line in lines[:-1]]



def BinarySearch(x):
    min = 0
    max = len(index)-1
    mid = 0

    while min <= max:

        mid = (max + min) // 2

        if index[mid][0] == x:
            return index[mid][1], True

        elif index[mid][0] < x:
            min = mid + 1

        else:
            max = mid - 1

    return -1, False


consulta = input("SEARCH: \t")
while consulta != "-1" or len(consulta) == 0:
        words = word_tokenize(alphabet_filter(consulta, ' '))
        stem = [stemmer.stem(w) for w in words if not w in stop_words and len(w) > 2]
        features = {}
        for s in stem:
                features[s] = [0, 0, 0]
        for s in stem:
                if features[s][0] == 0:
                        ptr, found = BinarySearch(s)
                        if found:
                                features[s][0] = 1
                                features[s][1] = ptr
                                features[s][2] += 1
                        else:
                                features[s][0] = -1
                elif features[s][0] == 1:
                        features[s][3] += 1
        
        cpp_argv = ""
        for feature in features.values():
                if feature[0] == 1:
                        cpp_argv += " " + feature[1] + " " + str(feature[2])
        os.system("search_bin.exe" + cpp_argv)
        print("\nCHECK FILE \"_result.txt\"\n\n")
        consulta = input("SEARCH: \t")
