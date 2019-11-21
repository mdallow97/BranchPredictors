// my_predictor.h

class my_update : public branch_update {
public:
	unsigned int index;
};

class my_predictor : public branch_predictor {
public:
#define TABLE_BITS	5
	my_update u;
	branch_info bi;
	unsigned char table[1<<TABLE_BITS]; // 0000_0000 0000_0000 0000_0000 0010_0000 == 32 = 2^5

	branch_update *predict (branch_info & b) {
		bi = b;

		// BR_CONDITIONAL == 1
		// Checking to make sure it is a valid branch
		if (b.br_flags & BR_CONDITIONAL) {
			// b.address[6:2] & 0b0001_1111
			u.index = (b.address >> 2) & ((1<<TABLE_BITS)-1);

			// 000 <= table[i] <= 111
			// prediction is true if table[i] == 111 or 110 or 101 or 100
			// prediction is false if table[i] == 000 or 001 or 010 or 011
			u.direction_prediction (table[u.index] >> 2); 
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	// Updates the predictor table
	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {

			// 000 <= c <= 111
			// 3-bit predictor (0,3)
			unsigned char *c = &table[((my_update*)u)->index];
			if (taken) {
				if (*c < 7) (*c)++;
			} else {
				if (*c > 0) (*c)--;
			}
		}
	}
};
