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

			// 0000 <= table[i] <= 1111
			// prediction is true if table[i] == 1111 or 1110 or 1101 or 1011 or 1100 or 1001 or 1000
			// prediction is false if table[i] == 0000 or 0001 or 0010 or 0100 or 0011 or 0110 or 0101 or 0111
			u.direction_prediction (table[u.index] >> 3); 
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	// Updates the predictor table
	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {

			// 0000 <= c <= 1111
			// 4-bit predictor (0,4)
			unsigned char *c = &table[((my_update*)u)->index];
			if (taken) {
				if (*c < 15) (*c)++;
			} else {
				if (*c > 0) (*c)--;
			}
		}
	}
};
