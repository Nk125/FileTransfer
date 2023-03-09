#pragma once
#include <iostream>

class Except {
protected:
	void selfd(bool act) {
		if (act) std::exit(1);
	}

	const char* prefix = " "
#ifdef WINDOWS_SYSTEM
		"WINDOWS"
#else
		"LINUX"
#endif
		": ";

	void display(bool fatal = false, const char* content = "No description provided") {
		std::cerr << (fatal ? "[FATAL]" : "[ERROR]") << prefix << content << "\n";
		selfd(fatal);
	}

public:
	Except(const char* content, bool fatal = false) {
		display(fatal, content);
	}

	Except(bool fatal = false) {
		diplay(fatal);
	}
};