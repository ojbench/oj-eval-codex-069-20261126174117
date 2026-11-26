#pragma once

#include <cstddef>

namespace sjtu {

constexpr std::size_t MEMORY_SIZE = 1024 * 1024;

struct MemoryBlock {
    int address;
    int size;
    bool is_free;
    MemoryBlock* next = nullptr;
};

inline MemoryBlock* head = nullptr;

inline MemoryBlock* splitBlock(MemoryBlock* block, std::size_t size) {
    if (size > static_cast<std::size_t>(block->size) ||
        static_cast<std::size_t>(block->size) - size < sizeof(MemoryBlock)) {
        return nullptr;
    }
    const int offset = static_cast<int>(size + sizeof(MemoryBlock));
    MemoryBlock* remainder = new MemoryBlock{
        block->address + offset, block->size - offset, true, block->next};
    block->size = static_cast<int>(size);
    block->next = remainder;
    return remainder;
}

inline void mergeBlock(MemoryBlock* block) {
    MemoryBlock* next = block->next;
    if (next == nullptr) return;
    block->size += static_cast<int>(sizeof(MemoryBlock)) + next->size;
    block->next = next->next;
    delete next;
}

inline MemoryBlock* allocate(int size) {
    if (size <= 0) return nullptr;
    for (MemoryBlock* block = head; block != nullptr; block = block->next) {
        if (block->is_free && block->size >= size) {
            // A remainder too small for its header stays in this allocation.
            splitBlock(block, static_cast<std::size_t>(size));
            block->is_free = false;
            return block;
        }
    }
    return nullptr;
}

inline void deallocate(MemoryBlock* ptr) {
    if (ptr == nullptr) return;
    ptr->is_free = true;
    while (ptr->next != nullptr && ptr->next->is_free) {
        mergeBlock(ptr);
    }
}

inline void clearMemory() {
    while (head != nullptr) {
        MemoryBlock* next = head->next;
        delete head;
        head = next;
    }
}

inline void resetMemory() {
    clearMemory();
    head = new MemoryBlock{
        0, static_cast<int>(MEMORY_SIZE - sizeof(MemoryBlock)), true, nullptr};
}

} // namespace sjtu
