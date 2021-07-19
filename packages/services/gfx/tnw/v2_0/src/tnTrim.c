/**
 * @file tnCut.c, by solarhe at 2008/3/6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tnTrim.h"

/** Take the ' ' out of the string 
@param in, the string to be process.
@retval the string after processed.
*/
char *trim_left(char *in)
{
	int 	c = 0;
	char	*out;

	if (strlen(in) == 0) return NULL;

	while (in[c] == ' ') c++;

	if (c != 0) 
		return &in[c];
	else 
		return in;
}

/** Replace the 'what' with 'whereof' in the string 
@param in, the string to be process.
@param what, the character will be replaced.
@param whereof, the character will be replaced with.
@retval the string after processed.
*/
char *str_swap(char *in, char what, char whereof)
{
	int 	i;

	if (strlen(in) == 0) return NULL;
	for (i = 0; i < (int)strlen(in); i++)
		if (in[i] == what) in[i] = whereof;

	return in;
}

/** Take the ' ' out of the string 
@param in, the string to be process.
@retval the string after processed.
*/
char *trim(char *in)
{
	int 	c = 0;
	int	i = 0;
	int	id = 0;

	if (strlen(in) == 0) return NULL;

	while (c != (int)strlen(in)) {
		if (in[c] == '"') id =! id;
		if ((in[c] == ' ') && (!id)) {
			for (i = 0; i < (int)strlen(in) - c; i++)
				in[c + i] = in[c + i + 1];
			continue;
		}
		c++;
	}

	c = 0;
	while (c != (int)strlen(in)) {
		if ((in[c] == '"')) {
			for (i = 0; i < (int)strlen(in) - c; i++)
				in[c + i] = in[c + i + 1];
			continue;
		}
		c++;
	}

	return in;
}

/** return the numth string separated by 'sep'.' 
@param in, the string to be process.
@param out, the string to be retruned.
@param sep, the separate flag.
@param num, the serid.
*/
void cut_item(char *in, char *out, char sep, int num)
{
	int	i;
	int	n;
	int	c;

	for (c = 0, n = 0, i = 0; i < (int)strlen(in); i++) {
		if (in[i] == sep) n++;
		if (n >= num && in[i] != sep) out[c++] = in[i];
		if (n >= num && in[i + 1] == sep) {
			out[c] = 0;
			return;
		}
	}
	out[c] = 0;
	
	//if (out[0] == 0)
		//printf("Warning : incomplete parameter in line %d\n", line);
}

/**  return the number tansfrom from the numth string separaed by 'sep'.
@param in, the string to be process.
@param sep, the separate flag.
@param num, the serid.
@retval integer number
*/
int cut_item_int(char *in, char sep, int num)
{
	char tmp[512];
	cut_item(in, tmp, sep, num);
	return atoi(tmp);
}
