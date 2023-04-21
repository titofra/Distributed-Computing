/*
    @brief Simple int non-blocking mailbox
    @brief Mailbox can stores values, wich can be retrieve at any moment
    @author Titouan ABADIE - http://github.com/titofra - 04/23
*/

#ifndef MAILBOX_H
#define MAILBOX_H

#define mbkind int

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mailbox {
    mbkind* box;
    uint16_t N;
    uint16_t N_max;
    mbkind novalues;
    bool overwrite;
} mailbox_t;

/*
    @brief Initialise a mailbox
    @param mailbox_t* mb, Pointer to the mailbox
    @param uint16_t N_max, Macimum number of elements in the mailbox
    @param mbkind novalues, Value return when there is no element stored in the mailbox
    @param bool overwrite, Should we replace elements when we want to insert a new element in a full mailbox? If true, first element is removed, else the new element is not accepted
*/
void InitMailBox (mailbox_t* mb, uint16_t N_max, mbkind novalues, bool overwrite);

/*
    @brief Add a new element to the mailbox
    @param mailbox_t* mb, Pointer to the mailbox
    @param mbkind mail, The element
    @return true if the element has been added, else false
*/
bool Send (mailbox_t* mb, mbkind mail);

/*
    @brief Retrieve the first element
    @param mailbox_t* mb, Pointer to the mailbox
    @return The first element if the mail box is not empty, else novalues (defined at InitMailBox)
*/
mbkind Receive (mailbox_t* mb);

/*
    @bried Get the number of elements
    @param mailbox_t* mb, Pointer to the mailbox
    @return The number of elements
*/
uint16_t GetNbMails (mailbox_t* mb);

#ifdef __cplusplus
}
#endif

#endif  // MAILBOX_H