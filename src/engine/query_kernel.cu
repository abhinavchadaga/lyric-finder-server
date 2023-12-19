#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <thrust/device_vector.h>
#include <thrust/fill.h>
#include <thrust/remove.h>

#include <cstdlib>

#include "engine/query_kernel.h"
#include "engine/search_engine_gpu.hpp"

__device__ bool is_lyric_in_song(const char *lyric, const char *song) {
  int i = 0;
  int j = 0;

  while (lyric[i] != '\0' && song[j] != '\0') {
    if (lyric[i] == song[j]) {
      i++;
      j++;
    } else {
      i = 0;
      j++;
    }
  }

  if (lyric[i] == '\0') {
    return true;
  } else {
    // reached end of song without finding full lyric
    return false;
  }
}

__global__ void query_kernel(const char *lyric_to_search, const char *d_lyrics,
                             const int *d_song_indices, int num_songs,
                             int *d_results) {
  int song_index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  for (int i = song_index; i < num_songs; i += stride) {
    if (is_lyric_in_song(lyric_to_search, d_lyrics + d_song_indices[i])) {
      d_results[i] = i;
    }
  }
}

void launch_query_kernel(const char *lyric_to_search, int lyric_len,
                         const char *d_lyrics, const int *d_song_indices,
                         int num_songs, int **h_results, int *num_results,
                         float *kernel_time_ms, float *data_transfer_time_ms) {
  char *d_lyric_to_search;

  float total_data_transfer_time_ms = 0.0, dt_ms = 0.0;
  cudaEvent_t data_transfer_start, data_transfer_stop;
  cudaEventCreate(&data_transfer_start);
  cudaEventCreate(&data_transfer_stop);

  cudaEventRecord(data_transfer_start);
  gpuErrchk(
      cudaMalloc((void **)&d_lyric_to_search, (lyric_len + 1) * sizeof(char)));
  gpuErrchk(cudaMemcpy(d_lyric_to_search, lyric_to_search,
                       (lyric_len + 1) * sizeof(char), cudaMemcpyHostToDevice));
  cudaEventRecord(data_transfer_stop);
  gpuErrchk(cudaDeviceSynchronize());
  cudaEventElapsedTime(&dt_ms, data_transfer_start, data_transfer_stop);
  total_data_transfer_time_ms += dt_ms;

  thrust::device_vector<int> d_results(num_songs, -1);

  int threads_per_block = 128;
  int num_blocks = (num_songs + threads_per_block - 1) / threads_per_block;

  cudaEvent_t kernel_start, kernel_stop;
  cudaEventCreate(&kernel_start);
  cudaEventCreate(&kernel_stop);

  cudaEventRecord(kernel_start);
  query_kernel<<<num_blocks, threads_per_block>>>(
      d_lyric_to_search, d_lyrics, d_song_indices, num_songs,
      thrust::raw_pointer_cast(d_results.data()));
  cudaEventRecord(kernel_stop);
  gpuErrchk(cudaDeviceSynchronize());
  cudaEventElapsedTime(kernel_time_ms, kernel_start, kernel_stop);

  // reduce results to only valid indices
  // using gpu
  auto new_end = thrust::remove(d_results.begin(), d_results.end(), -1);
  d_results.erase(new_end, d_results.end());

  cudaEventRecord(data_transfer_start);
  *h_results = (int *)malloc(d_results.size() * sizeof(int));
  gpuErrchk(cudaMemcpy(*h_results, thrust::raw_pointer_cast(d_results.data()),
                       d_results.size() * sizeof(int), cudaMemcpyDeviceToHost));
  *num_results = d_results.size();
  cudaEventRecord(data_transfer_stop);
  gpuErrchk(cudaDeviceSynchronize());
  cudaEventElapsedTime(&dt_ms, data_transfer_start, data_transfer_stop);
  total_data_transfer_time_ms += dt_ms;
  *data_transfer_time_ms = total_data_transfer_time_ms;

  // cleanup
  gpuErrchk(cudaFree(d_lyric_to_search));
  cudaEventDestroy(data_transfer_start);
  cudaEventDestroy(data_transfer_stop);
  cudaEventDestroy(kernel_start);
  cudaEventDestroy(kernel_stop);
}