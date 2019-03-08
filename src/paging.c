// Jacob Laney
// jpl0015@auburn.edu

#include "config.h"
#include "paging.h"

int
search_tlb(struct System *sys, pagenum_t page, framenum_t *frame) {
    int i;
    int foundIndex;

    // loop through the tlb and try to find the page/frame reference
    foundIndex = -1;
    for (i = 0; i < TLB_SIZE; i++ ) {
        sys->tlb[i].age++; // increment the age each tlb access
        if (sys->tlb[i].page == page) {
            foundIndex = i;
            sys->tlb[i].age = MINIMUM_TLB_AGE;
        }
    }

    // tlb miss
    if (foundIndex < 0) {
        return 1; // tlb miss
    }

    // tlb hit
    sys->tlb[foundIndex].age = MINIMUM_TLB_AGE;
    *frame = sys->tlb[foundIndex].frame;

    return 0;
}

int
search_page_table(struct System *sys, pagenum_t page, framenum_t *frame) {
    int err;

    // page fault
    if (page < 0 || page >= PAGE_TABLE_SIZE) {
        return 1; // this page does not exist
    }

    // page fault
    if (sys->ptable[page] == NULL_FRAME) {
        return 1; // no frame associated with this page
    }

    // now the frame number is found and can be returned
    *frame = sys->ptable[page];

    // now invoke tlb replacement
    err = 0;
    switch (sys->algorithmChoice) {
        case FIFO:
            err = tlb_fifo_replace(sys, page, *frame);
            break;
        case LRU:
            err = tlb_lru_replace(sys, page, *frame);
            break;
        default:
            err = 1;
            break;
    }

    return err;
}

int
page_fault_handle(struct System *sys, pagenum_t page, framenum_t *frame) {
    int err;
    static int currentFrame = 0;

    // try to read from virtual mem into physical mem
    // and update page table
    int memOffset = currentFrame * FRAME_SIZE;
    int vmfileOffset = page * PAGE_SIZE;
    fseek(sys->vmfile, vmfileOffset, 0);
    err = fread(sys->mem + memOffset, FRAME_SIZE, 1, sys->vmfile);
    if (err != 1) {
        return 1; // failed to read into physica mem
    }

    // update the page table
    sys->ptable[page] = currentFrame;
    *frame = currentFrame;

    // run the tlb replacement algorithm
    err = 0;
    switch (sys->algorithmChoice) {
        case FIFO:
            err = tlb_fifo_replace(sys, page, *frame);
            break;
        case LRU:
            err = tlb_lru_replace(sys, page, *frame);
            break;
        default:
            err = 1;
            break;
    }

    currentFrame = (currentFrame + 1) % FRAME_COUNT;

    return err;
}

// for FIFO, I implemented a circular queue
int
tlb_fifo_replace(struct System *sys, pagenum_t page, framenum_t frame) {
    int i;

    static int head = 0; // head of the queue

    if (page == NULL_PAGE || frame == NULL_FRAME) {
        return 1; // failed to replace
    }

    // replace
    sys->tlb[head].age = MINIMUM_TLB_AGE;
    sys->tlb[head].page = page;
    sys->tlb[head].frame = frame;

    // move to next spot in circular queue
    head = (head + 1) % TLB_SIZE;

    return 0;
}

// for LRU, each tlb entry has an age and the least recently used
// is replaced
int
tlb_lru_replace(struct System *sys, pagenum_t page, framenum_t frame) {
    int i;
    int maxAge;
    int maxIndex;

    if (page == NULL_PAGE || frame == NULL_FRAME) {
        return 1; // invalid input
    }

    // get the max age entry
    maxAge = sys->tlb[0].age;
    maxIndex = 0;
    for (i = 0; i < TLB_SIZE; i++) {
        if (sys->tlb[i].age >= maxAge) {
            maxAge = sys->tlb[i].age;
            maxIndex = i;
        }
    }

    // repalce maximum age entry with new entry
    sys->tlb[maxIndex].age = MINIMUM_TLB_AGE;
    sys->tlb[maxIndex].page = page;
    sys->tlb[maxIndex].frame = frame;

    return 0;
}

pagenum_t
get_page_number(addr_t logicalAddress) {
    return (logicalAddress >> 8) & 0xFF;
}

offset_t
get_page_offset(addr_t logicalAddress) {
    return logicalAddress & 0xFF;
}

addr_t
get_virtual_address(pagenum_t page, offset_t offset) {
    return page * PAGE_SIZE + offset;
}

addr_t
get_physical_address(framenum_t frame, offset_t offset) {
    return frame * FRAME_SIZE + offset;
}

// convert logical address to page and offset
int
logical_to_page_and_offset(addr_t addr, pagenum_t *page, offset_t *offset) {
    *page = get_page_number(addr);
    *offset = get_page_offset(addr);
    if (*page >= PAGE_TABLE_SIZE) {
        return 1;
    }
    if (*offset >= PAGE_SIZE) {
        return 1;
    }
    return 0;
}
