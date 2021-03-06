// socket_d.c
#include <daemon.h>
#include <socket.h>
#include <socket_err.h>

#define monitor(x)	CHANNEL_D->channel_broadcast("nch", "SOCKET_D: " + x)
#define PROCESS_TIMEOUT		30

mapping sockets = allocate_mapping(0);

mapping query_sockets()
{
	return sockets;
}

private void connect_timeout(int fd)
{
	if( undefinedp(sockets[fd]) ) return;
	if( objectp(sockets[fd]["owner"]) )
		evaluate(sockets[fd]["stat_callback"], "Unable to connect to remote host: Connection timeout");
	socket_close(fd);
	return;
}

private void receive_data(int fd, string msg)
{ 
	if( !sockets[fd]["owner"] )
	{
		socket_close(fd);
		return;
	}
	evaluate(sockets[fd]["rece_callback"], msg);
	return;
}

private void write_data(int fd)
{
	sockets[fd]["write_fd"] = fd;
	
	if( !undefinedp(sockets[fd]["connect_handle"]) )
	{
		remove_call_out(sockets[fd]["connect_handle"]);
		map_delete(sockets[fd], "connect_handle");
	}

	// 如果收到 socket write callback 時
	// buffer 內有訊息未傳送則立即傳送，並清除 buffer
	if( sockets[fd]["write_buffer"] )
	{
		socket_write(fd, sockets[fd]["write_buffer"]);
		sockets[fd]["write_buffer"] = 0;
	}
	return;
}


void socket_close(int fd)
{
	monitor("socket_close: " + fd);
	efun::socket_close(fd);

	if( !undefinedp(sockets[fd]["connect_handle"]) )
	{
		remove_call_out(sockets[fd]["connect_handle"]);
		map_delete(sockets[fd], "connect_handle");
	}
	map_delete(sockets, fd);
	return;
}

void socket_send(int fd, mixed msg)
{
	int res;

#ifdef DEBUG
	monitor("送出訊息: " + msg);
#endif

	if( !msg || msg == "" ) return; 
	// 尚未收到 socket write callback 前，將欲傳送的訊息存入 buffer
	if( !sockets[fd]["write_fd"] )
	{
		if( !sockets[fd]["write_buffer"] )
			sockets[fd]["write_buffer"] = msg;
		else
			sockets[fd]["write_buffer"] += msg;
		return;
	}
	
	// 如果 buffer 中有訊息尚未送出，則與本次訊息同時送出
	if( sockets[fd]["write_buffer"] )
	{
		sockets[fd]["write_buffer"] += msg;
		res = socket_write(fd, sockets[fd]["write_buffer"]);
	}
	else res = socket_write(fd, msg);

	switch(res) {

	case EESUCCESS:		/* 成功 */
	case EECALLBACK:	/* 等待完成 */
		sockets[fd]["write_buffer"] = 0;
		break;
	case EEALREADY:		/* 程序已進行 */
	case EEWOULDBLOCK:	/* 程序停滯 */
	case EESEND:		/* 傳送資料錯誤 */
		if( !sockets[fd]["write_buffer"] )
			sockets[fd]["write_buffer"] = msg;

		call_out((: socket_send :), 1, fd, "");
		break;
	default:		/* 其他錯誤 */
		evaluate(sockets[fd]["stat_callback"], socket_error(res));
		socket_close(fd);
		break;
	}
}

int socket_open(string host, int port, function rece_callback, function stat_callback)
{
	int socket_fd, res;
	string addr;

	monitor("socket_open: " + sprintf("%s %d %O %O", host, port, rece_callback, stat_callback));

	socket_fd = socket_create(STREAM, (: receive_data :), (: socket_close :));
	if( socket_fd < 0 )
	{
		evaluate(stat_callback, "socket_create: " + socket_error(socket_fd));
		return -1;
	}

	addr = sprintf("%s %d", host, port);
	res = socket_connect(socket_fd, addr, (: receive_data :), (: write_data :));

	if( res != EESUCCESS )
	{
		evaluate(stat_callback, "socket_connect: " + socket_error(res));
		socket_close(socket_fd);
		return -1;
	}

	sockets[socket_fd] = allocate_mapping(0);
	sockets[socket_fd]["addr"] = host;
	sockets[socket_fd]["port"] = port;
	sockets[socket_fd]["owner"] = previous_object();
	sockets[socket_fd]["rece_callback"] = rece_callback;
	sockets[socket_fd]["stat_callback"] = stat_callback;
	sockets[socket_fd]["connect_handle"] = call_out((: connect_timeout :), PROCESS_TIMEOUT, socket_fd);
	return socket_fd;
}
string query_name()
{
	return "SOCKET 系統(SOCKET_D)";
}
