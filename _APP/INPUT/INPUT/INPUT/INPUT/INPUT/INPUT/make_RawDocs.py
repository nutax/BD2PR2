csv_path = "INPUT/articles1.csv"

with open(csv_path, 'r', encoding = "utf8", errors = "replace") as csv_file:
        with open("RawDocs/Names/names.txt", 'w') as names_file:
                line = csv_file.readline()
                while True:
                        line = csv_file.readline()
                        if not line or line == '':
                                break
                        values = line.split(',')
                        if len(values) < 9:
                                break
                        with open("RawDocs/Docs/" + values[1]+ ".txt", 'w', errors = "replace") as doc_file:
                                doc_file.write(','.join(values[9:]))
                                names_file.write(values[1]+".txt\n")