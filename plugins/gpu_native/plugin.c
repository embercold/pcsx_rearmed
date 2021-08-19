// PSX GPU plugin interface.

#include "plugin.h"

static const gpu_callbacks_t *gpu_cbs = 0;

long GPUinit(void) {
    return GPU_RESULT_SUCCESS;
}

long GPUshutdown(void) {
    return GPU_RESULT_SUCCESS;
}

long GPUopen(unsigned long *_unk1, char *_unk2, char *_unk3) {
    // TODO
    return GPU_RESULT_SUCCESS;
}

long GPUclose(void) {
    // TODO
    return GPU_RESULT_SUCCESS;
}

void GPUrearmedCallbacks(const gpu_callbacks_t *cbs) {
    gpu_cbs = cbs;
}

uint32_t GPUreadData(void) {
    // TODO
    return 0x0BADC0DE;
}

uint32_t GPUreadStatus(void) {
    // TODO
    return 0x0BADC0DE;
}

void GPUwriteDataMem(uint32_t *base, int words) {
    // TODO
}

long GPUdmaChain(uint32_t *base, uint32_t offset) {
    return 0;
}

void GPUreadDataMem(uint32_t *ptr, int words) {
    // TODO
}

void GPUwriteData(uint32_t word) {
    // TODO
}

void GPUwriteStatus(uint32_t word) {
    // TODO
}

long GPUfreeze(uint32_t is_save, gpu_freeze_data_t *freeze_data) {
    // TODO
    return 1;
}

void GPUvBlank(int is_vblank_start, int is_odd_field) {
    // TODO
}

void GPUupdateLace(void) {
    // TODO
}
