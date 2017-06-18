#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <string>

#include "utils.h"

/**
 * Validates the ap, ad and cdr values.
 * CDR can be input as a decimal or an integer; thus, the valid ranges are 0-0.4 and 0-40.
 * A value of 1 is taken to be equivalent to a value of 0.01.
 */
int Utils::validateInput(const float ap, const float ad, const float cdr, std::ostream &ostr) {
	if(ap < 0) {
		ostr << "Ability Power cannot be negative. Check your arguments and try again." << std::endl;
		return 1;
	}
	else if(ad < 0) {
		ostr << "Attack Damage cannot be negative. Check your arguments and try again." << std::endl;
		return 1;
	}
	else if(cdr < 0) {
		ostr << "Cooldown Reduction cannot be negative. Check your arguments and try again." << std::endl;
		return 1;
	}
	else if(cdr > 40) {
		ostr << "Cooldown Reduction caps at 40%. Check your arguments and try again." << std::endl;
		return 1;
	}
	else if(cdr < 1) {
		// floating point comparisons; pretty neglible beyond this point
		if((cdr - 0.4) > 0.0000001) {
			ostr << "Cooldown Reduction caps at 40%. Check your arguments and try again." << std::endl;
			return 1;
		}
	}
	return 0;
}

/**
 * This is a callback function that stores the downloaded data from getData() into a string.
 */
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

/**
 * Generic function to download API data from Riot.
 * Takes in a url string, an API key, and a stream to print errors to.
 */
std::string Utils::getData(const std::string url, const std::string key, std::ostream &ostr) {
	CURL *curl;
	CURLcode res;
	std::string buf;

	curl = curl_easy_init();
	if(curl) {
		std::string urlData = url + key;
		curl_easy_setopt(curl, CURLOPT_URL, urlData.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		// on error, print error and exit
		if(res != 0) {
			ostr << "libcURL: " << curl_easy_strerror(res) << " (" << res << ")" << std::endl;
			return "";
		}

		// otherwise return the data
		return buf;
  }

  // curl failed
  ostr << "libcURL: failed to initialize (call to curl_easy_init() failed)" << std::endl;
	return "";
}

/**
 * Generic function to read from a file and store as a string,
 * Takes in a path to the file, but does no error checking.
 */
std::string Utils::readFile(std::ifstream &ifs) {
	std::stringstream sstr;
	sstr << ifs.rdbuf();
	return sstr.str();
}
