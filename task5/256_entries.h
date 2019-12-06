// my_predictor.h
// This file contains a sample my_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.
// Note that this predictor doesn't use the whole 32 kilobytes available
// for the CBP-2 contest; it is just an example.

class my_update : public branch_update {
public:
	unsigned int index;
};

class my_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH	4 // Keep track of the last HISTORY_LENGTH branches
#define TABLE_BITS	8
	my_update u;
	branch_info bi;
	unsigned int branch_history[1<<TABLE_BITS]; // keeps track of history for different branches
	unsigned char predictors[1<<TABLE_BITS][1<<HISTORY_LENGTH];

	my_predictor (void) {
		memset(branch_history, 0, sizeof(branch_history));
		memset (predictors, 0, sizeof (predictors));
	}

	branch_update *predict (branch_info & b) {
		bi = b;

		// BR_CONDITIONAL == 1
		// Checking to make sure it is a valid branch
		if (b.br_flags & BR_CONDITIONAL) {
			// b.address[6:2] & 0b0001_1111
			u.index = (b.address >> 2) & ((1<<TABLE_BITS)-1);
			unsigned int history = branch_history[u.index];

			// predictor first based on the branch address, then on the history of that branch
			u.direction_prediction(predictors[u.index][history&((1<<HISTORY_LENGTH)-1)] >> 1);

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
			unsigned int index = ((my_update*)u)->index;
			unsigned int history = branch_history[index];
			c = &predictors[index][history&((1<<HISTORY_LENGTH)-1)];

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

			branch_history[index] <<= 1; // Make space for update in history
			branch_history[index] |= taken; // if branch is taken, last bit in history = 1
			// Only keep track of history in past HISTORY_LENGTH bits
			branch_history[index] &= (1<<HISTORY_LENGTH)-1;



		}
	}
};
