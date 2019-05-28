with open("gametext3.h", "rb") as f:
    text = f.read()
#/decoded = text.decode('cp850')
#for i, char in enumerate(text):
#    if char == "\\" and text[i+1] == "x":
#        number = text[i+2 : 2].from_hex()
#        print(number)
for char in text:
    if char > 127:
        print("\\" + format(char, 'o'), end = "")
    else:
        print(chr(char), end = "")
#print(text)
