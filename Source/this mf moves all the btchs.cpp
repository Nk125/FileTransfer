#include <ArgMF.h>
#include <ConManager.hpp>
#ifdef WINDOWS_SYSTEM
#pragma comment(lib, "ws2_32")
#endif

int main(int argc, char* argv[]) {
	argh::parser args(argc, argv, argh::parser::SINGLE_DASH_IS_MULTIFLAG);
}