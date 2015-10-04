
#define TEXT_SZ	2048

struct shared_use_st {
	int has_data;	/* notify consumer has data */
	char some_text[TEXT_SZ];
};

