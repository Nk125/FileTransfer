#include <FileMF.hpp>

bool FileOperator::readChunk(FileOperator::Portal& File, FileOperator::Arr& arr, size_t bRead, size_t offset) {
	arr.assign(bRead, 0x00);

	if (File.is_open()) {
		File.seekg(offset);
		File.read(&arr[0], bRead);

		arr.resize(File.gcount());

		return true;
	}
	else {
		Except("File read error: file descriptor closed");
	}

	return false;
}

bool FileOperator::writeChunk(FileOperator::Portal& File, const Arr& arr, size_t size) {
	if (File.is_open()) {
		File.write(arr.data(), size);

		return true;
	}
	else {
		Except("File write error: file descriptor closed");
	}

	return false;
}

FileOperator::Directory FileOperator::makeFileList(std::filesystem::path dPath, std::regex exc, std::regex inc, size_t min, size_t max) {
	FileOperator::Directory dir;

	if (std::filesystem::is_directory(dPath)) {
		try {
			std::filesystem::current_path(dPath);

			for (const std::filesystem::directory_entry& P : std::filesystem::recursive_directory_iterator(dPath)) {
				if (!P.is_directory()) {
					std::string relPath = P.path().string();
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
			Except("Invalid directory path", false);
		}
	}
	else {
		try {
			if (dPath.has_parent_path()) {
				std::filesystem::current_path(dPath.parent_path());
			}

			dir[dPath] = std::filesystem::file_size(dPath);
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