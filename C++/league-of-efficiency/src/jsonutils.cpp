#include "jsonutils.h"

#include <regex>

/**
 * Parses a string as a Json::Value object.
 * Takes in a source string and an output stream for error printing.
 */
Json::Value JsonUtils::jsonify(const std::string src, std::ostream &ostr) {
	Json::Value root;
	Json::Reader reader;
	if(reader.parse(src, root, false) == false) {
		ostr << "Failed to parse API data." << reader.getFormattedErrorMessages() << std::endl;
		return EXIT_FAILURE;
	}

	return root;
}

/**
 * Parses a Json::Value object as a Spell object.
 */
Spell JsonUtils::parseSpell(const Json::Value champJson, const Json::Value spellJson, const float ap, const float ad, const float cdr) {
	// initialize variables
	// note: baseAttack refers to champion's natural attack damage at lvl 18
	//       baseDamage refers to the spell's base damage at max rank
	Spell ret(ap, ad, cdr);
	std::string champName, spellName;
	int maxRank, baseDamage = 0;
	float cooldown, baseAttack, apRatio = 0.0, adRatio = 0.0, adBonusRatio = 0.0;
	char type = 'n'; // spell type (damage, healing, neither)

	// parse and store initial spell values
	champName = champJson["name"].asString();
	spellName = spellJson["name"].asString();
	maxRank = spellJson["maxrank"].asInt();
	cooldown = spellJson["cooldown"][maxRank - 1].asFloat();

	// getting the level 18 base attack damage (ad) of a champion is calculated as:
	// baseAttack = starting ad + ad/level * 17
	baseAttack = champJson["stats"]["attackdamage"].asFloat() +
		           champJson["stats"]["attackdamageperlevel"].asFloat() * 17;

	// getting the base damage of a spell at max rank requires some magic
	// we look for the "damage" level tooltip and take the index
	// from the associated number (e.g. "e1" being index 1)
	Json::Value finder = spellJson["leveltip"];
	std::string eff;
	int lvlIdx = -1, effIdx = 1;
	for(int i = 0; i < finder["label"].size(); ++i) {
		std::string found = finder["label"][i].asString();
		// perform regex
		std::regex r(".*Damage");
		if(std::regex_match(found, r)) {
			lvlIdx = i;
			break;
		}
	}
	if(lvlIdx != -1) {
		eff = finder["effect"][lvlIdx].asString();
		effIdx = eff[4] - '0';
		baseDamage = spellJson["effect"][effIdx][maxRank - 1].asInt();
		type = 'd';
	}

	// getting ratios can be tricky...
	// we only look at the first two indices because later indices contain
	// ratios for different portions of the spell that we don't consider
	Json::Value ratios = spellJson["vars"];
	for(int i = 0; i < 2; ++i) {
		std::string link = ratios[i]["link"].asString();
		if(link == "attackdamage") adRatio = ratios[i]["coeff"][0].asFloat();
		else if(link == "bonusattackdamage") adBonusRatio = ratios[i]["coeff"][0].asFloat();
		else if(link == "spelldamage") apRatio = ratios[i]["coeff"][0].asFloat();
	}

	// final check: cooldown isn't 0 (divide by zero error)
	if(cooldown == 0.0) type = 'n';

	// store relevant data and calculate efficiency
	ret.setSpellInfo(champName, spellName, type);
	ret.setSpellData(baseAttack, baseDamage, cooldown, apRatio, adRatio, adBonusRatio);
	ret.calculate();

	return ret;
}
