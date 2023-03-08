#include <FileMF.hpp>

FileOperator::Arr FileOperator::readChunk(std::ifstream inFile, size_t bRead, size_t offset) {
	if (inFile.is_open()) {
		FileOperator::Arr bytes(bRead);
		inFile.seekg(offset);
		inFile.read(bytes.data(), bRead);

		return bytes;
	}

	return FileOperator::Arr{};
}

void FileOperator::writeChunk(std::ofstream outFile, FileOperator::Arr bytes) {
	if (outFile.is_open()) {
		outFile.write(bytes.data(), bytes.size());
	}

	return;
}

FileOperator::Directory FileOperator::makeFileList(std::filesystem::path dPath) {
	FileOperator::Directory dir = {};

	for (const std::filesystem::directory_entry& P : std::filesystem::recursive_directory_iterator(dPath)) {
		if (!P.is_directory()) {
			dir.push_back(P.path());
		}
	}

	return dir;
}

template<typename T>
T FileOperator::init(const char* fName) {
	return T(fName, std::ios::binary);
}