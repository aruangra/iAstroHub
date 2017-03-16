/*
 * server.h
 *
 *  Created on: 04.12.2010
 *      Author: gm
 *
 *
 * This file is part of Lin_guider.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>

#include <set>
#include <vector>
#include <map>
#include <deque>

#include <QObject>


using namespace std;

typedef struct net_params_s
{
	char  bcast_ip[16];
	int   bcast_port;
	char  listen_socket[255];
	int   listen_port;
	bool  use_tcp;

	bool operator == ( const net_params_s &v ) const
	{
		if( &v == this ) return true;
		return !strncmp( bcast_ip, v.bcast_ip, sizeof(bcast_ip) ) &&
				bcast_port == v.bcast_port &&
				!strncmp( listen_socket, v.listen_socket, sizeof(listen_socket) ) &&
				listen_port == v.listen_port &&
				use_tcp == v.use_tcp;
	}
	bool operator != ( const net_params_s &v ) const
	{
		if( &v == this ) return false;
		return strncmp( bcast_ip, v.bcast_ip, sizeof(bcast_ip) ) ||
				bcast_port != v.bcast_port ||
				strncmp( listen_socket, v.listen_socket, sizeof(listen_socket) ) ||
				listen_port != v.listen_port ||
				use_tcp != v.use_tcp;
	}
}net_params_t;

enum bcast_messages
{
	BCM_DEBUG = 0,

	BCM_SRV_STARTED = 100,
	BCM_SRV_STOPPED,
	BCM_NORMAL_IMAGE_QUALITY,
	BCM_LOW_IMAGE_QUALITY,
	BCM_CRITICAL_IMAGE_QUALITY,
	BCM_GUIDING_STABLE,
	BCM_GUIDING_UNSTABLE,
	BCM_DRIFT_DATA,

	BCM_MAX
};

#pragma pack(1)
struct proto_hdr
{
	enum consts
	{
		SIGNATURE = 2,
		HDR_SIZE = 8,
	};
	static void set( uint8_t *mem, uint16_t cmd, uint32_t sz )
	{
		struct proto_hdr *hdr = reinterpret_cast<proto_hdr*>(mem);
		hdr->sign = SIGNATURE;
		hdr->cmd = cmd;
		hdr->data_sz = sz;
	}
	static const proto_hdr* get( const uint8_t *mem )
	{
		return reinterpret_cast<const proto_hdr*>(mem);
	}
	static const uint8_t *data( const uint8_t *mem )
	{
		return mem + HDR_SIZE;
	}
	static uint8_t *data( uint8_t *mem )
	{
		return mem + HDR_SIZE;
	}
	static bool check( const uint8_t *mem )
	{
		const proto_hdr *hdr = reinterpret_cast<const proto_hdr*>(mem);
		return (hdr && hdr->sign == SIGNATURE);
	}
	uint16_t sign;
	uint16_t cmd;
	uint32_t data_sz;
} __attribute__ ((aligned (1)));

#pragma pack()


struct conn_t
{
	friend class server;
	enum sizes
	{
		BUF_SZ = 4096
	};

	conn_t() :
		answer_sz(0),
		fd(-1),
		id( 0 ),
		read_pos(0),
		read_len(0),
		write_pos(0),
		write_len(0),

		m_answer(write_buf+proto_hdr::HDR_SIZE)
	{}
	const uint8_t *get_data( size_t* data_sz ) const
	{
		if( data_sz )
			*data_sz = read_len;
		return read_buf;
	}
	uint8_t *get_answer( size_t* answer_max_sz ) const
	{
		if( answer_max_sz )
			*answer_max_sz = BUF_SZ;
		return m_answer;
	}
	ssize_t  answer_sz;
private:
	int fd;
	uint64_t id;
	uint8_t read_buf[proto_hdr::HDR_SIZE+BUF_SZ];
	ssize_t read_pos, read_len;
	uint8_t write_buf[proto_hdr::HDR_SIZE+BUF_SZ];
	ssize_t write_pos, write_len;
	void init( void )
	{
		fd = -1;
		id = ++autoinc_id;
		read_pos = write_pos = 0;
		read_len = write_len = 0;
		answer_sz = 0;
	}
	void close()
	{
		if( fd >= 0 )
		{
			::close( fd );
			fd = -1;
		}
		id = 0;
		read_pos = write_pos = 0;
		read_len = write_len = 0;
		answer_sz = 0;
		read_buf[0] = 0;
		write_buf[0] = 0;
	}
	void reset( void )
	{
		read_pos = write_pos = 0;
		read_len = write_len = 0;
	}
	uint8_t *m_answer;

	static unsigned int autoinc_id;
};


class server : public QObject
{
	Q_OBJECT

public:
	server( const net_params_t &nparams );
	virtual ~server();

	void start( void );
	void stop( void );
	conn_t* take_conn( void );
	void return_conn( conn_t *cn );

	size_t get_num_active_connections( void ) const;
	static bool send_bcast_msg( int msg, const char *fmt = "", ... );
	static void set_msg_map( const std::map<int, bool>& map );
	static void get_msg_map( std::map<int, bool> *map );

	enum cmd
	{
		GET_VER = 1, //
		SET_GUIDER_SQUARE_POS,
		SAVE_FRAME,
		DITHER,
		/* Add to support iAstroHub, by Anat & Zachary */
		START_AUTOGUIDING, 
		STOP_AUTOGUIDING,
        START_CALIBRATION,
		STOP_CALIBRATION,
        FINDSTAR,
		SET_VIDEO_GAIN,
		SET_FPS,
		SET_GAIN_P,
		GET_GAIN_P,
		MOVE_RETICLE,
		/* iAstroHub patch ends */
		DITHER_NO_WAIT_XY,
		GET_DISTANCE,
		SAVE_FRAME_DECORATED
	};
	enum sizes
	{
		EPOLL_QUEUE_LEN = 8
	};
signals:
	void do_command();

private:
	int m_res_fd;
	int m_epollfd;
	set<conn_t *> m_conns;
	vector<conn_t *> m_free_conns;
	std::deque< conn_t* > m_in_queue;
	std::deque< conn_t* > m_out_queue;

	// thread stuff
	pthread_t m_thread;		// thread desc.
	pthread_mutex_t	m_mtx;	// sync mutex
	pthread_mutex_t	m_in_mtx;	// sync mutex
	pthread_mutex_t	m_out_mtx;	// sync mutex
	int	m_quit_thread_flag;		// quit flag

	// params
	static net_params_t m_net_params;

	bool m_initialized;

	void set_nonblocking( int sock );
	void set_reuseport( int sock );

	int add_epoll_event( int fd, void * data, int eventfl );
	int mod_epoll_event( int fd, void *data, int eventfl );
	int del_epoll_event( int fd );

	conn_t *get_conn( void );
	int  close_conn( conn_t *pconn );

	int  do_read( conn_t *pconn );
	int  do_write( conn_t *pconn );

	static void *server_thread( void *param );

	// misc
	static bool send_broadcast( const char * str, size_t str_len );
	static const char *m_bcm_names[BCM_MAX];
	static bool m_bcm_available_map[BCM_MAX];

	// command names
	std::map< int, const char* > m_cmd_names;
};

#endif /* SERVER_H_ */
