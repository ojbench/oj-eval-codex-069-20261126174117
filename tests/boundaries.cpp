#include "template.h"
#include <cassert>

int main() {
    using namespace sjtu;
    const int header = sizeof(MemoryBlock);
    const int capacity = MEMORY_SIZE - header;

    resetMemory();
    assert(allocate(capacity + 1) == nullptr);
    MemoryBlock* almost_full = allocate(capacity - header + 1);
    assert(almost_full == head && head->size == capacity);
    assert(head->next == nullptr);

    resetMemory();
    allocate(capacity - header);
    assert(head->next && head->next->size == 0);
    assert(head->next->address == capacity);
    deallocate(head);
    assert(head->size == capacity && head->next == nullptr);

    resetMemory();
    MemoryBlock* first = allocate(100);
    MemoryBlock* second = allocate(100);
    MemoryBlock* third = allocate(100);
    MemoryBlock* fourth = allocate(100);
    deallocate(second);
    deallocate(third);
    assert(second->next == third); // Freeing never merges with a predecessor.
    assert(allocate(101) != second); // Skip the first hole when it is too small.
    deallocate(first);
    assert(first->size == 300 + 2 * header);
    assert(first->next == fourth && !fourth->is_free);
    assert(allocate(200) == first);
    clearMemory();
}
