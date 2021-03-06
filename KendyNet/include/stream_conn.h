#ifndef _STREAM_CONN_H
#define _STREAM_CONN_H

//面向数据流的连接
#include "kendynet.h"
#include "packet.h"
#include "rpacket.h"
#include "wpacket.h"
#include "rawpacket.h"
#include "kn_refobj.h"
#include "kn_timer.h"

#define MAX_WBAF 512
#define MAX_SEND_SIZE 65535

enum {
	decode_packet_too_big = -1,
	decode_socket_close = -2,
	decode_unknow_error = -3,
};

struct stream_conn;
//解包器接口
typedef struct decoder{
	int  (*unpack)(struct decoder*,struct stream_conn*);
	void (*destroy)(struct decoder*);
	int  mask;
}decoder;

typedef struct stream_conn
{
	refobj   refobj;	
	handle_t handle;
	struct    iovec wsendbuf[MAX_WBAF];
	struct    iovec wrecvbuf[2];
	st_io      send_overlap;
	st_io      recv_overlap;
	void*    ud;
	void      (*destroy_ud)(void*);
	uint32_t unpack_size; //还未解包的数据大小
	uint32_t unpack_pos;
	uint32_t next_recv_pos;
	buffer_t next_recv_buf;
	buffer_t unpack_buf;
	kn_list  send_list;//待发送的包
	uint32_t recv_bufsize;
	void     (*on_packet)(struct stream_conn*,packet_t);
	void     (*on_disconnected)(struct stream_conn*,int err);
	kn_timer_t sendtimer;
	decoder* _decoder;
	uint8_t  doing_send:1;
	uint8_t  doing_recv:1;
	uint8_t  close_step:2;
	uint8_t  processing:1;		   
}stream_conn,*stream_conn_t;

/*
 *   返回1：process_packet调用后rpacket_t自动销毁
 *   否则,将由使用者自己销毁
 */
typedef void  (*CCB_PROCESS_PKT)(stream_conn_t,packet_t);
typedef void (*CCB_DISCONNECTD)(stream_conn_t,int err);

/*packet_type:RPACKET/RAWPACKET*/
stream_conn_t new_stream_conn(handle_t sock,uint32_t buffersize,decoder *_decoder);
void     stream_conn_close(stream_conn_t c);
int      stream_conn_send(stream_conn_t c,packet_t p);
static inline handle_t stream_conn_gethandle(stream_conn_t c){
	return c->handle;
} 

static inline void stream_conn_setud(stream_conn_t c,void *ud,void (*destroy_ud)(void*)){
	c->ud = ud;
	c->destroy_ud = destroy_ud;
}
static inline void* stream_conn_getud(stream_conn_t c){
	return c->ud;
}

static inline int stream_conn_isclose(stream_conn_t c){
	return c->close_step > 0;
}
/*
 * 与engine关联并启动接收过程.
 * 如果conn已经关联过,则首先与原engine断开关联.
 * 如果engine为NULL，则断开关联
 *  
*/
int     stream_conn_associate(engine_t,stream_conn_t conn,CCB_PROCESS_PKT,CCB_DISCONNECTD);


typedef struct {
	decoder base;
	uint32_t maxpacket_size;
}rpk_decoder;

typedef struct {
	decoder base;
}rawpk_decoder;

decoder* new_rpk_decoder(uint32_t maxpacket_size);
decoder* new_rawpk_decoder();

void destroy_decoder(decoder*);








#endif
