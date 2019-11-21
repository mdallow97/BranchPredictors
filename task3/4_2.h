// my_predictor.h

class my_update : public branch_update {
public:
	unsigned int index;
};

class my_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH	4 // Keep track of the last 2^HISTORY_LENGTH branches
#define TABLE_BITS	5
	my_update u;
	branch_info bi;
	unsigned int history; // SHIFT register
	unsigned char predictors[1<<HISTORY_LENGTH][1<<TABLE_BITS]; // [2^HISTORY_LENGTH][2^5]

	my_predictor (void) : history(0) { 
		memset (predictors, 0, sizeof (predictors));
	}

	branch_update *predict (branch_info & b) {
		bi = b;

		// BR_CONDITIONAL == 1
		// Checking to make sure it is a valid branch
		if (b.br_flags & BR_CONDITIONAL) {
			// b.address[6:2] & 0b0001_1111
			// Need 5 bits of the address because we have 32 (2^5) entries in BHT
			u.index = (b.address >> 2) & ((1<<TABLE_BITS)-1);

			// prediction is true if table[i] == 11 or 10, else false
			// Uses history as shift register to pick a predictor
			u.direction_prediction(predictors[history&((1<<HISTORY_LENGTH)-1)][u.index] >> 1);

		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	// Updates the predictor table
	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {

			

			unsigned char *c;

			// predictor to be updated is the one corresponding to history before its updated
			c = &predictors[history&((1<<HISTORY_LENGTH)-1)][((my_update*)u)->index];

			// 2-bit predictor
			// 00 <= c <= 11
			if (taken) {
				if (*c == 0) *c = 1;
				else if (*c == 1) *c = 3;
				else if (*c == 2) *c = 3;
			} else {
				if (*c == 3) *c = 2;
				else if (*c == 2) *c = 0;
				else if (*c == 1) *c = 0;
			}

			history <<= 1; // Make space for update in history
			history |= taken; // if branch is taken, last bit in history = 1
			// Only keep track of history in past HISTORY_LENGTH bits
			history &= (1<<HISTORY_LENGTH)-1; 


			
		}
	}
};
