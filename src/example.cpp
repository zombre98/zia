#include <iostream>

int function() {
  /*
	nkpp::Utils::FileWatcher file(".", std::chrono::milliseconds(1000));
	std::thread fileWatcherProcess([&file](){
		file.watch([](const std::string &, nkpp::Utils::FileWatcher::State s){
			switch (s) {
				case nkpp::Utils::FileWatcher::State::MODIFIED:
					std::cout << "Modified" << std::endl;
					break;
				case nkpp::Utils::FileWatcher::State::CREATED:
					std::cout << "Created" << std::endl;
					break;
				case nkpp::Utils::FileWatcher::State::DELETED:
					std::cout << "Deleted" << std::endl;
					break;
			}
		});
	});
  file.stop();
  fileWatcherProcess.join();
   */

	std::cout << "Hello world!" << std::endl;
	return 0;
}
