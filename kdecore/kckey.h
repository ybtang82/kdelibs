// This file has been automatically genrated by "generate_keys.sh" 

typedef struct { 
	const char *name; 
	int code; 
} KKeys; 

#define MAX_KEY_LENGTH           15   // should be calculated (gawk ?) 
#define MAX_KEY_MODIFIER_LENGTH   5   // SHIFT CRTL ALT 
#define NB_KEYS                 233 
extern const KKeys KKEYS[NB_KEYS];
