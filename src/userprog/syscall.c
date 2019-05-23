// approx whole filew is our code
#include "userprog/syscall.h"
#include <stdio.h>
#include <stdarg.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h" 
#include "threads/vaddr.h" //ch1
#include "userprog/pagedir.h" //ch1 
#include <stdint.h> //ch1
#include <stdbool.h> //ch1
#include "userprog/process.h" // ch1

static void syscall_handler (struct intr_frame *);
void halt_handler (void);
void exit_handler (int status);
tid_t exec_handler (const char *file);
int wait_handler (tid_t pid);
int create_handler (const char *file, unsigned initial_size);
int remove_handler (const char *file);
int open_handler (const char *file);
int filesize_handler (int fd);
int read_handler (int fd, void *buffer, unsigned size);
int write_handler (int fd, const void *buffer, unsigned size);
void seek_handler (int fd, unsigned position) ;
int tell_handler (int fd) ;
void close_handler (int fd);
void validate_ptr(void * ptr);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void validate_ptr(void * ptr) // think
{
	if (ptr >= PHYS_BASE)
	{
		exit_handler(-1);
	}
	if (ptr < 0x0804800)
	{
		exit_handler(-1);
	}

	if (pagedir_get_page(thread_current()->pagedir,ptr)==NULL)
	{
		exit_handler(-1);
	}	
}


static void
syscall_handler (struct intr_frame *f) 
{
	
	int * esp = (int *)f->esp;
	validate_ptr((void *)esp);

	int syscall_number  = *esp;
	
	if(syscall_number == SYS_HALT)
	{
		halt_handler();
	}
	else if(syscall_number == SYS_EXIT)
	{
		int * arg1 = (esp+1);
		validate_ptr((void *)arg1);
	
		int status = *(arg1);

		exit_handler(status);
	}
	else if(syscall_number == SYS_EXEC)
	{
		int * arg1 = (esp+1);
		validate_ptr((void *)arg1);	

		const char * file  = (*((char **)arg1));

		f->eax =  (uint32_t) exec_handler(file);
	}
	else if(syscall_number == SYS_WAIT)
	{
		int * arg1 = (esp+1);
		validate_ptr((void *)arg1);	

		tid_t pid = *((tid_t *)arg1);

		f->eax =  (uint32_t) wait_handler(pid);
	}
	else if(syscall_number == SYS_CREATE)
	{
		int * arg1 = (esp+1);
		int * arg2 = (esp+2);
		validate_ptr((void *)arg1);
		validate_ptr((void *)arg2);

		const char * file = *((char **)arg1);
		unsigned initial_size = (unsigned) *((unsigned *)arg2);

		f->eax =  (uint32_t) create_handler(file, initial_size);
	}
	else if(syscall_number == SYS_REMOVE)
	{
		int * arg1 = (esp+1);
		validate_ptr((void *)arg1);

		const char * file = *((char **)arg1);

		f->eax =  (uint32_t) remove_handler(file);
	}
	else if(syscall_number == SYS_OPEN)
	{
		int * arg1 = (esp+1);
		validate_ptr((void *)arg1);

		const char * file = *((char **)arg1);

		f->eax =  (uint32_t) open_handler(file);
	}
	else if(syscall_number == SYS_FILESIZE)
	{
		int * arg1 = (esp+1);
		validate_ptr((void *)arg1);

		int fd = *(arg1);

		f->eax =  (uint32_t) filesize_handler(fd);
	}
	else if(syscall_number == SYS_READ)
	{
		int * arg1 = (esp+1); 
		int * arg3 = (esp+3);
		int * arg2 = esp + 2 ;
		validate_ptr((void *)arg1);
		validate_ptr((void *)arg2);
		validate_ptr((void *)arg3);	
		
		int fd = *(arg1);
		void * buffer = *((void **)*arg2);
		unsigned size = *((unsigned *)arg3);;
		
		f->eax =  (uint32_t) read_handler(fd,buffer,size);
	}
	else if(syscall_number == SYS_WRITE)
	{
		int * arg1 = (esp+1); 
		int * arg3 = (esp+3);
		int * arg2 = esp + 2;

		validate_ptr((void *)arg1);
		validate_ptr((void *)arg2);
		validate_ptr((void *)arg3);

		int fd =  *(arg1);
		void * buffer = *((void **)arg2);
		unsigned size = *((unsigned *)arg3);;

		f->eax =  (uint32_t) write_handler(fd,buffer,size);
	}
	else if(syscall_number == SYS_SEEK)
	{
		int * arg1 = (esp+1);
		int * arg2 = (esp+2);
		validate_ptr((void *)arg1);
		validate_ptr((void *)arg2);
	

		int fd = *(arg1);
		unsigned position  = *((unsigned *)arg2);;

		seek_handler(fd,position);
	}
	else if(syscall_number == SYS_TELL)
	{
		int * arg1 = (esp+1);
		validate_ptr((void *)arg1);
		int fd = *(arg1);

		f->eax  = (uint32_t) tell_handler(fd);
	}
	else if(syscall_number == SYS_CLOSE)
	{
		int * arg1 = (esp+1);
		validate_ptr((void *)arg1);
		int fd = *(arg1);

		close_handler(fd);
	}
	else{
		// think!
		printf ("system call!\n");
		thread_exit ();
	}
}




void halt_handler (void) 
{

}

void exit_handler (int status)
{
	struct thread * cur_thread = thread_current();
	cur_thread->exit_status = status;
	printf("%s: exit(%d)\n", cur_thread->name,status);
	thread_exit_via_syscall();
}

tid_t exec_handler (const char *file)
{
 return -1;
}

int wait_handler (tid_t pid)
{
 return -1;
}

int create_handler (const char *file, unsigned initial_size)
{
 return 0; 
}

int remove_handler (const char *file)
{
 return 0;
}

int open_handler (const char *file)
{
return -1;
}

int filesize_handler (int fd) 
{
  return -1;
}

int read_handler (int fd, void *buffer, unsigned size)
{
  return -1;
}

int write_handler (int fd, const void *buffer, unsigned size)
{
 	if (fd ==1)
 	{
 		putbuf (buffer, size);
 	}	
 return -1;
}

void seek_handler (int fd, unsigned position) 
{
 
}

int tell_handler (int fd) 
{
 	return 0;
}

void close_handler (int fd)
{
  
}


