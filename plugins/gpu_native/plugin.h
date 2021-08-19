// PSX GPU plugin interface.

#pragma once
#include <stdint.h>
#include "../../frontend/plugin_lib.h"

// GPU plugin interface result codes.
enum { GPU_RESULT_SUCCESS = 0, GPU_RESULT_FAILURE = -1 };

// Emulator settings and callbacks.
typedef struct rearmed_cbs gpu_callbacks_t;

// GPU save state type. Must match the one in the core.
typedef struct {
	uint32_t ulFreezeVersion;
	uint32_t ulStatus;
	uint32_t ulControl[256];
	char psxVRam[1024*512*2];
} gpu_freeze_data_t;


// Initialize the GPU plugin. Returns GPU_RESULT_SUCCESS on success. Called
// during a separate init phase before @GPUopen. Originally used for loading
// plugins dynamically, now best left unused.
extern long GPUinit(void);

// Shut down the GPU plugin. The return value is ignored. Called during
// a separate shutdown phase after @GPUclose. Best left unused.
extern long GPUshutdown(void);

// Initialize the emulated GPU instance. Returns GPU_RESULT_SUCCESS on success.
// The arguments should be ignored. Called exactly once per game session. The
// callbacks are not available for this function. Originally used by plugins
// to open their own windows for content.
extern long GPUopen(unsigned long *_unk1, char *_unk2, char *_unk3);

// Terminate the emulated GPU instance. The return value is ignored.
extern long GPUclose(void);

// Update the emulator settings and callbacks. May be called at any point
// between @GPUopen and @GPUclose.
extern void GPUrearmedCallbacks(const gpu_callbacks_t *cbs);

// Read the GP0 or GP1 command response word.
extern uint32_t GPUreadData(void);

// Read the Status Register word.
extern uint32_t GPUreadStatus(void);

// Do a DMA write of the specified number of words into the GP0 rendering
// and VRAM access port. The memory is read in the little-endian byte order.
extern void GPUwriteDataMem(uint32_t *base, int words);

// Do a chained DMA write to the GP0 rendering and VRAM access port. The base
// pointer must point to the start of the emulated PSX RAM. Returns the number
// of words written, for timing purposes. The memory is read in the little-
// endian byte order.
extern long GPUdmaChain(uint32_t *base, uint32_t offset);

// Do a DMA read of the specified number of words from the GPU. The memory
// is written in the little-endian byte order.
extern void GPUreadDataMem(uint32_t *ptr, int words);

// Write a word to the GP0 Rendering and VRAM access port. The word is read
// in the host byte order.
extern void GPUwriteData(uint32_t word);

// Write a word to the GP1 Display Control port. The word is read in the host
// byte order.
extern void GPUwriteStatus(uint32_t word);

// Load or save the emulated GPU state using the provided struct. The return
// value is ignored, but should be set to 1 on success.
extern long GPUfreeze(uint32_t is_save, gpu_freeze_data_t *freeze_data);

// Notify the emulated GPU of the vblank interval start or end, discerning
// odd/even fields on entering alternate fields in interlaced modes.
extern void GPUvBlank(int is_vblank_start, int is_odd_field);

// Notify the emulated GPU of the vblank interval start.
extern void GPUupdateLace(void);
