APP_NAME = bobashareupload
CXX = g++
CXXFLAGS = -pedantic -Wall -Wextra -O3 -lcurl

app: src/main.cpp
	$(CXX) src/main.cpp src/MimeTypes.cpp -o $(APP_NAME) $(CXXFLAGS) -std=c++20