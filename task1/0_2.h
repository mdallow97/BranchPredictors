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

			// 00 <= table[i] <= 11
			// prediction is true if table[i] == 11 or 10
			// prediction is false if table[i] == 00 or 01
			u.direction_prediction (table[u.index] >> 1); 
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	// Updates the predictor table
	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {

			// 00 <= c <= 11
			// 2-bit predictor
			unsigned char *c = &table[((my_update*)u)->index];
			if (taken) {
				if (*c == 0) *c = 1;
				else if (*c == 1) *c = 3;
				else if (*c == 2) *c = 3;
			} else {
				if (*c == 3) *c = 2;
				else if (*c == 2) *c = 0;
				else if (*c == 1) *c = 0;
			}
		}
	}
};
