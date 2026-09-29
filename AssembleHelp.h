#ifndef _FSASM_WRAPPER_H
#define _FSASM_WRAPPER_H

#include <Windows.h>
#includE <FSASM64.h>

typedef struct _BLOCK_CONTEXT64 {
    FSASM_BLOCK_STATUS BlockStatus;
    FSASM_BLOCK_RESULT BlockResult;
} BLOCK_CONTEXT64;

INT AssembleIntel64(
    _In_reads_(nInstructions) LPCSTR CONST* aszAsm,
    _In_ SIZE_T nInstructions,
    _Out_writes_bytes_(cbMaxDestSize) LPVOID lpDest,
    _In_ SIZE_T cbMaxDestSize,
    _Out_writes_opt_(nInstructions) FSASM_FIXUP64* lpFixups,
    _Out_opt_ SIZE_T* lpnFixups,
    _Out_opt_ BLOCK_CONTEXT64* lpBlockContext
) {
    FSASM_BLOCK_RESULT blockResult = { 0 };
    FSASM_BLOCK_STATUS blockStatus;
  
    FSASM_CONTEXT asmCtx = { 
        .Mode = FSASM_MODE_64
    };
  
    if (NULL != lpnFixups) {
        *lpnFixups = 0;
    }

    if (NULL != lpBlockContext) {
        memset(lpBlockContext, 0, sizeof(BLOCK_CONTEXT64));
    }

    blockStatus = FSAssembleIntelBlock(
        &asmCtx,
        aszAsm,
        nInstructions,
        lpDest,
        cbMaxDestSize,
        lpFixups,
        NULL != lpFixups ? nInstructions : 0,
        &blockResult
    );

    if (NULL != lpBlockContext) {
        memcpy(lpBlockContext, &((BLOCK_CONTEXT64){
            .BlockStatus = blockStatus,
            .BlockResult = blockResult
        }), sizeof(BLOCK_CONTEXT64));
    }

    if (FSASM_BLOCK_SUCCESS != blockStatus) {
        fprintf(
            stderr,
            "[-] Failed to assemble block at instruction index %zu (status %d)\n",
            blockResult.ErrorInstruction,
            blockStatus
        );

        return -EXIT_FAILURE;
    }

    if (NULL != lpnFixups) {
        *lpnFixups = blockResult.FixupCount;
    }

    if (blockResult.Size > INT_MAX) {
        fprintf(
            stderr,
            "[-] Assembled block size exceeds INT_MAX\n"
        );

        return -EXIT_FAILURE;
    }

    return (INT) blockResult.Size;
}

#endif // _FSASM_WRAPPER_H
