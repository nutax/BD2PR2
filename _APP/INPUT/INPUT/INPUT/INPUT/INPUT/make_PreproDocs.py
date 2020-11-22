from nltk.stem import SnowballStemmer
from nltk.corpus import stopwords 
from nltk.tokenize import word_tokenize 
  
stop_words = set(stopwords.words('english'))
stemmer = SnowballStemmer('english')

names_path = "INPUT/RawDocs/Names/names.txt"
docs_path = "INPUT/RawDocs/Docs/"

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

with open(names_path, 'r') as names_file:
        while True:
                name = names_file.readline()[:-1]
                if not name or name == '':
                        break
                with open(docs_path + name, 'r', encoding = "utf8", errors = "replace") as doc_file:
                        with open("PreproDocs/"+ name, 'w', encoding = "utf8") as pp_doc_file:
                                words = word_tokenize(alphabet_filter(doc_file.read(), ' '))
                                stem = [stemmer.stem(w) for w in words if not w in stop_words and len(w) > 2]
                                pp_doc_file.write(' '.join(stem))