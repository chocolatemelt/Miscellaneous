#ifndef JSONUTILS_H
#define JSONUTILS_H

#include "spell.h"
#include "json/json.h"

namespace JsonUtils {
	Json::Value jsonify(const std::string, std::ostream &);
	Spell parseSpell(const Json::Value, const Json::Value, const float, const float, const float);
}

#endif
