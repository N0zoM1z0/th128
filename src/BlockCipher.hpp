#pragma once

// THA1/replay block cipher. Each block is written back to front, odd
// positions first, as plain = cipher ^ key with key += step per byte. A short
// tail (less than a quarter block) and an odd final byte stay unencrypted,
// and decoding stops after `limit` bytes. Returns `data`.
unsigned char *__stdcall DecryptBlocks(unsigned char *data, int size, unsigned char key,
                                       unsigned char step, int block, int limit);
