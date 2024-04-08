section .text

global HtListFindElemByKey

HtListFindElemByKey:    ; (HtListType* list, const char* key, size_t* elemPos)
        mov     r9, QWORD  [rdi]        ; r9 = list->data
        xor     ecx, ecx                ; ecx = 0 (стартовая позиция)
        xor     r10d, r10d              ; r10d = 0

        vmovdqu ymm0, YWORD  [rsi]    ; loading key to ymm0
DO_WHILE_BODY:
        mov     rax, rcx                ; rax = pos
        sal     rax, 5                  ; rax * sizeof(HtListElemType)
        add     rax, r9                 ; rax = list->data + pos
        mov     r8, QWORD  [rax]        ; r8  = list->data[pos].value.key
        test    r8, r8                  ;
        je      DO_WHILE_CONDITION      ; if r8 == null jmp to condition
        
        vptest ymm0, YWORD  [r8]      ;
        jc HT_LIST_RET                  ; compare r8 and key. If equal -> ret

DO_WHILE_CONDITION:
        mov     rcx, QWORD  [rax+24]    ; rcx = list->data[pos].nextPos
        test    rcx, rcx                ;
        jne     DO_WHILE_BODY           ; if rcx != 0 -> continue
        mov     eax, 8                  ; return NO_ELEMENT_FOUND

        vzeroupper
        ret

HT_LIST_RET:
        mov     QWORD  [rdx], rcx       ; *elemPos = pos
        xor     eax, eax                ; return NO_ERR

        vzeroupper
        ret