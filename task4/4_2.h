// my_predictor.h

class my_update : public branch_update {
public:
	unsigned int index;
};

class my_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH	4
	my_update u;
	branch_info bi;
	unsigned int history; 
	unsigned char predictor[1<<HISTORY_LENGTH]; // 2^HISTORY_LENGTH

	my_predictor (void) : history(0) { 
		memset (predictor, 0, sizeof (predictor));
	}

	branch_update *predict (branch_info & b) {
		bi = b;

		// BR_CONDITIONAL == 1
		// Checking to make sure it is a valid branch
		if (b.br_flags & BR_CONDITIONAL) {
			// history & 2^HISTORY_LENGTH - 1
			// Use history as a shift register
			u.index = history&((1<<HISTORY_LENGTH)-1);

			// 00 <= predictor[i] <= 11
			// prediction is true if table[i] == 11 or 10
			// prediction is false if table[i] == 00 or 01

			u.direction_prediction(predictor[u.index] >> 1);

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

			// update the predictor with outcome
			c = &predictor[((my_update*)u)->index];

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
