#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <iostream>
#include <stdlib.h>

httplib::Server svr;

int main() {
	svr.Get(R"(/(https?://.*?)(/.*))", [&](const httplib::Request &req, httplib::Response &res) {
		std::string domain = req.matches[1];
		std::string file = req.matches[2];
		std::cout << domain << std::endl << file << std::endl;
		httplib::Client cli_head(domain);
		cli_head.set_follow_location(true);
		auto head = cli_head.Head(file);
		res.set_content_provider(atoll(head->headers.find("Content-Length")->first.c_str()),
			head->headers.find("Content-Type")->first,
			[&, domain, file](size_t offset, size_t length, httplib::DataSink &sink) {
				httplib::DataSink* toWrite = &sink;
				httplib::Client cli(domain);
				cli.set_follow_location(true);
				std::cout << "got sink" << std::endl;
				volatile size_t i = 0;
				auto get = cli.Get(file,
					[&](const char *data, size_t data_length) {
						std::cout << "writing" << std::endl;
						toWrite->write(data, data_length);
						i += data_length;
						return true;
					});
				std::cout << "sent request" << std::endl;
				while (i < length) usleep(1000);
				return true;
			});
		return res.status = httplib::StatusCode::OK_200;
	});
	svr.listen("0.0.0.0", 8080);
	return 0;
}
