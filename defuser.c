#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}


static uint32_t ror32(uint32_t x, unsigned n) {
    return (x >> n) | (x << (32U - n));
}

static uint32_t p4_answer(uint32_t target, uint32_t low, uint32_t high,
                          uint32_t key) {
    uint32_t mid = low + ((high - low) >> 1);
    if (target == mid) return (mid + UINT32_C(0xf6)) ^ key;
    if (target < mid)
        return (p4_answer(target, low, mid - 1U, key) * 3U)
               ^ (mid + key) ^ UINT32_C(0xc);
    return (p4_answer(target, mid + 1U, high, key) * 3U)
           ^ (mid + key) ^ UINT32_C(0x8);
}

static uint32_t p6_apply(unsigned slot, uint32_t x) {
    static const unsigned operations[8] = {4, 0, 1, 0, 2, 3, 4, 5};
    switch (operations[slot]) {
        case 0: return (x ^ UINT32_C(0x2446)) + (x << 7);
        case 1: return (x + UINT32_C(0xc9b9)) ^ (x >> 5);
        case 2: return ror32(x, 2);
        case 3: return ((x << 3) + UINT32_C(0x31c7)) ^ (x * 7U);
        case 4: return ((x >> 1) ^ x) + UINT32_C(0x909c);
        default: return ror32(x, 27) ^ UINT32_C(0x49b);
    }
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t low = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);
    uint32_t a = (high - low) * 3U + UINT32_C(0xfb80);
    uint32_t b = ((uint32_t)(challenge >> 16) + UINT32_C(0x629)) ^ low;
    values[0] = (b + a * 8U) ^ UINT32_C(0x3fce);
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    values[0] = ((uint32_t)(challenge >> 15) * 2U & UINT32_C(0x1e)) + 12U;
    for (unsigned i = 1; i < 6; ++i)
        values[i] = (values[i - 1] + i) * 5U
                    - (uint32_t)((challenge >> (9U + 3U * i)) & 63U);
    count = 6;
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t selector = (uint32_t)challenge & 7U;
    uint32_t x = (uint32_t)(challenge >> 3) ^ (uint32_t)(challenge >> 32);
    values[0] = selector;
    switch (selector) {
        case 0: values[1] = (x * 4U) ^ UINT32_C(0x15cf); break;
        case 1: values[1] = (x ^ UINT32_C(0x4729)) + (x >> 2); break;
        case 2: values[1] = x * 5U + UINT32_C(0x19258); break;
        case 3: values[1] = (x ^ UINT32_C(0xe3c2)) + (x >> 1); break;
        case 4: values[1] = x * 3U + UINT32_C(0xdeb5); break;
        case 5: values[1] = x * 5U + UINT32_C(0x6dd); break;
        case 6: values[1] = x * 5U + UINT32_C(0xbe5a); break;
        default: values[1] = (x * 4U) ^ UINT32_C(0x5db); break;
    }
    count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t key = (uint32_t)(challenge >> 7) ^ (uint32_t)(challenge >> 32);
    values[0] = p4_answer((uint32_t)challenge & 127U, 0, 127U, key);
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    static const struct {
        uint32_t value;
        uint32_t tag;
        unsigned next;
    } nodes[12] = {
        {0x3690U, 0x735U, 3}, {0x1848U, 0x264U, 7},
        {0x912dU, 0x93fU, 11}, {0x9418U, 0x353U, 2},
        {0xdcb3U, 0x507U, 1}, {0x6161U, 0x621U, 0},
        {0xa85dU, 0xa53U, 4}, {0x13bbU, 0x80fU, 9},
        {0x4208U, 0x52U, 6}, {0xb2eU, 0x442U, 10},
        {0xe6d7U, 0xb66U, 5}, {0x9085U, 0x14aU, 12}
    };
    static const unsigned heads[8] = {8, 6, 4, 1, 7, 9, 10, 5};
    unsigned node = heads[(uint32_t)challenge & 7U];
    unsigned steps = (unsigned)((challenge >> 3) & 3U);
    uint32_t acc = (uint32_t)(challenge >> 6)
                   ^ (uint32_t)(challenge >> 32) ^ UINT32_C(0xbab0);
    for (unsigned i = 0; i <= steps; ++i) {
        acc = (acc ^ nodes[node].value) + nodes[node].tag;
        values[1] = nodes[node].tag;
        if (i < steps) node = nodes[node].next;
    }
    values[0] = acc;
    count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t x = (uint32_t)(challenge >> 6) ^ (uint32_t)(challenge >> 32);
    unsigned first = (unsigned)(challenge & 7U);
    unsigned second = (unsigned)((challenge >> 3) & 7U);
    x = p6_apply(first, x) ^ UINT32_C(0x26f5);
    values[0] = p6_apply(second, x) + UINT32_C(0x9ca2);
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    uint32_t a = (uint32_t)challenge ^ (uint32_t)(challenge >> 32);
    uint32_t b = (uint32_t)context ^ (uint32_t)(context >> 32);
    uint32_t selector = (uint32_t)context & 3U;
    uint32_t second = ror32(a ^ b, 1) ^ UINT32_C(0x6b34)
                      ^ (selector * UINT32_C(0x7720)) ^ (uint32_t)(context >> 16);
    uint32_t third = second ^ (b + selector + UINT32_C(0x1c14) + (a >> 1));
    int size = snprintf(out, cap,
                        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAOPEN%" PRIu32 " %" PRIu32 " %" PRIu32,
                        selector, second, third);
    return size < 0 || (size_t)size >= cap;

}
