; ============================================================
;  MOV 完整测试用例
;  覆盖 8086 MOV 的全部 14 种编码形式 + 内存寻址 + 段前缀
; ============================================================

; ---------- 1. MOV r/m8, r8   88 /r ----------
mov al, bl              ; 88 D8     r8, r8
mov [bx], al            ; 88 07     m8, r8

; ---------- 2. MOV r/m16, r16  89 /r ----------
mov ax, bx              ; 89 D8     r16, r16
mov [bx], ax            ; 89 07     m16, r16

; ---------- 3. MOV r8, r/m8    8A /r ----------
mov al, bl              ; 8A C3     r8, r8
mov al, [bx]            ; 8A 07     r8, m8

; ---------- 4. MOV r16, r/m16  8B /r ----------
mov ax, bx              ; 8B C3     r16, r16
mov ax, [bx]            ; 8B 07     r16, m16

; ---------- 5. MOV r/m16, Sreg  8C /r ----------
mov ax, es              ; 8C C0     r16, Sreg
mov [bx], es            ; 8C 07     m16, Sreg

; ---------- 6. MOV Sreg, r/m16  8E /r ----------
mov es, ax              ; 8E C0     Sreg, r16
mov es, [bx]            ; 8E 07     Sreg, m16

; ---------- 7. MOV AL, moffs8   A0 ----------
mov al, [0x1234]        ; A0 34 12

; ---------- 8. MOV AX, moffs16  A1 ----------
mov ax, [0x1234]        ; A1 34 12

; ---------- 9. MOV moffs8, AL   A2 ----------
mov [0x1234], al        ; A2 34 12

; ---------- 10. MOV moffs16, AX  A3 ----------
mov [0x1234], ax        ; A3 34 12

; ---------- 11. MOV r8, imm8    B0+r ----------
mov al, 5               ; B0 05
mov bl, 5               ; B3 05
mov cl, 0xFF            ; B1 FF

; ---------- 12. MOV r16, imm16  B8+r ----------
mov ax, 0x1234          ; B8 34 12
mov bx, 0x5678          ; BB 78 56

; ---------- 13. MOV r/m8, imm8   C6 /0 ----------
;;mov byte [bx], 5        ; C6 07 05
;;mov byte [bp], 0x80     ; C6 46 00 80

; ---------- 14. MOV r/m16, imm16 C7 /0 ----------
;;mov word [bx], 0x1234   ; C7 07 34 12
;;mov word [bp], 0x5678   ; C7 46 00 78 56

; ============================================================
;  内存寻址模式（用 MOV r8, m8 和 MOV r8, m8 走 8A /r）
; ============================================================
mov al, [bx]            ; 8A 07          mod=00, rm=111
mov al, [bp]            ; 8A 46 00       mod=01, rm=110, disp8=0
mov al, [si]            ; 8A 04          mod=00, rm=100
mov al, [di]            ; 8A 05          mod=00, rm=101
mov al, [bx+si]         ; 8A 00          mod=00, rm=000
mov al, [bx+di]         ; 8A 01          mod=00, rm=001
mov al, [bp+si]         ; 8A 02          mod=01, rm=010, disp8=0
mov al, [bp+di]         ; 8A 03          mod=01, rm=011, disp8=0
mov al, [bx+5]          ; 8A 47 05       mod=01, rm=111, disp8=5
mov al, [bx+0x1234]     ; 8A 87 34 12    mod=10, rm=111, disp16
mov al, [bp+5]          ; 8A 46 05       mod=01, rm=110, disp8=5
mov al, [bp+0x1234]     ; 8A 86 34 12    mod=10, rm=110, disp16
mov al, [bx+si+5]       ; 8A 40 05       mod=01, rm=000, disp8=5
mov al, [bx+di+0x1234]  ; 8A 81 34 12    mod=10, rm=001, disp16
mov al, [0x1234]        ; A0 34 12       A0 形式

; ============================================================
;  段前缀
; ============================================================
mov al, [es:bx]         ; 26 8A 07
mov al, [cs:bx]         ; 2E 8A 07
mov al, [ss:bx]         ; 36 8A 07
mov al, [ds:bx]         ; 3E 8A 07
mov al, [es:bx+si+5]    ; 26 8A 40 05