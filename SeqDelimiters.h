#ifndef SEQDELIMITER_H
#define SEQDELIMITER_H
#include <string>

const int N_PUNC_SEQ_DELIM = 6;
std::string PUNC_SEQ_DELIM_ARR[N_PUNC_SEQ_DELIM]{
	"\xEF\xBC\x8C", // ¡A
	"\xE3\x80\x82", // ¡C
	"\xEF\xBC\x9F", // ¡H
	"\xEF\xBC\x81", // ¡I
	"\xEF\xBC\x9A", // ¡G
	"\xEF\xBC\x9B"  // ¡F
};

const int N_PUNC_QUOTATION = 2;
std::string PUNC_QUOTATION_ARR[N_PUNC_QUOTATION]{
	"\xE3\x80\x8C", // ¡u
	"\xE3\x80\x8D"  // ¡v
};

#endif //SEQDELIMITER_H
