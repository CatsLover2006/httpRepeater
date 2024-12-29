#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <iostream>

httplib::Server svr;

int main() {
	svr.Get(R"(/(https?://.*?)(/.*))", [&](const httplib::Request &req, httplib::Response &res) {
		std::string domain = req.matches[1];
		std::string file = req.matches[2];
		std::cout << domain << std::endl << file << std::endl;
		try {
			httplib::Client cli(domain);
			cli.set_follow_location(true);
			volatile std::string out = "";
			volatile bool downloading = true;
			volatile bool hold = false;
			res.set_chunked_content_provider(get->headers->find("Content-Type"),
				[&](size_t offset, httplib::DataSink &sink) {
					bool temp_downloading = true;
					std::string write = "";
					while (temp_downloading) {
						hold = true;
						write = out;
						out = "";
						hold = false;
						if (write.size() != 0)
							sink.write(write, write.size());
					}
      				sink.done();
				return true;
			}
						
			auto get = cli.Get(file	[&](const char *data, size_t data_length) {
				while (hold); // Stall
				out.append(data, data_length);
				return true;
			}));
`			downloading = false;
		} catch (...) {
			return res.status = httplib::StatusCode::NotAcceptable_406;
		}
	});
	svr.listen("0.0.0.0", 8080);
	return 0;
}
