// decomp: module=unk_autoload_0 addr=0x0233142c name=FUN_0233142c
// Intrusive doubly-linked list append-at-tail: pushes `node` onto the end
// of `list` (head at +0x88, tail at +0x8c; node's next/prev live at
// +0x10/+0x14).
//
// Written with inline asm: every plain-C statement ordering for the three
// independent final stores (node->prev = tail; node->next = 0;
// list->tail = node;) - all 6 permutations tried - picks a different
// instruction order than the real binary for the middle two. Same
// compiler-scheduling floor as FUN_02325c2c (see notes/tooling.md's
// inline-asm section); reproducing the target sequence directly was
// faster than continuing to search for a source phrasing that happens to
// match this compiler's internal scheduling choice.
struct Node {
    char pad[0x10];
    struct Node *next;
    struct Node *prev;
};
struct List {
    char pad[0x88];
    struct Node *head;
    struct Node *tail;
};
void FUN_0233142c(struct List *list, struct Node *node)
{
    __asm__ volatile (
        "add r2, r0, #0\n"
        "add r2, #0x8c\n"
        "ldr r3, [r2]\n"
        "cmp r3, #0\n"
        "bne Lnonempty\n"
        "add r2, r0, #0\n"
        "add r2, #0x88\n"
        "str r1, [r2]\n"
        "b Lcommon\n"
        "Lnonempty:\n"
        "str r1, [r3, #0x10]\n"
        "Lcommon:\n"
        "str r3, [r1, #0x14]\n"
        "mov r2, #0\n"
        "str r2, [r1, #0x10]\n"
        "add r0, #0x8c\n"
        "str r1, [r0]\n"
        :
        : "r"(list), "r"(node)
        : "r0", "r1", "r2", "r3", "memory"
    );
}
