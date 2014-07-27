#pragma once
#include "string.hpp"
#include "aio_socket_stream.hpp"
#include "ofstream.hpp"
#include "lib_msn.h"

class LIB_MSN_API CMsnChat : public acl::aio_open_callback
{
public:
	CMsnChat(const char* class_name = "unknown");
	virtual ~CMsnChat(void);

	/**
	* 异步连接远程服务器，同时异步解析服务器域名
	* 该过程因为包含了域名的异步解析过程，所以连接的结果是通过回调
	* 来通知的
	* @param domain {const char*} 服务器域名(也可以是IP地址)
	* @param port {unsigned short} 服务器端口
	* @param timeout {int} 连接超时时间
	*/
	void Open(const char* domain, unsigned short port, int timeout);

	/**
	* 异步连接远程服务器，地址格式为：ip:port；当然也可以是: domain:port
	* 但这种情况下的域名解析是同步的，会导致阻塞条件发生
	* @param addr {const char*} 服务器地址，格式：ip:port
	* @param timeout {int} 连接超时时间
	* @return {bool} 如果返回 false 则说明一连就出错，否则连接处于正在
	*  执行过程中，连接的成功与否是在回调中通知的
	*/
	bool Open(const char* addr, int timeout);

	/**
	* 关闭连接
	*/
	void Close();

	/**
	* 发送数据
	* @param data {const char*} 数据指针地址
	* @param len {size_t} 数据长度
	* @return {bool} 如果连接未打开调用此函数则返回 false
	*/
	bool Send(const char* data, size_t len);

	/**
	* 读一行数据
	* @param nonl {bool} 是否自动去掉\r\n
	* @return {bool} 如果连接未打开调用此函数则返回 false
	*/
	bool Gets(bool nonl = true);

	/**
	* 读规定长度的数据或有数据可读就读
	* @param len {int} 读取数据的长度，如果该值为0则表示有数据可读就
	*  读数据并返回读到的数据及长度
	* @return {bool} 如果连接未打开调用此函数则返回 false
	*/
	bool Read(int len = 0);

	/**
	* 当DNS查询返回后的回调函数
	* @param ip {const char*} 查询到的IP地址
	*/
	void OnDnsResult(const char* ip);

	/**
	* 设置流操作的调试状态
	* @param onoff {bool}
	*/
	void Debug(bool onoff);
protected:
	//////////////////////////////////////////////////////////////////////////
	// 虚函数

	/**
	* 当远程连接服务器成功后的虚函数
	*/
	virtual void OnOpen() {}

	/**
	* 当读到要求的数据后的回调函数
	* @param data {char*} 数据地址指针
	* @param len {int} 数据长度
	*/
	virtual void OnRead(char* data, int len) {}

	/**
	* 当发送成功后的回调函数
	*/
	virtual void OnSend() {}

	/**
	* 当连接超时或读超时的回调函数
	*/
	virtual void OnTimeout() {}

	/**
	* 当连接关闭时的回调函数
	*/
	virtual void OnClose() {}
private:
	// 基类 aio_open_callback 的虚函数
	virtual bool read_callback(char* data, int len);
	virtual bool write_callback();
	virtual void close_callback();
	virtual bool timeout_callback();
	virtual bool open_callback();

	acl::string class_name_;
	acl::string addr_;
	acl::aio_socket_stream* stream_;
	unsigned short port_;
	int conn_timeout_;
	bool debug_;
	acl::ofstream* debug_fpout_;
	bool opened_;

	void logger_format(const char* fmt, ...);
};
