#include <ostream>

/*
 * node definition
 */
typedef struct node {
	int val;
	struct node *next;
	struct node *prev;
} node;


class circular_ll {
	public:
		node *first;

		/*
		 * constructor
		 */
		circular_ll() {
			first = NULL;
		}

		/*
		 * insert ascending order
		 */
		void insert(int v) {
			// create initial node
			node *np = new node;
			np->val = v;
			if(!first) {
				first = np;
				first->next = np;
				first->prev = np;
			}
			// insert ascending
			else {
				// check head
				if(v < first->val) {
					node *tmp = first;
					first = np;
					first->next = tmp;
					first->prev = tmp->prev;
					tmp->prev->next = first;
					tmp->next->prev = first;
				}
				// look for insertion point
				node *ptr = first;
				while(ptr->next != first) {
					if(v < ptr->next->val) break;
					ptr = ptr->next;
				}
				// insert after ptr
				node *tmp = ptr->next;
				ptr->next = np;
				tmp->prev = np;
				np->next = tmp;
				np->prev = ptr;
			}
		}

		/*
		 * print list starting from first
		 */
		void print(std::ostream &ostr) {
			node *ptr = first;
			ostr << "This linked list contains:";
			if(!ptr) {
				ostr << " nothing.\n";
				return;
			}
			ostr << " " << ptr->val;
			while(ptr->next != first) {
				ptr = ptr->next;
				ostr << " " << ptr->val;
			}
			ostr << " → " << ptr->next->val;
			ostr << "\n";
		}

		/*
		 * destructor
		 */
		~circular_ll() {
			node *ptr = first;
			while(ptr->next != first) {
				node *p = ptr;
				ptr = ptr->next;
				delete p;
			}
			delete ptr;
			first = NULL;
		}
};
