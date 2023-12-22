#ifndef QUERY_KERNEL_H
#define QUERY_KERNEL_H

void launch_query_kernel(const char *lyric_to_search, int lyric_len,
                         const char *d_lyrics, const int *d_song_indices,
                         int num_songs, int **h_results, int *num_results,
                         float *kernel_time_ms, float *data_transfer_time_ms);

#endif