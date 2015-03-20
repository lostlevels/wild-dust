#pragma once

class CORE_API Config {
public:
	Config();
	~Config();

	bool loadFromFile(const std::string &filename);
	bool saveToFile(const std::string &filename) const;

	int getInt(const std::string &key, int defaultValue = 0) const;
	float getFloat(const std::string &key, float defaultValue = 0.0f) const;
	bool getBool(const std::string &key, bool defaultValue = false) const;
	std::string getString(const std::string &key, std::string defaultValue = "") const;

	void setInt(const std::string &key, int value);
	void setFloat(const std::string &key, float value);
	void setBool(const std::string &key, bool value);
	void setString(const std::string &key, const std::string &value);

private:
	std::map<std::string, std::string> mKeyValues;
};

inline int Config::getInt(const std::string &key, int defaultValue) const {
	auto result = mKeyValues.find(key);
	if (result != mKeyValues.end()) {
		return atoi(result->second.c_str());
	}
	return defaultValue;
}

inline float Config::getFloat(const std::string &key, float defaultValue) const {
	auto result = mKeyValues.find(key);
	if (result != mKeyValues.end()) {
		return (float)atof(result->second.c_str());
	}
	return defaultValue;
}

inline bool Config::getBool(const std::string &key, bool defaultValue) const {
	auto result = mKeyValues.find(key);
	if (result != mKeyValues.end()) {
		if (stricmp(result->second.c_str(), "true") == 0) {
			return true;
		}
		else {
			return false;
		}
	}
	return defaultValue;
}

inline std::string Config::getString(const std::string &key, std::string defaultValue) const {
	auto result = mKeyValues.find(key);
	if (result != mKeyValues.end()) {
		return result->second;
	}
	return defaultValue;
}

inline void Config::setInt(const std::string &key, int value) {
	auto result = mKeyValues.find(key);
	if (result == mKeyValues.end()) {
		mKeyValues.insert({ key, std::to_string(value) });
	}
	else {
		mKeyValues[key] = std::to_string(value);
	}
}

inline void Config::setFloat(const std::string &key, float value) {
	auto result = mKeyValues.find(key);
	if (result == mKeyValues.end()) {
		mKeyValues.insert({ key, std::to_string(value) });
	}
	else {
		mKeyValues[key] = std::to_string(value);
	}
}

inline void Config::setBool(const std::string &key, bool value) {
	auto result = mKeyValues.find(key);
	if (result == mKeyValues.end()) {
		mKeyValues.insert({ key, value ? "true" : "false" });
	}
	else {
		mKeyValues[key] = value ? "true" : "false";
	}
}

inline void Config::setString(const std::string &key, const std::string &value) {
	auto result = mKeyValues.find(key);
	if (result == mKeyValues.end()) {
		mKeyValues.insert({ key, value });
	}
	else {
		mKeyValues[key] = value;
	}
}
