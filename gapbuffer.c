/* The gapbuffer is an implementation of a sequence.
 *
 * The interface of a sequence is defined in sequence.h
 */

#include "sequence.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct sequence {
	char *buf;
	int gap_left;
	int gap_right;
	int size;
	int cap;
};

static void move_gap(struct sequence *seq, size_t pos)
{
	/* move left */
	while (pos < seq->gap_left)
		seq->buf[seq->gap_right--] = seq->buf[--seq->gap_left];

	/* move right */
	while (pos > seq->gap_left)
		seq->buf[seq->gap_left++] = seq->buf[++seq->gap_right];
}


static void grow_gap(struct sequence *seq, int grow_by)
{
	if (seq->size + grow_by >= seq->cap) {
		seq->buf = realloc(seq->buf, sizeof(*seq->buf) * (seq->cap + grow_by));
		seq->cap += grow_by;
	}

	memmove(&seq->buf[seq->gap_right+grow_by+1], &seq->buf[seq->gap_right+1], seq->size - seq->gap_right);

	seq->size += grow_by;
	seq->gap_right += grow_by;
}


enum seq_result sequence_open(struct sequence **ppseq, const char *filename)
{
	*ppseq = malloc(sizeof(struct sequence));
	struct sequence *pseq = *ppseq;

	pseq->size = 10;
	pseq->cap = 100;
	pseq->gap_left = 0;
	pseq->gap_right = pseq->size - 1;
	pseq->buf = malloc(sizeof(*pseq->buf) * pseq->cap);

	return SEQ_SUCCESS;
}


enum seq_result sequence_close(struct sequence *seq)
{
	free(seq->buf);
	free(seq);

	return SEQ_SUCCESS;
}


enum seq_result sequence_insert(struct sequence *seq, size_t pos, char c)
{
	if (pos != seq->gap_left)
		move_gap(seq, pos);

	if (seq->gap_right == seq->gap_left)
		grow_gap(seq, 10);

	seq->gap_left++;
	seq->buf[pos] = c;

	return SEQ_SUCCESS;
}


enum seq_result sequence_delete(struct sequence *seq, size_t pos)
{
	if (seq->gap_left != pos)
		move_gap(seq, pos);

	seq->gap_right++;
	return SEQ_SUCCESS;
}


int sequence_item_at(const struct sequence *seq, size_t pos)
{
	const size_t gap_size = seq->gap_right - seq->gap_left + 1;

	if (pos + gap_size >= seq->size)
		return EOF;

	if (pos < seq->gap_left)
		return seq->buf[pos];
	else 
		return seq->buf[pos + gap_size];
}

