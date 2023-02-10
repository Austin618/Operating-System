import re
import sys

filename = sys.argv[1]

counts_dict = {"I": 0, "L": 0, "S": 0, "M": 0}
instructions_dict = {}
data_dict = {}

# f1 = open('./traces/addr-matmul.ref', 'r')
f1 = open(filename, 'r')

while True:
    line = f1.readline()
    if line:
        reference = re.split("[ ,]+", line)
        # reference == ['I', '00108e0f', '3']

        if reference[0] == "I":
            counts_dict["I"] += 1
            temp = "0x{0}000".format(re.sub("^0+(?!$)", "", reference[1])[:-3])
            if temp not in instructions_dict:
                instructions_dict[temp] = 1
            else:
                instructions_dict[temp] += 1
        else:
            if reference[0] == "L":
                counts_dict["L"] += 1
            elif reference[0] == "S":
                counts_dict["S"] += 1
            elif reference[0] == "M":
                counts_dict["M"] += 1
            temp = "0x{0}000".format(re.sub("^0+(?!$)", "", reference[1])[:-3])
            if temp not in data_dict:
                data_dict[temp] = 1
            else:
                data_dict[temp] += 1
    else:
        break

f1.close()

print("Counts:")
print("  Instructions {0:>8}".format(counts_dict["I"]))
print("  Loads        {0:>8}".format(counts_dict["L"]))
print("  Stores       {0:>8}".format(counts_dict["S"]))
print("  Modifies     {0:>8}".format(counts_dict["M"]))

print("\nInstructions:")
for element in list(sorted(instructions_dict.items(), key = lambda k: k[1], reverse = True)):
    # print(element)
    print("{0},{1}".format(element[0], element[1])) 

print("\nData:")
for element in list(sorted(data_dict.items(), key = lambda k: k[1], reverse = True)):
    print("{0},{1}".format(element[0], element[1])) 

