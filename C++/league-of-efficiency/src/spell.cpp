#include "spell.h"

/**
 * Calculate "efficiency" of a spell (aka total).
 */
void Spell::calculate() {
	float totalAD, totalAP, totalBonusAD, totalCD;
	float totalDPS;

	totalAD = adRatio * ad;
	totalAP = apRatio * ap;
	totalBonusAD = adBonusRatio * (ad - baseAttack);
	totalCD = cooldown - (cooldown * cdr);
	totalDPS = baseDamage + totalAD + totalAP + totalBonusAD;
	total = totalDPS / totalCD;
}

/**
 * Print spell data.
 */
void Spell::print(std::ostream &ostr) {
	ostr << std::endl;
	ostr << champName << "'s " << spellName << " - ";
	ostr << total << " dps" << std::endl;
	ostr << "  └(" << baseDamage << ") + (";
	ostr << apRatio << " * " << ap << " AP) + (";
	ostr << adRatio << " * " << ad << " AD) + (";
	ostr << adBonusRatio << " * " << (ad - baseAttack) << " bonus AD)" << std::endl;
}
