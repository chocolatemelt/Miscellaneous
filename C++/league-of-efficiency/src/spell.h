#ifndef SPELL_H
#define SPELL_H

#include <ostream>

/**
 * Spell object
 * Stores data pertaining to an offensive or healing spell.
 * Implements comparisons as well as a print function for program output.
 */
class Spell {
	private:
		// identification & print data
		std::string champName;
		std::string spellName;
		char type;

		// calculation data
		float ap, ad, cdr;
		float baseAttack, cooldown;
		int baseDamage;
		float apRatio, adRatio, adBonusRatio;
		float total;

	public:
		// constructor
		Spell() {}
		Spell(float p, float d, float c) {
			ap = p;
			ad = d;
			cdr = c;
		}

		// accessor / mutator functions
		std::string getSpell() const { return spellName; }
		char getType()         const { return type; }
		float getTotal()       const { return total; }

		void setParams(float p, float d, float c) {
			ap = p;
			ad = d;
			cdr = c;
		}
		void setSpellInfo(std::string c, std::string s, char t) {
			champName = c;
			spellName = s;
			type = t;
		}
		void setSpellData(float ba, int bd, float c, float p, float d, float a) {
			baseAttack = ba;
			baseDamage = bd;
			cooldown = c;
			apRatio = p;
			adRatio = d;
			adBonusRatio = a;
		}

		// public functions
		void calculate();
		void print(std::ostream &);

		// overloads
		friend bool operator< (const Spell &l, const Spell &r){return l.getTotal() < r.getTotal();}
		friend bool operator> (const Spell &l, const Spell &r){return r < l;}
		friend bool operator<=(const Spell &l, const Spell &r){return !(l > r);}
		friend bool operator>=(const Spell &l, const Spell &r){return !(l < r);}
};

#endif
