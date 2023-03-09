#pragma once
#include <filesystem>
#include <fstream>
#include <vector>

class FileOperator {
public:
	typedef std::vector<char> Arr;
	typedef std::vector<std::filesystem::path> Directory;
	typedef std::ofstream out;
	typedef std::ifstream in;

	Arr readChunk(std::ifstream, size_t, size_t);
	void writeChunk(std::ofstream, Arr);

	Directory makeFileList(std::filesystem::path);

	template <typename T>
	T init(const char*);
};