/*
 * server.cpp
 *
 * Simple TCP server
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <set>
#include <vector>
#include <map>
#include <utility>


#include "server.h"
#include "utils.h"
#include "timer.h"

using namespace std;

unsigned int conn_t::autoinc_id = 0;

net_params_t server::m_net_params = { "127.0.0.1", 5001, "/tmp/lg_ss", 5656, false };

const char *server::m_bcm_names[BCM_MAX] = {
		"DEBUG", 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		// 1 row = 10 messages
		"SRV_STARTED", "SRV_STOPPED", "NORMAL IMAGE QUALITY", "LOW IMAGE QUALITY", "CRITICAL IMAGE QUALITY",
		"GUIDING STABLE", "GUIDING UNSTABLE", "DRIFT_DATA"

};

bool server::m_bcm_available_map[BCM_MAX] = {
		true/*DEBUG*/, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

		// 1 row = 10 messages
		true/*"STARTED"*/, true/*"STOPPED"*/, true/*"NORMAL IMAGE QUALITY"*/, true/*"LOW IMAGE QUALITY"*/, true/*"CRITICAL IMAGE QUALITY"*/,
		true/*"GUIDING STABLE"*/, true/*"GUIDING UNSTABLE"*/, true/*"DRIFT_DATA"*/
};


server::server( const net_params_t &nparams ) :
		m_res_fd( -1 ),
		m_epollfd( -1 ),
		m_quit_thread_flag(0),
		m_initialized(false)
{
	memcpy( &m_net_params, &nparams, sizeof(net_params_t) );
	m_conns.clear();
	m_free_conns.clear();

	m_cmd_names.insert( std::make_pair( server::GET_VER, "GET_VER" ) );
	m_cmd_names.insert( std::make_pair( server::SET_GUIDER_SQUARE_POS, "SET_GUIDER_SQUARE_POS" ) );
	m_cmd_names.insert( std::make_pair( server::SAVE_FRAME, "SAVE_FRAME" ) );
	m_cmd_names.insert( std::make_pair( server::DITHER, "DITHER" ) );
	m_cmd_names.insert( std::make_pair( server::DITHER_NO_WAIT_XY, "DITHER_NO_WAIT_XY" ) );
	m_cmd_names.insert( std::make_pair( server::GET_DISTANCE, "GET_DISTANCE" ) );
	/* Added to support iAstroHub, by Anat & Zachary */
	m_cmd_names.insert( std::make_pair( server::START_AUTOGUIDING, "START_AUTOGUIDING" ) );
	m_cmd_names.insert( std::make_pair( server::STOP_AUTOGUIDING, "STOP_AUTOGUIDING" ) );
	m_cmd_names.insert( std::make_pair( server::START_CALIBRATION, "START_CALIBRATION" ) );
	m_cmd_names.insert( std::make_pair( server::STOP_CALIBRATION, "STOP_CALIBRATION" ) );
	m_cmd_names.insert( std::make_pair( server::FINDSTAR, "FINDSTAR" ) );
	m_cmd_names.insert( std::make_pair( server::SET_VIDEO_GAIN, "SET_VIDEO_GAIN" ) );
	m_cmd_names.insert( std::make_pair( server::SET_FPS, "SET_FPS" ) );
	m_cmd_names.insert( std::make_pair( server::SET_GAIN_P, "SET_GAIN_P" ) );
	m_cmd_names.insert( std::make_pair( server::GET_GAIN_P, "GET_GAIN_P" ) );
	m_cmd_names.insert( std::make_pair( server::MOVE_RETICLE, "MOVE_RETICLE" ) );
	/* iAstroHub patch ends */
        m_cmd_names.insert( std::make_pair( server::SAVE_FRAME_DECORATED, "SAVE_FRAME_DECORATED" ) );
	
}


server::~server()
{
	stop();
}


void server::start( void )
{
	if( m_initialized )
		return;

	// init thread staff
	pthread_mutex_init( &m_mtx, NULL );
	pthread_mutex_init( &m_in_mtx, NULL );
	pthread_mutex_init( &m_out_mtx, NULL );
	m_quit_thread_flag 	= 0;

	pthread_create( &m_thread, NULL, &server::server_thread, this );

	// ok
	m_initialized = true;
}


void server::stop( void )
{
	if( !m_initialized )
		return;

	// stop thread
	pthread_mutex_lock( &m_mtx );
	m_quit_thread_flag = 1;
	pthread_mutex_unlock( &m_mtx );

	pthread_join( m_thread, NULL );

	// release stuff
	pthread_mutex_destroy( &m_mtx );
	pthread_mutex_destroy( &m_in_mtx );
	pthread_mutex_destroy( &m_out_mtx );

	m_initialized = false;
}


