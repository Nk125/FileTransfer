#pragma once
#include <ErrorMF.hpp>
#include <filesystem>
#include <fstream>
#include <map>
#include <regex>
#include <vector>

class FileOperator {
public:
	typedef std::vector<char> Arr;
	// File path : File size
	typedef std::map<std::filesystem::path, size_t> Directory;
	typedef std::fstream Portal;
	const static bool Out = true;
	const static bool In = false;

	static void readChunk(Portal&, Arr&, size_t, size_t);
	static void writeChunk(Portal&, const Arr&, size_t);

	static Directory makeFileList(std::filesystem::path, std::regex, std::regex, size_t, size_t);

	static Portal init(bool, const char*);
};