#include "tf_http.h"
#include <json/value.h>
#include <json/reader.h>
#include "xxtea.h"
#include "md5.h"
#include "base64.h"
#include <fstream>


#if !defined (_WIN32) && !defined (_WIN64)  
#define LINUX  
#include <unistd.h>
#else  
#define WINDOWS  
#include <windows.h>  
#endif  
unsigned core_count()
{
	unsigned count = 1;
#if defined (LINUX)  
	count = sysconf(_SC_NPROCESSORS_CONF);
#elif defined (WINDOWS)  
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	count = si.dwNumberOfProcessors;
#endif  
	return count;
}

void DefaultHandler(evpp::EventLoop* loop,
	const evpp::http::ContextPtr& ctx,
	const evpp::http::HTTPSendResponseCallback& cb) {
	/*std::stringstream oss;
	oss << "func=" << __FUNCTION__ << " OK"
		<< " ip=" << ctx->remote_ip() << "\n"
		<< " uri=" << ctx->uri() << "\n"
		<< " body=" << ctx->body().ToString() << "\n";
	ctx->AddResponseHeader("Content-Type", "application/octet-stream");
	ctx->AddResponseHeader("Server", "evpp");*/
	cb("");
}

tf_http::tf_http()
{
	Json::Value root;
	Json::Reader reader;
    std::ifstream ifs("./config.json");
	if (ifs.is_open())
	{
		reader.parse(ifs, root, false);
		ifs.close();
	}
	else
	{
		root["http_port"] = 8080;
		auto cfgjson=root.toStyledString();
		std::fstream file1;
		file1.open("./config.json", std::fstream::out);
		if (file1.is_open())
		{
			file1.write(cfgjson.data(), cfgjson.length());
			file1.close();
		}
	}
	evpp::http::Server tfhttpser(core_count()*2+1);
	tfhttpser.SetThreadDispatchPolicy(evpp::ThreadDispatchPolicy::kIPAddressHashing);
	tfhttpser.RegisterDefaultHandler(&DefaultHandler);
	tfhttpser.RegisterHandler("/login",
		[](evpp::EventLoop* loop,
			const evpp::http::ContextPtr& ctx,
			const evpp::http::HTTPSendResponseCallback& cb) {
		auto sj = ctx->body().ToString();
		Json::Value root;
		Json::Reader reader;
		bool isok=reader.parse(sj, root, false);
		if (isok)
		{
			auto unionid=root["unionid"].asString();
			auto timestamp = root["timestamp"].asString();
			auto sign = root["sign"].asString();
			MD5 m_md5(unionid + timestamp);
			auto md5str=m_md5.toStr();
			if (md5str== sign)
			{
				auto signtime = base64_decode(timestamp);
				xxtea_long relen = signtime.length();
				unsigned char * intime = new unsigned char[relen + 1];
				memcpy((char *)intime, signtime.data(), relen);
				intime[relen] = 0;
				unsigned char * jmtime = xxtea_decrypt(intime, relen, (unsigned char *)"xxtea678", 8, &relen);
				auto dqtm = time(0);
				if (relen)
				{
					auto ytime = atoi((char *)jmtime);
					if (dqtm - ytime < 5)//下面逻辑需要修改
					{
						Json::Value iproot;
						iproot["ip"] = "192.168.0.149";
						iproot["port"] = "8888";
						cb(iproot.toStyledString());
					}
				}
			}
		
		}
	}
	);

	tfhttpser.RegisterHandler("/tf_relaying",
		[](evpp::EventLoop* loop,
			const evpp::http::ContextPtr& ctx,
			const evpp::http::HTTPSendResponseCallback& cb) 
		{
			auto sj = ctx->body().ToString();
			if (sj.size())
			{
				
			}
			cb("{\"Err\":0}");
		}
	);
	if (tfhttpser.Init(root["http_port"].asInt()))
	{
		tfhttpser.Start();
	}
}


tf_http::~tf_http()
{

}
