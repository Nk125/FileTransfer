#include <FileMF.hpp>

void FileOperator::readChunk(FileOperator::Portal& File, FileOperator::Arr& arr, size_t bRead, size_t offset) {
	arr.assign(bRead, 0x00);
	
	if (File.is_open()) {
		File.seekg(offset);
		File.read(&arr[0], bRead);
	}
}

void FileOperator::writeChunk(FileOperator::Portal& File, const Arr& bytes, size_t size) {
	if (File.is_open()) {
		File.write(bytes.data(), size);
	}
}

FileOperator::Directory FileOperator::makeFileList(std::filesystem::path dPath, std::regex exc, std::regex inc, size_t min, size_t max) {
	FileOperator::Directory dir;

	if (std::filesystem::is_directory(dPath)) {
		try {
			for (const std::filesystem::directory_entry& P : std::filesystem::recursive_directory_iterator(dPath)) {
				if (!P.is_directory()) {
					std::string relPath = P.path().relative_path().string();
					size_t fileSz = P.file_size();

					if (std::regex_match(relPath, exc)) continue;
					if (!std::regex_match(relPath, inc)) continue;
					if (fileSz < min) continue;
					if (fileSz > max) continue;
					
					dir[relPath] = fileSz;
				}
			}
		}
		catch (...) {
			Except("Invalid directory path", true);
		}
	}
	else {
		try {
			dir[dPath.relative_path()] = std::filesystem::file_size(dPath);
		}
		catch (...) {
			Except("Invalid file path", true);
		}
	}

	return dir;
}

FileOperator::Portal FileOperator::init(bool type, const char* fName) {
	return FileOperator::Portal(fName, std::ios::binary | (type ? std::ios::out : std::ios::in));
}