conn_t* server::take_conn( void )
{
	conn_t *conn = NULL;

	pthread_mutex_lock( &m_in_mtx );
	if( m_in_queue.size() > 0 )
	{
		conn = m_in_queue.front();
		m_in_queue.pop_front();
	}
	pthread_mutex_unlock( &m_in_mtx );

	return conn;
}


void server::return_conn( conn_t *cn )
{
	pthread_mutex_lock( &m_out_mtx );
	m_out_queue.push_back( cn );
	pthread_mutex_unlock( &m_out_mtx );

	uint64_t n_val = 1;
	ssize_t wr_res = write( m_res_fd, &n_val, sizeof(n_val) );
	if( wr_res == -1 )
	{
		log_e( "server::return_conn(): Unable to write to pipe: '%s'", strerror(errno) );
	}
}


bool server::send_bcast_msg( int msg, const char *fmt, ... )
{
	va_list	argptr;
	char buf[1024];
	int len = 0;

	if( msg < BCM_DEBUG || msg >= BCM_MAX )
		return false;
	if( !m_bcm_available_map[ msg ] ) // skip silently
		return true;

	len = snprintf( buf, sizeof(buf), "%d:%s:", msg, m_bcm_names[ msg ] );

	va_start( argptr, fmt );
	len += vsnprintf( buf+len, sizeof(buf)-len, fmt, argptr );
	va_end (argptr);

	return send_broadcast( buf, len );
}


bool server::send_broadcast( const char * str, size_t str_len )
{
	int sock;
	struct sockaddr_in broadcast_addr;
	const char *broadcast_ip = m_net_params.bcast_ip;//"255.255.255.255";
	unsigned short broadcast_port = m_net_params.bcast_port;
	int broadcast_permission;
	in_addr_t in_addr = inet_addr(broadcast_ip);//INADDR_BROADCAST; //inet_addr(broadcast_ip);

	// clip string
	if( str_len > 1024 )
		str_len = 1024;

	if( (sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 )
	{
		log_e( "server::send_broadcast(): socket error" );
		return false;
	}

	broadcast_permission = 1;
	if( setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&broadcast_permission, sizeof(broadcast_permission)) < 0 )
	{
		log_e( "server::send_broadcast(): setsockopt error");
		close( sock );
		return false;
	}

	// construct local address structure
	memset(&broadcast_addr, 0, sizeof(broadcast_addr));
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_addr.s_addr = in_addr;
	broadcast_addr.sin_port = htons(broadcast_port);

	// broadcast str in datagram to clients
	if( sendto( sock, str, str_len, 0, (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) != (ssize_t)str_len )
	{
		log_e( "server::send_broadcast(): sendto error" );
		close( sock );
		return false;
	}

	close( sock );

	return true;
}


void server::set_msg_map( const std::map<int, bool>& map )
{
	for( std::map<int, bool>::const_iterator it = map.begin();it != map.end(); ++it )
	{
		if( it->first >= BCM_SRV_STARTED && it->first < BCM_MAX )
			m_bcm_available_map[ it->first ] = it->second;
	}
}


void server::get_msg_map( std::map<int, bool> *map )
{
	map->clear();

	for( int i = BCM_SRV_STARTED;i < BCM_MAX;i++ )
		map->insert( std::make_pair( i, m_bcm_available_map[ i ] ) );
}


void server::set_nonblocking( int sock )
{
	int opts;

	opts = fcntl( sock, F_GETFL );
	if( opts < 0 )
	{
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = (opts | O_NONBLOCK);
	if( fcntl( sock, F_SETFL, opts ) < 0 )
	{
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
}


void server::set_reuseport( int sock )
{
	int on = 1;
	int ret = setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (void *)&on, sizeof(int) );
	if( ret < 0 )
	{
		perror("setsockopt(SO_REUSEADDR)");
		exit(EXIT_FAILURE);
	}
	ret = setsockopt( sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(int) );
	if( ret < 0 )
	{
		perror("setsockopt(SO_KEEPALIVE)");
		exit(EXIT_FAILURE);
	}
}


int server::add_epoll_event( int fd, void *data, int eventfl )
{
	struct epoll_event event;
	memset( &event, 0, sizeof(struct epoll_event) );
	event.events = eventfl;
	event.data.ptr = data;
	if( epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &event) )
	{
		char err_buf[2048];
		strerror_r(errno,err_buf,sizeof err_buf);
		log_e("ADD_EPOLL_EVENT ERROR (%d): %s", errno, err_buf);
		return -1;
	}
	return 0;
}


int server::mod_epoll_event( int fd, void *data, int eventfl )
{
	struct epoll_event event;
	memset( &event, 0, sizeof(struct epoll_event) );
	event.events = eventfl;
	event.data.ptr = data;
	if( epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &event) )
	{
		log_e("MOD_EPOLL_EVENT ERROR (%d,%d,%d,%d): %s", errno, m_epollfd, fd, eventfl, strerror(errno));
		return -1;
	}
	return 0;
}


