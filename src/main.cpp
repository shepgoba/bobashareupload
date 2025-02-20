#include <curl/curl.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <format>
#include "MimeTypes.h"
#include "json.hpp"

constexpr const char *SERVER_URL = "https://share.boba.best";

static size_t write_callback(void *ptr, size_t size, size_t nmemb, std::string *data) {
    data->append((char *)ptr, size * nmemb);
    return size * nmemb;
}

static bool get_file_data(const std::string &path, std::vector<char> &data)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
		return false;

	data = std::vector(
		(std::istreambuf_iterator<char>(file)), 
		std::istreambuf_iterator<char>());

	return true;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cout << "Usage: bobashareupload <file path>\n";
		return 1;
	}
	curl_global_init(CURL_GLOBAL_ALL);

	CURL *curl = curl_easy_init();
	if (!curl) {
		std::cout << "Failed to initialize libCURL\n";
		return 1;
	}
	std::string file_path = argv[1];
	std::vector<char> file_data;
	if (!get_file_data(file_path, file_data)) {
		std::cout << std::format("Failed to get file data for '{}'\n", file_path);
		return 1;
	}

	std::filesystem::path path_object(file_path);
	std::string post_url = SERVER_URL + std::string("/api/v1/upload/") + path_object.filename().string();
	
	// set headers
	std::string mime_type = MimeTypes::getType(file_path.c_str());
	struct curl_slist *headers = nullptr;
	headers = curl_slist_append(headers, ("Content-Type: " + mime_type).c_str());


	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, file_data.data());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, file_data.size());
	curl_easy_setopt(curl, CURLOPT_URL, post_url.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string response_data;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
	}

	long response_code;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

	if (response_code == 201) {
		nlohmann::json response_json = nlohmann::json::parse(response_data);
		std::string response_url = response_json["url"];
		std::cout << "File uploaded successfully!\n";
		std::cout << "File URL: " << response_url << "\n";
	} else {
		std::cerr << "Failed to upload file. HTTP code: " << response_code << std::endl;
	}

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return 0;
}