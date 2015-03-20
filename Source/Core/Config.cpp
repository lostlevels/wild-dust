#include "Precompiled.h"
#include "Config.h"

Config::Config(){

}

Config::~Config() {

}

static bool isNewline(char c) {
	if (c == '\n' || c == '\r') {
		return true;
	}
	return false;
}

static bool isWhitespace(char c) {
	if (c == '\t' || c == ' ') {
		return true;
	}
	return false;
}

bool Config::loadFromFile(const std::string &filename) {
	FILE *file = fopen(filename.c_str(), "rb");
	if (file == NULL) {
		gLogger.error("Could not open config file from %s\n", filename.c_str());
		return false;
	}

	mKeyValues.clear();

	char line[256];
	while (fgets(line, sizeof(line), file)) {
		char *equalPtr = strchr(line, '=');
		if (equalPtr == NULL) {
			// No equal sign
			continue;
		}

		if (equalPtr == line) {
			// Nothing on left hand side
			continue;
		}

		char nextChar = *(equalPtr + 1);
		if (isWhitespace(nextChar) || isNewline(nextChar)) {
			// Nothing on right hand side
			continue;
		}

		char lhs[256];
		memcpy(lhs, line, (equalPtr - line));
		lhs[equalPtr - line] = 0;
		
		char rhs[256];
		char *rhs_cur = rhs;
		char *line_cur = equalPtr + 1;
		do {
			*rhs_cur = *line_cur;
			++rhs_cur;
			++line_cur;
			nextChar = *line_cur;
		} while (nextChar != '\0' && !isWhitespace(nextChar) && !isNewline(nextChar));
		*rhs_cur = 0;

		mKeyValues.insert({ lhs, rhs });
	}

	fclose(file);

	return true;
}

bool Config::saveToFile(const std::string &filename) const {
	FILE *file = fopen(filename.c_str(), "wb");
	if (file == NULL) {
		gLogger.error("Could not open file %s for writing\n", filename.c_str());
		return false;
	}

	for (auto it : mKeyValues) {
		fprintf(file, "%s=%s", it.first.c_str(), it.second.c_str());
	}

	fclose(file);

	return true;
}



