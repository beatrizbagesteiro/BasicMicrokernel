.section .text
.global context_switch

# a0 = &old->regs
# a1 = &new->regs

context_switch:

    # ---------- SAVE OLD CONTEXT ----------

    sd ra,   0(a0)
    sd sp,   8(a0)

    sd gp,  16(a0)
    sd tp,  24(a0)

    sd t0,  32(a0)
    sd t1,  40(a0)
    sd t2,  48(a0)

    sd s0,  56(a0)
    sd s1,  64(a0)

    sd a0,  72(a0)
    sd a1,  80(a0)
    sd a2,  88(a0)
    sd a3,  96(a0)
    sd a4, 104(a0)
    sd a5, 112(a0)
    sd a6, 120(a0)
    sd a7, 128(a0)

    sd s2, 136(a0)
    sd s3, 144(a0)
    sd s4, 152(a0)
    sd s5, 160(a0)
    sd s6, 168(a0)
    sd s7, 176(a0)
    sd s8, 184(a0)
    sd s9, 192(a0)
    sd s10,200(a0)
    sd s11,208(a0)

    sd t3, 216(a0)
    sd t4, 224(a0)
    sd t5, 232(a0)
    sd t6, 240(a0)

    # ---------- LOAD NEW CONTEXT ----------

    mv t6, a1

    ld ra,   0(t6)
    ld sp,   8(t6)

    ld gp,  16(t6)
    ld tp,  24(t6)

    ld t0,  32(t6)
    ld t1,  40(t6)
    ld t2,  48(t6)

    ld s0,  56(t6)
    ld s1,  64(t6)

    ld a0,  72(t6)
    ld a1,  80(t6)
    ld a2,  88(t6)
    ld a3,  96(t6)
    ld a4, 104(t6)
    ld a5, 112(t6)
    ld a6, 120(t6)
    ld a7, 128(t6)

    ld s2, 136(t6)
    ld s3, 144(t6)
    ld s4, 152(t6)
    ld s5, 160(t6)
    ld s6, 168(t6)
    ld s7, 176(t6)
    ld s8, 184(t6)
    ld s9, 192(t6)
    ld s10,200(t6)
    ld s11,208(t6)

    ld t3, 216(t6)
    ld t4, 224(t6)
    ld t5, 232(t6)
    ld t6, 240(t6)

    ret