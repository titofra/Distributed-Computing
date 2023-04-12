#include "mailbox.h"

void InitMailBox (mailbox_t* mb, uint16_t N_max, mbkind novalues, bool overwrite) {
    mb->N = 0;
    mb->N_max = N_max;
    mb->novalues = novalues;
    mb->overwrite = overwrite;
    mb->box = (mbkind*) malloc (0);
}

bool Send (mailbox_t* mb, mbkind mail) {
    if (mb->N < mb->N_max) {
        // The box is not full, let's add the mail
        mb->box = (mbkind*) realloc (mb->box, (mb->N + 1) * sizeof (mbkind));
        if (mb->box == NULL) {
            printf ("Cannot extent the mailbox\n");
            exit (1);
        }
        mb->box [mb->N] = mail;
        mb->N += 1;
    } else {
        if (mb->overwrite) {
            // The box is full, let's remove the first mail and add the new one
            for (uint16_t i = 0; i < mb->N - 1; i++) {
                mb->box [i] = mb->box [i + 1];
            }
            mb->box [mb->N] = mail;
        } else {
            // The mail cannot be stored
            return false;
        }
    }
    return true;
}

mbkind Receive (mailbox_t* mb) {
    if (mb->N > 0) {
        // There is at least one mail, let's remove it
        mbkind ret = mb->box [0];
        for (uint16_t i = 0; i < mb->N - 1; i++) {
            mb->box [i] = mb->box [i + 1];
        }
        mb->N -= 1;
        return ret; // The mail is out of the box
    } else {
        // There is no mail, we return novalues
        return mb->novalues;
    }
}

uint16_t GetNbMails (mailbox_t* mb) {
    return mb->N;
}