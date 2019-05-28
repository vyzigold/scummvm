#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <iostream>

#include "po_parser.h"

PoMessageList::PoMessageList() : _list(NULL), _size(0), _allocated(0) {
}

PoMessageList::~PoMessageList() {
	for (int i = 0; i < _size; ++i)
		delete _list[i];
	delete[] _list;
}

int PoMessageList::compareString(const char* left, const char* right) {
	if (left == NULL && right == NULL)
		return 0;
	if (left == NULL)
		return -1;
	if (right == NULL)
		return 1;
	return strcmp(left, right);
}

int PoMessageList::compareMessage(const char *msgLeft, const char *contextLeft, const char *msgRight, const char *contextRight) {
	int compare = compareString(msgLeft, msgRight);
	if (compare != 0)
		return compare;
	return compareString(contextLeft, contextRight);
}

void PoMessageList::insert(const char *translation, const char *msg, const char *context) {
	if (msg == NULL || *msg == '\0' || translation == NULL || *translation == '\0')
		return;

	// binary-search for the insertion index
	int leftIndex = 0;
	int rightIndex = _size - 1;
	while (rightIndex >= leftIndex) {
		int midIndex = (leftIndex + rightIndex) / 2;
		int compareResult = compareMessage(msg, context, _list[midIndex]->msgid, _list[midIndex]->msgctxt);
		if (compareResult == 0)
			return; // The message is already in this list
		else if (compareResult < 0)
			rightIndex = midIndex - 1;
		else
			leftIndex = midIndex + 1;
	}
	// We now have rightIndex = leftIndex - 1 and we need to insert the new message
	// between the two (i.a. at leftIndex).
	if (_size + 1 > _allocated) {
		_allocated += 100;
		PoMessage **newList = new PoMessage*[_allocated];
		for (int i = 0; i < leftIndex; ++i)
			newList[i] = _list[i];
		for (int i = leftIndex; i < _size; ++i)
			newList[i + 1] = _list[i];
		delete[] _list;
		_list = newList;
	} else {
		for (int i = _size - 1; i >= leftIndex; --i)
			_list[i + 1] = _list[i];
	}
	_list[leftIndex] = new PoMessage(translation, msg, context);
	++_size;
}

const char *PoMessageList::findTranslation(const char *msg, const char *context) {
	if (msg == NULL || *msg == '\0')
		return NULL;

	// binary-search for the message
	int leftIndex = 0;
	int rightIndex = _size - 1;
	while (rightIndex >= leftIndex) {
		int midIndex = (leftIndex + rightIndex) / 2;
		int compareResult = compareMessage(msg, context, _list[midIndex]->msgid, _list[midIndex]->msgctxt);
		if (compareResult == 0)
			return _list[midIndex]->msgstr;
		else if (compareResult < 0)
			rightIndex = midIndex - 1;
		else
			leftIndex = midIndex + 1;
	}
	return NULL;
}

std::string escapeQuotes(char *s) {
	std::string str = s;
	long unsigned int pos = 0;
	while ((pos = str.find('\"', pos)) != std::string::npos)
	{
		str.replace(pos, 1, "\\\"");
		pos+=2;
	}
	return str;
}

