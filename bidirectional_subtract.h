#ifndef BIDIRECTIONAL_SUBTRACT_H
#define BIDIRECTIONAL_SUBTRACT_H

#include <algorithm>

template <class ForwardIterator1, class ForwardIterator2, class OutputIterator1, class OutputIterator2>
void bidirectional_subtract(ForwardIterator1 first1, ForwardIterator1 last1,
			  ForwardIterator2 first2, ForwardIterator2 last2,
			  OutputIterator1 aMinusB, OutputIterator2 bMinusA)	{
	while(true)	{
		if(first1 == last1) { std::copy(first2, last2, bMinusA); return; }
		if(first2 == last2) { std::copy(first1, last1, aMinusB); return; }
		if(*first1 < *first2)	{
				*aMinusB++ = *first1++;
		}
		else	{
			if(*first2 < *first1)	{
				*bMinusA++ = *first2++;
			}
			else	{ // ==
				++first1;
				++first2;
			}
		}
	}
}

template <class ForwardIterator1, class ForwardIterator2, class OutputIterator1, class OutputIterator2, class Comp>
void bidirectional_subtract(ForwardIterator1 first1, ForwardIterator1 last1,
			  ForwardIterator2 first2, ForwardIterator2 last2,
			  OutputIterator1 aMinusB, OutputIterator2 bMinusA,
							Comp comp)	{
	while(true)	{
		if(first1 == last1) { std::copy(first2, last2, bMinusA); return; }
		if(first2 == last2) { std::copy(first1, last1, aMinusB); return; }
		if(comp(*first1,*first2) )	{
				*aMinusB++ = *first1++;
		}
		else	{
			if(comp(*first2, *first1))	{
				*bMinusA++ = *first2++;
			}
			else	{ // ==
				++first1;
				++first2;
			}
		}
	}
}
#endif