int server::del_epoll_event( int fd )
{

	struct epoll_event event;
	memset( &event, 0, sizeof(struct epoll_event) );
	event.events = EPOLLIN|EPOLLOUT;
	event.data.ptr = 0;
	if( epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &event) )
	{
		log_e("EP: delete connection from epoll failed: %s, %d, %d", strerror( errno), errno, fd);
		return -1;
	}
	return 0;
}


conn_t *server::get_conn( void )
{
	conn_t *pconn;

	if( m_free_conns.size() )
	{
		pconn = m_free_conns.back();
		m_free_conns.pop_back();
	}
	else
	{
		pconn = new conn_t;
	}
	pconn->init();

	m_conns.insert( pconn );

 return pconn;
}


int  server::close_conn( conn_t *conn )
{
	int ret = -1;
 	 int old_fd = conn->fd;

	ret = del_epoll_event( conn->fd );
	if( ret == -1 )
		return ret;
	conn->close();
	m_conns.erase( conn );
	m_free_conns.push_back( conn );
	log_i( "Deleted[%d], opened: %d conns", old_fd, m_conns.size() );

	return ret;
}

size_t server::get_num_active_connections( void ) const
{
	return m_conns.size();
}


int  server::do_read( conn_t *pconn )
{
	int ret = 0;
	ssize_t rd_len = pconn->read_len < proto_hdr::HDR_SIZE ? (proto_hdr::HDR_SIZE-pconn->read_pos) : pconn->read_len - pconn->read_pos;
	ssize_t rd_cnt = 0;

	if( rd_len )
	{
		do
		{
			rd_cnt = read( pconn->fd, pconn->read_buf + pconn->read_pos, rd_len );
			pconn->read_pos += rd_cnt;
		}while( rd_cnt < 0 && errno == EINTR );
	}

	if( rd_cnt <= 0 )
	{
		ret = close_conn( pconn );
	}
	else
	{
		// first read of header
		if( pconn->read_pos >= proto_hdr::HDR_SIZE && pconn->read_len == 0 )
		{
			const proto_hdr *r_hdr = proto_hdr::get( pconn->read_buf );
			pconn->read_len = r_hdr->data_sz + proto_hdr::HDR_SIZE;
			if( !proto_hdr::check( pconn->read_buf ) )
			{
				log_e( "SRV: Protocol error. Signature is wrong" );
				ret = close_conn( pconn );
				return ret;
			}
			std::map< int, const char* >::const_iterator cmd = m_cmd_names.find( r_hdr->cmd );
			log_i( "SRV: header parsed: cmd = '%s' read_len = %d", cmd != m_cmd_names.end() ? cmd->second : "UNKNOWN", pconn->read_len );
			if( pconn->read_len > (ssize_t)sizeof(pconn->read_buf) )
			{
				pconn->read_len = sizeof(pconn->read_buf);
				log_e( "SRV: Data stripped to = %d", sizeof(pconn->read_buf) );
			}
		}

		if( pconn->read_pos >= pconn->read_len ) // whole packet is read
		{
			pconn->answer_sz = 0;

			del_epoll_event( pconn->fd );
			m_conns.erase( pconn );
			pthread_mutex_lock( &m_in_mtx );
			m_in_queue.push_back( pconn );
			pthread_mutex_unlock( &m_in_mtx );

			// emit event
			emit do_command();
		}
	}

 return ret;
}


int  server::do_write( conn_t *pconn )
{
	int ret = 0;
	ssize_t wr_len = pconn->write_len - pconn->write_pos;
	ssize_t wr_cnt = 0;

	if( wr_len )
	{
		do
		{
			wr_cnt = write( pconn->fd, pconn->write_buf + pconn->write_pos, wr_len );
			pconn->write_pos += wr_cnt;
		}while( wr_cnt < 0 && errno == EINTR );
	}

	if( wr_cnt < 0 )
	{
		ret = close_conn( pconn );
	}
	else
	{
		if( pconn->write_pos < pconn->write_len )
		{
			ret = mod_epoll_event( pconn->fd, pconn, EPOLLOUT );
			//log_i("EPOLLOUT - out again");
		}
		else
		{
			pconn->reset();
			ret = mod_epoll_event( pconn->fd, pconn, EPOLLIN );
			//log_i("EPOLLOUT - IN");
		}
	}

	return ret;
}