PoMessageList *parsePoFile(const char *file) {
	std::cout << "\
/* ScummVM - Graphic Adventure Engine\n\
 *\n\
 * ScummVM is the legal property of its developers, whose names\n\
 * are too numerous to list here. Please refer to the COPYRIGHT\n\
 * file distributed with this source distribution.\n\
 *\n\
 * This program is free software; you can redistribute it and/or\n\
 * modify it under the terms of the GNU General Public License\n\
 * as published by the Free Software Foundation; either version 2\n\
 * of the License, or (at your option) any later version.\n\
 *\n\
 * This program is distributed in the hope that it will be useful,\n\
 * but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
 * GNU General Public License for more details.\n\
 *\n\
 * You should have received a copy of the GNU General Public License\n\
 * along with this program; if not, write to the Free Software\n\
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.\n\
 *\n\
 * This is a utility for generating a data file for the supernova engine.\n\
 * It contains strings extracted from the original executable as well\n\
 * as translations and is required for the engine to work properly.\n\
 */\n\
\n\
#ifndef GAMETEXT_H\n\
#define GAMETEXT_H\n\
\n\
#include <stddef.h>\n\
\n\
// This file contains the strings in German and is encoded using CP850 encoding.\n\
// Other language should be provided as po files also using the CP850 encoding.\n\
\n\
// TODO: add the strings from the engine here, add an Id string in comment.\n\
// And in the engine add a StringId enum with all the Ids = index in this array.\n\
\n\
const char *gameText[] = {\n";
	FILE *inFile = fopen(file, "r");
	if (!inFile)
		return NULL;

	char msgidBuf[1024], msgctxtBuf[1024], msgstrBuf[1024];
	char line[1024], *currentBuf = msgstrBuf;

	PoMessageList *list = new PoMessageList();

	// Initialize the message attributes.
	bool fuzzy = false;
	bool fuzzy_next = false;
	int row_count = -1;

	// Parse the file line by line.
	// The msgstr is always the last line of an entry (i.e. msgid and msgctxt always
	// precede the corresponding msgstr).
	msgidBuf[0] = msgstrBuf[0] = msgctxtBuf[0] = '\0';
	while (!feof(inFile) && fgets(line, 1024, inFile)) {
		if (line[0] == '#' && line[1] == ',') {
			// Handle message attributes.
			if (strstr(line, "fuzzy")) {
				fuzzy_next = true;
				continue;
			}
		}
		// Skip empty and comment line
		if (*line == '\n' || *line == '#')
			continue;
		if (strncmp(line, "msgid", 5) == 0) {
			if (currentBuf == msgstrBuf) {
				// add previous entry
				if (*msgstrBuf != '\0' && !fuzzy)
				{
					//list->insert(msgstrBuf, msgidBuf, msgctxtBuf);
					if (row_count % 5 == 0)
						std::cout << "// " << row_count << std::endl;
					if (row_count != -1)
						std::cout << "\"" << escapeQuotes(msgidBuf) << "\",    //" << msgstrBuf << std::endl;
					row_count++;
				}
				msgidBuf[0] = msgstrBuf[0] = msgctxtBuf[0] = '\0';

				// Reset the attribute flags.
				fuzzy = fuzzy_next;
				fuzzy_next = false;
			}
			strcpy(msgidBuf, stripLine(line));
			currentBuf = msgidBuf;
		} else if (strncmp(line, "msgctxt", 7) == 0) {
			if (currentBuf == msgstrBuf) {
				// add previous entry
				if (*msgstrBuf != '\0' && !fuzzy)
				{
					//list->insert(msgstrBuf, msgidBuf, msgctxtBuf);
					if (row_count % 5 == 0)
						std::cout << "// " << row_count << std::endl;
					if (row_count != -1)
						std::cout << "\"" << escapeQuotes(msgidBuf) << "\",    //" << msgstrBuf << std::endl;
					row_count++;
				}
				msgidBuf[0] = msgstrBuf[0] = msgctxtBuf[0] = '\0';

				// Reset the attribute flags
				fuzzy = fuzzy_next;
				fuzzy_next = false;
			}
			strcpy(msgctxtBuf, stripLine(line));
			currentBuf = msgctxtBuf;
		} else if (strncmp(line, "msgstr", 6) == 0) {
			strcpy(msgstrBuf, stripLine(line));
			currentBuf = msgstrBuf;
		} else {
			// concatenate the string at the end of the current buffer
			if (currentBuf)
				strcat(currentBuf, stripLine(line));
		}
	}
	if (currentBuf == msgstrBuf) {
		// add last entry
		if (*msgstrBuf != '\0' && !fuzzy)
		{
			list->insert(msgstrBuf, msgidBuf, msgctxtBuf);
			std::cout << "NULL" << std::endl << "};" << std::endl << std::endl << "#endif // GAMETEXT_H" << std::endl;
		}
	}

	fclose(inFile);
	return list;
}

char *stripLine(char *const line) {
	// This function modifies line in place and return it.
	// Keep only the text between the first two unprotected quotes.
	// It also look for literal special characters (e.g. preceded by '\n', '\\', '\"', '\'', '\t')
	// and replace them by the special character so that strcmp() can match them at run time.
	// Look for the first quote
	char const *src = line;
	while (*src != '\0' && *src++ != '"') {}
	// shift characters until we reach the end of the string or an unprotected quote
	char *dst = line;
	while (*src != '\0' && *src != '"') {
		char c = *src++;
		if (c == '\\') {
			switch (c = *src++) {
				case  'n': c = '\n'; break;
				case  't': c = '\t'; break;
				case '\"': c = '\"'; break;
				case '\'': c = '\''; break;
				case '\\': c = '\\'; break;
				default:
					// Just skip
					fprintf(stderr, "Unsupported special character \"\\%c\" in string. Please contact ScummVM developers.\n", c);
					continue;
			}
		}
		*dst++ = c;
	}
	*dst = '\0';
	return line;
}

char *parseLine(const char *line, const char *field) {
	// This function allocate and return a new char*.
	// It will return a NULL pointer if the field is not found.
	// It is used to parse the header of the po files to find the language name
	// and the charset.
	const char *str = strstr(line, field);
	if (str == NULL)
		return NULL;
	str += strlen(field);
	// Skip spaces
	while (*str != '\0' && isspace(*str)) {
		++str;
	}
	// Find string length (stop at the first '\n')
	int len = 0;
	while (str[len] != '\0' && str[len] != '\n') {
		++len;
	}
	if (len == 0)
		return NULL;
	// Create result string
	char *result = new char[len + 1];
	strncpy(result, str, len);
	result[len] = '\0';
	return result;
}

