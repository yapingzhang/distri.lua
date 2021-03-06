#ifndef _KENDYNET_H
#define _KENDYNET_H

#include "kn_common_include.h"
#include "kn_common_define.h"
#include "kn_sockaddr.h"
#include "kn_time.h"

typedef struct handle* handle_t;
typedef void* engine_t;

engine_t kn_new_engine();
void     kn_release_engine(engine_t);
int      kn_engine_run(engine_t);
void     kn_engine_runonce(engine_t,uint32_t);
void     kn_stop_engine(engine_t);

handle_t kn_new_sock(int domain,int type,int protocal);
int      kn_close_sock(handle_t);

int      kn_sock_listen(engine_t,
						handle_t,
						kn_sockaddr*,
						void (*cb_accept)(handle_t,void*),
						void*);
						
int      kn_sock_connect(engine_t,
						 handle_t,
						 kn_sockaddr *remote,
						 kn_sockaddr *local,
						 void (*cb_connect)(handle_t,int,void*,kn_sockaddr*),
						 void*);

int      kn_sock_associate(handle_t,
						   engine_t,
						   void (*cb_ontranfnish)(handle_t,st_io*,int,int),
						   void (*destry_stio)(st_io*));


/*
*  如果io操作立即完成返回完成的字节数
*  如果io无法立即完成会调用kn_sock_post_xxx函数.
*  出错返回-1,通过errno查看出错原因  
*/   
int      kn_sock_send(handle_t,st_io*);
int      kn_sock_recv(handle_t,st_io*);

/*
*   投递io请求,将请求插入到请求队列的尾部  
*   实际的io操作在主循环中完成，请求完成后回调cb_ontranfnish
*   成功返回0,否则返回-1
*/ 

int      kn_sock_post_send(handle_t,st_io*);
int      kn_sock_post_recv(handle_t,st_io*);


void     kn_sock_setud(handle_t,void*);
void*    kn_sock_getud(handle_t);
int      kn_sock_fd(handle_t);
engine_t kn_sock_engine(handle_t);
kn_sockaddr* kn_sock_addrlocal(handle_t);
kn_sockaddr* kn_sock_addrpeer(handle_t);

#include <arpa/inet.h>
#include <endian.h>

#ifdef _ENDIAN_CHANGE_

static inline uint16_t kn_swap16(uint16_t num){
	return ((num << 8) & 0xFF00) | ((num & 0xFF00) >> 8); 
}

static inline uint32_t kn_swap32(uint32_t num){
	return  (kn_swap16(((uint16_t*)&num)[0]) << 16) | kn_swap16(((uint16_t*)&num)[1]); 
}

static inline uint64_t kn_swap64(uint64_t num){
	return ((uint64_t)kn_swap32(((uint32_t*)&num)[0]) << 32) | kn_swap32(((uint32_t*)&num)[1]);
}

#define kn_hton16(x) ((uint16_t)htons(x))
#define kn_ntoh16(x) ((uint16_t)ntohs(x))
#define kn_hton32(x) ((uint32_t)htonl(x))
#define kn_ntoh32(x) ((uint32_t)ntohl(x))

static inline uint64_t kn_hton64(uint64_t num){
#if __BYTE_ORDER == __BIG_ENDIAN 
	return num;
#else
	return kn_swap64(num);
#endif
}

static inline uint64_t kn_ntoh64(uint64_t num){
#if __BYTE_ORDER == __BIG_ENDIAN 
	return num;
#else
	return kn_swap64(num);
#endif
}

#else

#define kn_hton16(x) (x)
#define kn_ntoh16(x) (x)
#define kn_hton32(x) (x)
#define kn_ntoh32(x) (x)

static inline uint64_t kn_hton64(uint64_t num){
	return num;
}

static inline uint64_t kn_ntoh64(uint64_t num){
	return num;
}

#endif

#endif

