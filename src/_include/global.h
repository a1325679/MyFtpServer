#ifndef _GLOBAL_H
#define _GLOBAL_H

typedef struct{
	char ItemName[50];
	char ItemContent[500];
}ConfItem,*LPConfItem;

extern int thread_count_;
extern unsigned short port_;
extern const char *root_path_;

#endif