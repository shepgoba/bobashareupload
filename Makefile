APP_NAME = bobashareupload
CXX = g++
CXXFLAGS = -pedantic -Wall -Wextra -O3 -lcurl -std=c++20

$(APP_NAME): src/main.cpp
	$(CXX) src/main.cpp src/MimeTypes.cpp -o $(APP_NAME) $(CXXFLAGS)
