#include <curl/curl.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>

#include "utils.h"
#include "spell.h"
#include "json/json.h"
#include "jsonutils.h"

int main(int argc, char *argv[]) {
	// program variables
	const std::string riotChampData = "https://global.api.pvp.net/api/lol/static-data/na/v1.2/champion?champData=stats&api_key=";
	const std::string riotSpellData = "https://global.api.pvp.net/api/lol/static-data/na/v1.2/champion?champData=spells&api_key=";
	std::vector<Spell> spellList;
	std::ifstream ifs;
	std::ofstream ofs;

	// argument variables
	size_t listSize = 1; // -l switch; defaults to 1 (only the most efficient spell)
	float ap = -1, ad, cdr;
	std::string outputPath = "";

	// handle arguments, if any
	if(argc > 9) {
		std::cerr << "Too many arguments! Try " << argv[0] << " -h for usage details." << std::endl;
		return EXIT_FAILURE;
	}
	else if(argc > 0) {
		for(int i = 1; i < argc; ++i) {
			// display help
			if(strcmp(argv[i], "-h") == 0) {
				std::cout << "USAGE: " << argv[0] << " [-h] [-i AP AD CDR] [-l INT >= 1] [-o FILE]" << std::endl;
				std::cout << std::endl << "NOTE:  CDR may be input as a percentage (e.g. 40) or decimal (e.g. 0.4)." << std::endl;
				std::cout << "Inputs >= 1 will be read as percentages." << std::endl;
				return EXIT_SUCCESS;
			}

			// get input data
			else if(strcmp(argv[i], "-i") == 0) {
				if(argc < i + 3) {
					std::cerr << "Too little arguments for -i switch! Check your arguments and try again." << std::endl;
					return EXIT_FAILURE;
				}

				// store ap, ad, cdr
				// currently fails if any of the following 3 arguments aren't numbers...
				std::string tmps(argv[++i]);
				ap = stof(tmps);
				tmps = std::string(argv[++i]);
				ad = stof(tmps);
				tmps = std::string(argv[++i]);
				cdr = stof(tmps);

				// check legitimacy
				if(Utils::validateInput(ap, ad, cdr, std::cerr)) return EXIT_FAILURE;

				// normalize cdr
				if(cdr > 0.4)
				cdr /= 100.0;
			}

			// get list size
			else if(strcmp(argv[i], "-l") == 0) {
				if(argc < i + 1) {
					std::cerr << "Too little arguments for -l switch! Check your arguments and try again." << std::endl;
					return EXIT_FAILURE;
				}

				std::string tmps(argv[++i]);
				listSize = stoi(tmps);

				if(listSize < 1) {
					std::cerr << "-l switch argument must be >= 1." << std::endl;
					return EXIT_FAILURE;
				}
			}

			// get output filepath
			else if(strcmp(argv[i], "-o") == 0) {
				if(argc < i + 1) {
					std::cerr << "Too little arguments for -o switch! Check your arguments and try again." << std::endl;
					return EXIT_FAILURE;
				}

				outputPath = std::string(argv[++i]);
			}

			// unknown flag
			else {
				std::cerr << "Unknown flag. Try " << argv[0] << " -h for usage details." << std::endl;
				return EXIT_FAILURE;
			}
		}
	}

	// check for api.key file
	// looks in current directory for now
	ifs.open("api.key", std::ifstream::in);
	if(!ifs.is_open()) {
		std::cerr << "Could not open api.key for reading." << std::endl;
		return EXIT_FAILURE;
	}

	// check availability of output filepath
	ofs.open(outputPath, std::ifstream::out);
	if(!ifs.is_open()) {
		std::cerr << "Could not open " << outputPath << " for writing." << std::endl;
		return EXIT_FAILURE;
	}

	// query user for parameters if not passed in
	// (only check ap because its either unset (< 0) or set (>= 0))
	if(ap < 0) {
		std::cout << "Enter AP value: ";
		std::cin >> ap;
		std::cout << "Enter AD value: ";
		std::cin >> ad;
		std::cout << "Enter CDR value: ";
		std::cin >> cdr;

		if(Utils::validateInput(ap, ad, cdr, std::cerr)) return EXIT_FAILURE;

		// normalize cdr
		if(cdr > 0.4)
		cdr /= 100.0;
	}

	// all good? begin processing!

	// initialize cURL
	curl_global_init(CURL_GLOBAL_ALL);

	// scrape & slurp the api data
	std::string apiKey = Utils::readFile(ifs);
	std::string riotChampAPI = Utils::getData(riotChampData, apiKey, std::cerr);
	std::string riotSpellAPI = Utils::getData(riotSpellData, apiKey, std::cerr);
	Json::Value riotChampJson = JsonUtils::jsonify(riotChampAPI, std::cerr);
	Json::Value riotSpellJson = JsonUtils::jsonify(riotSpellAPI, std::cerr);

	// process spell-by-spell, iterating through the champion and spell API data
	riotChampJson = riotChampJson["data"];
	riotSpellJson = riotSpellJson["data"];
	Json::ValueIterator citr = riotChampJson.begin();
	Json::ValueIterator sitr = riotSpellJson.begin();

	while(citr != riotChampJson.end() || sitr != riotSpellJson.end()) {
		Json::Value champ  = (*citr);
		Json::Value spells = (*sitr)["spells"];
		for(Json::Value &spell : spells) {
			Spell tmpSpell = JsonUtils::parseSpell(champ, spell, ap, ad, cdr);
			if(tmpSpell.getType() == 'd') spellList.push_back(tmpSpell);
		}
		// advance key
		++citr;
		++sitr;
	}

	// sort the spell list...
	std::sort(spellList.begin(), spellList.end(), Utils::greater());

	// ...and print the most efficient spell(s)!
	int loops = std::min(listSize, spellList.size());
	for(int i = 0; i < loops; ++i) {
		if(ofs.is_open()) spellList[i].print(ofs);
		else spellList[i].print(std::cout);
	}

	// cleanup resources
	ifs.close();
	ifs.close();
	curl_global_cleanup();
	return 0;
}
