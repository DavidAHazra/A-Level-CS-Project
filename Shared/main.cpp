#include "GameHelpers.hpp"
#include "Application.hpp"

int main(int argc, char* argv[]) {
	srand(static_cast<unsigned int>(time(nullptr)));
    
#ifdef IS_WINDOWS
	if (argc >= 1) { EXE_PATH::get_instance().set(boost::filesystem::path(argv[0]).make_preferred()); }
	 FreeConsole();
#endif

	try {
        Application app;

		app.start();

	} catch (const std::runtime_error& exception) {
		std::cout << exception.what() << std::endl;
		int i;
		std::cin >> i;
		return EXIT_FAILURE;
	}
    
	return EXIT_SUCCESS;
}
