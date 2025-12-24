#pragma once

#include <stddef.h>

enum seq_result {
	SEQ_SUCCESS,
};

struct sequence;

enum seq_result sequence_open(struct sequence **seq, const char *filename);
enum seq_result sequence_close(struct sequence *seq);

enum seq_result sequence_insert(struct sequence *seq, size_t pos, char c);
enum seq_result sequence_delete(struct sequence *seq, size_t pos);

int sequence_item_at(const struct sequence *seq, size_t pos);