//-----------------------------------------------------------------------------
void *server::server_thread( void *param )
{
	server *srv = reinterpret_cast<server *>(param);
	int quit = 0;
	conn_t *pconn = NULL;
	int ret = -1;

	//const char *sock_name = "/tmp/lg_ss";
	struct epoll_event *ev, events[EPOLL_QUEUE_LEN];
	int listen_sock, conn_sock, nfds;
	struct sockaddr_un  ucli_addr, userv_addr;
	struct sockaddr_in  icli_addr, iserv_addr;
	struct sockaddr *serv_addr, *cli_addr;
	socklen_t serv_len, cli_len;
	(void)serv_len;

 	send_bcast_msg( BCM_SRV_STARTED );

	if (m_net_params.use_tcp) {
		memset( (char *) &iserv_addr, 0, sizeof(iserv_addr) );
		iserv_addr.sin_family = AF_INET;
		iserv_addr.sin_addr.s_addr = INADDR_ANY;
		iserv_addr.sin_port = htons(m_net_params.listen_port);
		serv_len = sizeof(iserv_addr);
		cli_len = sizeof(icli_addr);
		serv_addr = (struct sockaddr *)&iserv_addr;
		cli_addr = (struct sockaddr *)&icli_addr;
	} else {
		memset( (char *) &userv_addr, 0, sizeof(userv_addr) );
		userv_addr.sun_family = AF_UNIX;
		strcpy( userv_addr.sun_path, m_net_params.listen_socket );
		serv_len = sizeof(userv_addr);
		cli_len = sizeof(ucli_addr);
		unlink( m_net_params.listen_socket );
		serv_addr = (struct sockaddr *)&userv_addr;
		cli_addr = (struct sockaddr *)&ucli_addr;
	}
 	// create listener

	listen_sock = socket( serv_addr->sa_family, SOCK_STREAM, 0 );
 	if( listen_sock < 0 )
 	{
 		perror("creating socket");
 		exit(EXIT_FAILURE);
 	}
 	srv->set_reuseport( listen_sock );
 	srv->set_nonblocking( listen_sock );

	if( bind( listen_sock, serv_addr, serv_len ) < 0 )
 	{
 		perror("binding socket");
 		exit(EXIT_FAILURE);
 	}
 	listen(listen_sock, 5);

	if (m_net_params.use_tcp) {
		log_i("SRV: listening on TCP port %d", m_net_params.listen_port);
	} else {
		log_i("SRV: listening on Unix socket %s", m_net_params.listen_socket);
	}

 	//-------------------------------------------------------------
 	// create EPOLL
 	srv->m_epollfd = epoll_create(EPOLL_QUEUE_LEN);
 	if( srv->m_epollfd == -1)
 	{
 		perror("epoll_create");
 		exit(EXIT_FAILURE);
 	}
 	// begin listening
 	ret = srv->add_epoll_event( listen_sock, reinterpret_cast<void*>(listen_sock), EPOLLIN );
 	if( ret == -1)
 		exit(EXIT_FAILURE);

 	srv->m_res_fd = eventfd( 0, 0 );
    if( srv->m_res_fd < 0 )
    {
        log_e( "server::server_thread(): eventfd() failed. '%s'", strerror( errno ) );
    }
    else
    {
    	srv->set_nonblocking( srv->m_res_fd );
    }
    ret = srv->add_epoll_event( srv->m_res_fd, reinterpret_cast<void*>(srv->m_res_fd), EPOLLIN );
	if( ret == -1)
		exit(EXIT_FAILURE);


 	while( true )
 	{
  		// check for pause and quit
  		pthread_mutex_lock( &srv->m_mtx );
  		quit = srv->m_quit_thread_flag;
  		pthread_mutex_unlock( &srv->m_mtx );

  		// check for quit command
  		if( quit )
  			break;

  		// wait for socket events
  		nfds = epoll_wait( srv->m_epollfd, events, EPOLL_QUEUE_LEN, 500 );
  		if( nfds == -1 && errno != EINTR )
  		{
  			perror("epoll_pwait");
  			exit(EXIT_FAILURE);
  		}

  		// process events
  		for( int n = 0;n < nfds;n++ )
  		{
  			ev = &events[n];
  			// accept
  			if( ev->data.fd == listen_sock )
  			{
				socklen_t clen = cli_len;
				conn_sock = accept(listen_sock,	cli_addr, &clen);
  				if( conn_sock == -1 )
  				{
  					perror("accept");
  					exit(EXIT_FAILURE);
  				}
  				if( srv->m_conns.size() == EPOLL_QUEUE_LEN-1 )
  				{
  					log_i( "Event array is full. conn.cnt = %d", srv->m_conns.size() );
  					close( conn_sock );
  					continue;
  				}
  				srv->set_nonblocking( conn_sock );

  				pconn = srv->get_conn();
  				pconn->fd = conn_sock;

  				ret = srv->add_epoll_event( pconn->fd, pconn, EPOLLIN );
  				if( ret == -1 )
  					exit(EXIT_FAILURE);
  				log_i( "Connected[%d], opened: %d conns", pconn->fd, srv->m_conns.size() );
  			}
  			else
  			if( ev->data.fd == srv->m_res_fd )
  			{
				uint64_t notify_val;
				ssize_t rd = 0;
				do
				{
					rd = read( srv->m_res_fd, &notify_val, sizeof(notify_val) );
				}while( rd > 0 );

  				pthread_mutex_lock( &srv->m_out_mtx );
  				for( size_t i = 0;i < srv->m_out_queue.size();i++ )
				{
					pconn = srv->m_out_queue.front();
					srv->m_out_queue.pop_front();

					// write answer
					pconn->reset();
					int eflags = 0;
					if( pconn->answer_sz > 0 )
					{
						const proto_hdr *r_hdr = proto_hdr::get( pconn->read_buf );
						proto_hdr::set( pconn->write_buf, r_hdr->cmd, pconn->answer_sz );
						pconn->write_len = proto_hdr::HDR_SIZE + pconn->answer_sz;
						eflags = EPOLLOUT;
					}
					else
						eflags = EPOLLIN;

					srv->m_conns.insert( pconn );

					ret = srv->add_epoll_event( pconn->fd, pconn, eflags );
					if( ret == -1 )
						exit(EXIT_FAILURE);
				}
  				pthread_mutex_unlock( &srv->m_out_mtx );
  			}
  			else
  			{
  				pconn = (conn_t *)ev->data.ptr;

  				// READING
  				if( ev->events & EPOLLIN )
  				{
  					ret = srv->do_read( pconn );
  					if( ret < 0 )
  					  	exit(EXIT_FAILURE);
  				}
  				else
  				// WRITING
  				if( ev->events & EPOLLOUT )
  				{
  					ret = srv->do_write( pconn );
  					if( ret < 0 )
  						exit(EXIT_FAILURE);
  				}
  				else
  				// ERRORS (disconnects)
  				if( (ev->events & EPOLLHUP) || (ev->events & EPOLLERR) )
  				{
  					ret = srv->close_conn( pconn );
  					if( ret < 0 )
  						exit(EXIT_FAILURE);
  				}
  			}
  		}
 	}

 	// cleanup all
 	for( set<conn_t *>::iterator it = srv->m_conns.begin();
 		 it != srv->m_conns.end();
 		 ++it )
 	{
 		pconn = (*it);
 		srv->del_epoll_event( pconn->fd );
 		close( pconn->fd );
 		delete (*it);
 	}
 	srv->m_conns.clear();

 	for( vector<conn_t *>::iterator it = srv->m_free_conns.begin();
 	 	 it != srv->m_free_conns.end();
 	 	 ++it )
 	{
 		pconn = (*it);
 		delete (*it);
 	}
 	srv->m_free_conns.clear();

 	pthread_mutex_lock( &srv->m_in_mtx );
 	log_i( "SRV:in_queue.size(): %zu", srv->m_in_queue.size() );
 	for( deque<conn_t *>::iterator it = srv->m_in_queue.begin();
 		it != srv->m_in_queue.end();
 	 	++it )
	{
		pconn = (*it);
		delete (*it);
	}
 	srv->m_in_queue.clear();
 	pthread_mutex_unlock( &srv->m_in_mtx );

 	pthread_mutex_lock( &srv->m_out_mtx );
 	log_i( "SRV:out_queue.size(): %zu", srv->m_out_queue.size() );
	for( deque<conn_t *>::iterator it = srv->m_out_queue.begin();
		it != srv->m_out_queue.end();
		++it )
	{
		pconn = (*it);
		delete (*it);
	}
	srv->m_out_queue.clear();
	pthread_mutex_unlock( &srv->m_out_mtx );

 	close( srv->m_res_fd );

 	close( srv->m_epollfd );
	close( listen_sock );

	unlink( m_net_params.listen_socket );

 	send_bcast_msg( BCM_SRV_STOPPED );

 	log_i( "SRV:stopped" );

 return NULL;
}
