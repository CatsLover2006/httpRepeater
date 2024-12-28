#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <iostream>

httplib::Server svr;

int main() {
	svr.Get(R"(/(.+))", [&](const httplib::Request &req, httplib::Response &res) {
		std::string url = req.matches[1];
		res.set_content(url, "text/plain");
	});
	svr.listen("0.0.0.0", 8080);
	return 0;
}
